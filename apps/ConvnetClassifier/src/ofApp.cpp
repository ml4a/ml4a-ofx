#include "ofApp.h"

const ofColor backgroundPlotColor = ofColor(50,50,50,255);

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowShape(640, 480);
    cam.initGrabber(320, 240);
    
    largeFont.load("verdana.ttf", 12, true, true);
    largeFont.setLineHeight(14.0f);
    smallFont.load("verdana.ttf", 10, true, true);
    smallFont.setLineHeight(12.0f);
    hugeFont.load("verdana.ttf", 20, true, true);
    hugeFont.setLineHeight(38.0f);
    
    //Initialize the training and info variables
    infoText = "";
    trainingClassLabel = 1;
    predictedClassLabel = 0;
    trainingModeActive = false;
    recordTrainingData = false;
    predictionModeActive = false;
    drawInfo = true;
    
    //Set the inputs
    trainingData.setNumDimensions( 4096 );
    
    KNN knn; /*Other classifiers: AdaBoost adaboost; DecisionTree dtree; KNN knn; GMM gmm; ANBC naiveBayes; MinDist minDist; RandomForests randomForest; Softmax softmax; SVM svm; */
    pipeline.setClassifier( knn );
    
    ccv.setup("../../../../data/image-net-2012.sqlite3");
    if (!ccv.isLoaded()) return;
    
    //OSC
    // default settings
    oscDestination = DEFAULT_OSC_DESTINATION;
    oscAddress = DEFAULT_OSC_ADDRESS;
    oscPort = DEFAULT_OSC_PORT;
    sender.setup(oscDestination, oscPort);
    
    //GUI
    bTrain.addListener(this, &ofApp::trainClassifier);
    bSave.addListener(this, &ofApp::save);
    bLoad.addListener(this, &ofApp::load);
    bClear.addListener(this, &ofApp::clear);
    
    gui.setup();
    gui.add(sliderClassLabel.setup("Class Label", 1, 1, 9));
    gui.add(tRecord.setup("Record", false));
    gui.add(bTrain.setup("Train"));
    gui.add(bSave.setup("Save"));
    gui.add(bLoad.setup("Load"));
    gui.add(bClear.setup("Clear"));
    gui.setPosition(10,10);
    
}

//--------------------------------------------------------------
void ofApp::update() {
    if (!ccv.isLoaded()) {
        ofDrawBitmapString("Network file not found! Check your data folder to make sure it exists.", 20, 20);
        return;
    }
    
    cam.update();
    
    VectorFloat inputVector(4096);
    featureEncoding = ccv.encode(cam, ccv.numLayers()-1);
    
    for (int i=0; i<featureEncoding.size(); i++) {
        inputVector[i] =  featureEncoding[i];
    }
    
    if( tRecord ) {
        trainingData.addSample( sliderClassLabel, inputVector );
    }
    
    if( predictionModeActive ){
        if( pipeline.predict( inputVector ) ){
            predictedClassLabel = pipeline.getPredictedClassLabel();
            predictionPlot.update( pipeline.getClassLikelihoods() );
            sendOSC();
        }else{
            infoText = "ERROR: Failed to run prediction!";
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackground(150);
    ofSetColor(255);
    cam.draw(270, 200);
    
    //Draw the info text
    int graphX, graphY = 10;
    int graphW = ofGetWidth() - graphX*2;
    int graphH = 150;
    float infoX = 10;
    float infoW = 200;
    float textX = 10;
    float textY = gui.getHeight() + 10;
    float textSpacer = smallFont.getLineHeight() * 1.5;
    
    ofFill();
    ofSetColor( 255, 255, 255 );
    
    ofDrawBitmapString( "MFCCS CLASSIFIER EXAMPLE", textX, textY +20); textY += textSpacer*2;
    ofDrawBitmapString( "Num Samples: " + ofToString( trainingData.getNumSamples() ), textX, textY ); textY += textSpacer;
    textY += textSpacer;
    
    ofDrawBitmapString( infoText, textX, textY ); textY += textSpacer;
    
    //Draw the graph
    graphX = infoX + infoW + 60;
    graphW = 320;
    
    if( pipeline.getTrained() ){
        ofSetLineWidth(1);
        predictionPlot.draw( graphX, graphY, graphW, graphH ); graphY += graphH * 1.1;
        std::string txt = "Predicted Class: " + ofToString( predictedClassLabel );
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        ofSetColor(0,255,0);
        hugeFont.drawString( txt, 10, ofGetHeight() - bounds.height*3 );
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){ //Optional key interactions
    
    switch ( key) {
        case '1':
            sliderClassLabel = 1;
            break;
        case '2':
            sliderClassLabel = 2;
            break;
        case '3':
            sliderClassLabel = 3;
            break;
        case '4':
            sliderClassLabel = 4;
            break;
        case '5':
            sliderClassLabel = 5;
            break;
        case '6':
            sliderClassLabel = 6;
            break;
        case '7':
            sliderClassLabel = 7;
            break;
        case '8':
            sliderClassLabel = 8;
            break;
        case '9':
            sliderClassLabel = 9;
            break;
            
        case 's':
            save();
            break;
        case 'l':
            load();
            break;
        case 't':
            trainClassifier();
            break;
        case 'c':
            clear();
            break;
        case 'r':
            tRecord =! tRecord;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::trainClassifier() {
    ofLog(OF_LOG_NOTICE, "Training...");
    tRecord = false;
    if( pipeline.train( trainingData ) ){
        infoText = "Pipeline Trained";
        std::cout << "getNumClasses: " << pipeline.getNumClasses() << std::endl;
        predictionPlot.setup( 500, pipeline.getNumClasses(), "prediction likelihoods" );
        predictionPlot.setDrawGrid( true );
        predictionPlot.setDrawInfoText( true );
        predictionPlot.setFont( smallFont );
        predictionPlot.setBackgroundColor( ofColor(50,50,50,255));
        predictionModeActive = true;
    }else infoText = "WARNING: Failed to train pipeline";
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::save() {
    if( trainingData.save( ofToDataPath("TrainingData.grt") ) ){
        infoText = "Training data saved to file";
    }else infoText = "WARNING: Failed to save training data to file";
}

//--------------------------------------------------------------
void ofApp::load() {
    if( trainingData.load( ofToDataPath("TrainingData.grt") ) ){
        infoText = "Training data loaded from file";
        trainClassifier();
    }else infoText = "WARNING: Failed to load training data from file";
}

//--------------------------------------------------------------
void ofApp::clear() {
    trainingData.clear();
    infoText = "Training data cleared";
    predictionModeActive = false;
}

//--------------------------------------------------------------
void ofApp::sendOSC() {
    ofxOscMessage m;
    m.setAddress(oscAddress);
    m.addIntArg(pipeline.getPredictedClassLabel());
    sender.sendMessage(m, false);
}
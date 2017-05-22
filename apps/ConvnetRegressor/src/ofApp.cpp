#include "ofApp.h"

const ofColor backgroundPlotColor = ofColor(50,50,50,255);

//--------------------------------------------------------------
void ofApp::setup() {
    
    string ccvPath = ofToDataPath("../../../../data/image-net-2012.sqlite3");
    
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
    
    ccv.setup(ccvPath);
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
    bAddSlider.addListener(this, &ofApp::addSlider);
    
    gui.setup();
    gui.setName("Convnet regressor");
    gui.add(tRecord.setup("Record", false));
    gui.add(bTrain.setup("Train"));
    gui.add(tPredict.setup("Predict", false));
    gui.add(bSave.setup("Save"));
    gui.add(bLoad.setup("Load"));
    gui.add(bAddSlider.setup("Add Slider"));
    
    addSlider();
    
    gui.setPosition(10,10);
    
}

//--------------------------------------------------------------
void ofApp::addSlider() {
    if (pipeline.getNumTrainingSamples() > 0) {
        ofLog(OF_LOG_ERROR, "Can't add slider, already have training samples");
        return;
    }
    ofParameter<float> newSlider;
    gui.add(newSlider.set("y"+ofToString(values.size()), 0.5, 0.0, 1.0));
    values.push_back(newSlider);
    trainingData.setInputAndTargetDimensions( SIZE_INPUT_VECTOR, values.size() );
    setupRegressor();
}

//--------------------------------------------------------------
void ofApp::setupRegressor() {
    unsigned int numInputNeurons = trainingData.getNumInputDimensions();
    unsigned int numHiddenNeurons = 10;
    unsigned int numOutputNeurons = 1; //1 as we are using multidimensional regression
    
    //Initialize the MLP
    MLP mlp;
    mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::SIGMOID, Neuron::SIGMOID );
    
    //Set the training settings
    mlp.setMaxNumEpochs( 100 ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
    mlp.setMinChange( 1.0e-10 ); //This sets the minimum change allowed in training error between any two epochs
    mlp.setLearningRate( 0.01 ); //This sets the rate at which the learning algorithm updates the weights of the neural network
    mlp.setNumRandomTrainingIterations( 1 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
    mlp.setUseValidationSet( true ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
    mlp.setValidationSetSize( 20 ); //Use 20% of the training data for validation during the training phase
    mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
    
    //The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e. [0.0 1.0])
    mlp.enableScaling( true );
    
    pipeline << MultidimensionalRegression(mlp,true);
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
        VectorFloat targetVector(values.size());
        for (int p=0; p<values.size(); p++) {
            targetVector[p] = values[p];
        }
        
        if( !trainingData.addSample(inputVector, targetVector) ){
            infoText = "WARNING: Failed to add training sample to training data!";
        }

    }
    
    if( tPredict ){
        if( pipeline.predict( inputVector ) ){
            
            for (int p=0; p<values.size(); p++) {
                values[p] = pipeline.getRegressionData()[p];
            }
            
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
    
    ofDrawBitmapString( "Num Samples: " + ofToString( trainingData.getNumSamples() ), textX, textY ); textY += textSpacer;
    textY += textSpacer;
    
    ofDrawBitmapString( infoText, textX, textY ); textY += textSpacer;
    
    
    //Draw the graph
    graphX = infoX + infoW + 60;
    graphW = 320;
    
    if( pipeline.getTrained() ){
        ofSetLineWidth(1);
        predictionPlot.draw( graphX, graphY, graphW, graphH ); graphY += graphH * 1.1;
        std::string txt = "__";//"Predicted Regression: " + ofToString( myValue1 )+", "+ofToString( myValue2);
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        ofSetColor(0,255,0);
        hugeFont.drawString( txt, 10, ofGetHeight() - bounds.height*3 );
    }
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){ //Optional key interactions
    switch ( key) {
        case 's':
            save();
            break;
        case 'l':
            load();
            break;
        case 't':
            //trainClassifier();
            break;
        case 'c':
            clear();
            break;
        case 'r':
            tRecord =! tRecord;
            break;
        case 'p':
            tPredict =! tPredict;
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
    }else infoText = "WARNING: Failed to train pipeline";
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::save() {
    if( trainingData.save( ofToDataPath("TrainingData.grt") ) ){
        infoText = "Training data saved to file";
    } else infoText = "WARNING: Failed to save training data to file";
}

//--------------------------------------------------------------
void ofApp::load() {
    if( trainingData.load( ofToDataPath("TrainingData.grt") ) ){
        infoText = "Training data loaded from file";
        trainClassifier();
    } else infoText = "WARNING: Failed to load training data from file";
}

//--------------------------------------------------------------
void ofApp::clear() {
    trainingData.clear();
    infoText = "Training data cleared";
    tPredict = false;
}

//--------------------------------------------------------------
void ofApp::sendOSC() {
    ofxOscMessage m;
    m.setAddress(oscAddress);
    m.addIntArg(pipeline.getPredictedClassLabel());
    sender.sendMessage(m, false);
}

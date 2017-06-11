#include "ofApp.h"

const ofColor backgroundPlotColor = ofColor(50,50,50,255);

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);

    smallFont.load(ofToDataPath("verdana.ttf"), 10, true, true);
    smallFont.setLineHeight(12.0f);
    hugeFont.load(ofToDataPath("verdana.ttf"), 36, true, true);
    hugeFont.setLineHeight(38.0f);
    
    //Initialize the training and info variables
    infoText = "";
    trainingClassLabel = 1;
    predictedClassLabel = 0;
    trainingModeActive = false;
    recordTrainingData = false;
    predictionModeActive = false;
    drawInfo = true;
    
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //RAWINPUTS=136, ORIENTATIONINPUTS=9, GESTUREINPUTS=5
    
    trainingData.setNumDimensions( trainingInputs );
    
    KNN knn; /*Other classifiers: AdaBoost adaboost; DecisionTree dtree; KNN knn; GMM gmm; ANBC naiveBayes; MinDist minDist; RandomForests randomForest; Softmax softmax; SVM svm; */
    pipeline.setClassifier( knn );
    
    
    grabber.setup(1280,720);

    //grabber.setup(960,540);
    
    // Setup tracker
    tracker.setup(ofToDataPath("../../../../data/shape_predictor_68_face_landmarks.dat"));
//    tracker.setup(ofToDataPath("shape_predictor_68_face_landmarks.dat"));

    
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
    bPause.addListener(this, &ofApp::pause);
    
    rawBool.addListener(this, &ofApp::rawToogled);
    gestureBool.addListener(this, &ofApp::gestureToggled);
    orientationBool.addListener(this, &ofApp::orientationToggled);
    
    gui.setup();
    gui.add(sliderClassLabel.setup("Class Label", 1, 1, 9));
    gui.add(tRecord.setup("Record", false));
    gui.add(bTrain.setup("Train"));
    gui.add(bSave.setup("Save"));
    gui.add(bLoad.setup("Load"));
    gui.add(bClear.setup("Clear"));
    
    gui.add(bPause.setup("Pause Prediction Mode"));
    gui.add(rawBool.setup("raw input", true));
    gui.add(gestureBool.setup("gesture input", false));
    gui.add(orientationBool.setup("orientation input",false));
    
    gui.setPosition(10,10);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //FaceTracker2
    grabber.update();
    
    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }
    
    VectorFloat inputVector(trainingInputs);
    
    if (tracker.size()>0) {
        
        //RAW (136 values)
        if (rawBool) {
            //Send all raw points (68 facepoints with x+y = 136 values) (only 1 face)
            auto facePoints = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::ALL_FEATURES);
            
            for (int i = 0; i<facePoints.size(); i++) {
                ofPoint p = facePoints.getVertices()[i].getNormalized(); //only values from 0-1. Experiment with this, and try to send non-normalized as well
                
                inputVector[i] = p.x;
                inputVector[i + facePoints.size()] = p.y; //Not that elegant...
            }
        }
        
        
        //ORIENTATION (9 values)
        if (orientationBool) {
            for (int i = 0; i<=2; i++) {
                for (int j = 0; j<=2; j++) {
                    inputVector[i+RAWINPUTS*rawBool] = tracker.getInstances()[0].getPoseMatrix().getRowAsVec3f(i)[j];
                }
            }
        }
        
        
        //GESTURES (5 values)
        if (gestureBool) {
            inputVector[0+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(RIGHT_EYE_OPENNESS);
            inputVector[1+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(LEFT_EYE_OPENNESS);
            inputVector[2+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(RIGHT_EYEBROW_HEIGHT);
            inputVector[3+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(LEFT_EYEBROW_HEIGHT);
            inputVector[4+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(MOUTH_HEIGHT);
        }
    }
 
        if ( tRecord ) {
            trainingData.addSample( sliderClassLabel, inputVector );
        }
    
        //Update the prediction mode if active
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
void ofApp::draw(){
    
    ofBackground(225, 225, 225);
    
    ofSetColor(255);
    grabber.draw(0, 0);
    
    for (int i = 0; i<tracker.size(); i ++) {
        
        //Draw face
        ofPolyline jaw = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::JAW);
        ofPolyline left_eyebrow = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYEBROW);
        ofPolyline right_eyebrow = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYEBROW);
        ofPolyline left_eye_top = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE_TOP);
        ofPolyline right_eye_top = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYE_TOP);
        ofPolyline left_eye = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE);
        ofPolyline right_eye = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYE);
        ofPolyline outher_mouth = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::OUTER_MOUTH);
        ofPolyline inner_mouth = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::INNER_MOUTH);
        ofPolyline nose_bridge = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BRIDGE);
        ofPolyline nose_base = tracker.getInstances()[i].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BASE);
        
        jaw.draw();
        left_eyebrow.draw();
        right_eyebrow.draw();
        left_eye_top.draw();
        right_eye_top.draw();
        left_eye.draw();
        right_eye.draw();
        outher_mouth.draw();
        inner_mouth.draw();
        nose_bridge.draw();
        nose_base.draw();
    }
    
    int marginX = 5;
    int marginY = 5;
    int graphX = marginX + gui.getWidth() + 10;
    int graphY = marginY;
    
    int graphW = ofGetWidth() - graphX*2;
    int graphH = 150;


    //Draw graph + prediction
    if( pipeline.getTrained() ){
        predictionPlot.draw( graphX, graphY, graphW, graphH ); graphY += graphH * 1.1;
        std::string txt = "Predicted Class: " + ofToString( predictedClassLabel );
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        ofSetColor(0,0,255);
        hugeFont.drawString( txt, ofGetWidth()/2 - bounds.width*0.5, ofGetHeight() - bounds.height*3 );
    }
    
    //Draw info texts
    ofSetColor(0);
    ofDrawBitmapString("Num Samples: " + ofToString( trainingData.getNumSamples() ), 10, gui.getHeight() + 50 );
    ofDrawBitmapString("Total input values: "+ofToString(trainingInputs), 10, gui.getHeight() + 70 );
    ofDrawBitmapString(infoText, 10, gui.getHeight() + 90 );
    gui.draw();
}


//--------------------------------------------------------------
float ofApp:: getGesture (Gesture gesture){
    
    if(tracker.size()<1) {
        return 0;
    }
    int start = 0, end = 0;
    int gestureMultiplier = 10;
    
    
    switch(gesture) {
            // left to right of mouth
        case MOUTH_WIDTH: start = 48; end = 54; break;
            // top to bottom of inner mouth
        case MOUTH_HEIGHT: start = 51; end = 57; gestureMultiplier = 10; break;
            // center of the eye to middle of eyebrow
        case LEFT_EYEBROW_HEIGHT: start = 38; end = 20; gestureMultiplier = 10; break;
            // center of the eye to middle of eyebrow
        case RIGHT_EYEBROW_HEIGHT: start = 43; end = 23; gestureMultiplier = 10; break;
            // upper inner eye to lower outer eye
        case LEFT_EYE_OPENNESS: start = 38; end = 40; gestureMultiplier = 25; break;
            // upper inner eye to lower outer eye
        case RIGHT_EYE_OPENNESS: start = 43; end = 47; gestureMultiplier = 25; break;
            // nose center to chin center
        case JAW_OPENNESS: start = 33; end = 8; break;
            // left side of nose to right side of nose
        case NOSTRIL_FLARE: start = 31; end = 35; break;
    }
    
    
    //Normalized
    return (gestureMultiplier*abs(abs(tracker.getInstances()[0].getLandmarks().getImagePoint(start).getNormalized().y - tracker.getInstances()[0].getLandmarks().getImagePoint(end).getNormalized().y)));
    
    
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
        case 'p':
            pause();
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
    if( trainingData.save( ofToDataPath("TrainingData_FaceClassifier.grt") ) ){
        infoText = "Training data saved to file";
    }else infoText = "WARNING: Failed to save training data to file";
}

//--------------------------------------------------------------
void ofApp::load() {
    if( trainingData.load( ofToDataPath("TrainingData_FaceClassifier.grt") ) ){
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
void ofApp::pause() {
    infoText = "Model paused. Add more samples or retrain.";
    predictionModeActive = false;
}


//--------------------------------------------------------------
void ofApp::rawToogled(bool & state) {
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool;
    trainingData.setNumDimensions( trainingInputs );
}

//--------------------------------------------------------------
void ofApp::gestureToggled(bool & state) {
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool;
    trainingData.setNumDimensions( trainingInputs );
}

//--------------------------------------------------------------
void ofApp::orientationToggled(bool & state) {
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool;
    trainingData.setNumDimensions( trainingInputs );
}


//--------------------------------------------------------------
void ofApp::sendOSC() {
    ofxOscMessage m;
    m.setAddress(oscAddress);
    m.addIntArg(pipeline.getPredictedClassLabel());
    sender.sendMessage(m, false);
}




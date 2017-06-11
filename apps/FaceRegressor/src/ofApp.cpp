/*
 Boiled down example showing how to train a model and do real time regression with ofxGrt and ofxFaceTracker2.
 Inputs are your selected facial features (gestures, orientation and/or raw points). Outputs are width and height of a rectangle.
 
 Instructions:
 1) Set the height and width of the rectangle using the sliders
 2) Press record and use the countdown time to prepare a distinct facial expression. Once recording, your training examples will contain containing your selected facial features (gestures, orientation and/or raw points) and the current width and height of the rectangle.
 3) Repeat step 1-2 with different width and height values for the rectangle and different expressions
 4) Press train to train the model
 5) Move your face and see the changes in rectangle width and height

 */

#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    
    largeFont.load(ofToDataPath("verdana.ttf"), 12, true, true);
    largeFont.setLineHeight(14.0f);
    smallFont.load(ofToDataPath("verdana.ttf"), 10, true, true);
    smallFont.setLineHeight(12.0f);
    hugeFont.load(ofToDataPath("verdana.ttf"), 36, true, true);
    hugeFont.setLineHeight(38.0f);
    
    //Initialize the training and info variables
    infoText = "";
    trainingModeActive = false;
    recordTrainingData = false;
    predictionModeActive = false;
    drawInfo = true;
    
    //Select the inputs: Gestures, orientations or raw inputs
    rawBool = false;
    gestureBool = true;
    orientationBool = false;
    
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool;
    trainingData.setInputAndTargetDimensions( trainingInputs, 2 );
    
    setRegressifier( LINEAR_REGRESSION );
    
    grabber.setup(1280,720);
    tracker.setup(ofToDataPath("../../../../data/shape_predictor_68_face_landmarks.dat"));
//    tracker.setup(ofToDataPath("shape_predictor_68_face_landmarks.dat"));


    //GUI
    bRecord.addListener(this, &ofApp::record);
    bTrain.addListener(this, &ofApp::trainClassifier);
    bSave.addListener(this, &ofApp::save);
    bLoad.addListener(this, &ofApp::load);
    bClear.addListener(this, &ofApp::clear);
    bPause.addListener(this, &ofApp::pause);
    
    rawBool.addListener(this, &ofApp::rawToogled);
    gestureBool.addListener(this, &ofApp::gestureToggled);
    orientationBool.addListener(this, &ofApp::orientationToggled);
    
    gui.setup();
    gui.add(rectWidth.setup("rect width", 100, 1, 500));
    gui.add(rectHeight.setup("rect height", 100, 1, 500));
    gui.add(bRecord.setup("Record"));
    gui.add(bTrain.setup("Train"));
    gui.add(bSave.setup("Save"));
    gui.add(bLoad.setup("Load"));
    gui.add(bClear.setup("Clear"));
    gui.add(bPause.setup("Pause Prediction Mode"));
    gui.add(rawBool.setup("raw input", true));
    gui.add(gestureBool.setup("gesture input", false));
    gui.add(orientationBool.setup("orientation input",false));
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    grabber.update();
    
    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }
    VectorFloat trainingSample(trainingInputs);
    VectorFloat inputVector(trainingInputs);
    
    if ( tracker.size()>0) {
        //RAW (136 values)
        if (rawBool) {
            //Send all raw points (68 facepoints with x+y = 136 values) (only 1 face)
            auto facePoints = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::ALL_FEATURES);
            
            for (int i = 0; i<facePoints.size(); i++) {
                ofPoint p = facePoints.getVertices()[i].getNormalized();
                trainingSample[i] = p.x;
                trainingSample[i + facePoints.size()] = p.y; //Not that elegant...
            }
        }
        
        
        //ORIENTATION (9 values)
        if (orientationBool) {
            for (int i = 0; i<=2; i++) {
                for (int j = 0; j<=2; j++) {
                    trainingSample[i+RAWINPUTS*rawBool] = tracker.getInstances()[0].getPoseMatrix().getRowAsVec3f(i)[j];
                }
            }
        }
        
        //GESTURES (5 values)
        if (gestureBool) {
            trainingSample[0+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(RIGHT_EYE_OPENNESS);
            trainingSample[1+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(LEFT_EYE_OPENNESS);
            trainingSample[2+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(RIGHT_EYEBROW_HEIGHT);
            trainingSample[3+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(LEFT_EYEBROW_HEIGHT);
            trainingSample[4+RAWINPUTS*rawBool+ORIENTATIONINPUTS*orientationBool] = getGesture(MOUTH_HEIGHT);
        }
        
        
        //Update the training mode if needed
        if( trainingModeActive){
            
            //Check to see if the countdown timer has elapsed, if so then start the recording
            if( !recordTrainingData ){
                if( trainingTimer.timerReached() ){
                    recordTrainingData = true;
                    trainingTimer.start( RECORDING_TIME );
                }
            }else{
                //We should be recording the training data - check to see if we should stop the recording
                if( trainingTimer.timerReached() ){
                    trainingModeActive = false;
                    recordTrainingData = false;
                }
            }
            
            if( recordTrainingData ){
                VectorFloat targetVector(2);
                targetVector[0] = rectWidth;
                targetVector[1] = rectHeight;
                
                if( !trainingData.addSample(trainingSample,targetVector) ){
                    infoText = "WARNING: Failed to add training sample to training data!";
                }
            }
        }
        
        inputVector = trainingSample;
        
        //Update the prediction mode if active
        if( predictionModeActive ){
            if( pipeline.predict( inputVector ) ){
                rectWidth = ofClamp(pipeline.getRegressionData()[0],10, ofGetWidth());
                rectHeight = ofClamp(pipeline.getRegressionData()[1],10, ofGetHeight());
            }else{
                infoText = "ERROR: Failed to run prediction!";
            }
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
    
    
    if( trainingModeActive ){
        if( !recordTrainingData ){
            ofSetColor(255, 204, 0);
            string txt = "PREP";
            ofRectangle bounds = hugeFont.getStringBoundingBox(txt,0,0);
            hugeFont.drawString(txt,ofGetWidth()-25-bounds.width,ofGetHeight()-25-bounds.height);
        }else{
            ofSetColor(255,0,0);
            string txt = "REC";
            ofRectangle bounds = hugeFont.getStringBoundingBox(txt,0,0);
            hugeFont.drawString(txt,ofGetWidth()-25-bounds.width,ofGetHeight()-25-bounds.height);
        }
    }
    
    
    ofSetColor(255, 0, 0, 100);
    ofRectangle myRect(0, 0, 100, 100);
    myRect.setFromCenter(ofGetWidth()/2, ofGetHeight()/2, rectWidth, rectHeight);
    ofDrawRectangle(myRect);
    
    ofSetColor(0);
    ofDrawBitmapString("Num Samples: " + ofToString( trainingData.getNumSamples() ), 10, gui.getHeight() + 50 );
    ofDrawBitmapString("Total input values: "+ofToString(trainingInputs), 10, gui.getHeight() + 70 );
    ofDrawBitmapString(infoText, 10, gui.getHeight() + 90 );
    gui.draw();
}


//--------------------------------------------------------------
void ofApp::record() {
    predictionModeActive = false;
    trainingModeActive = true;
    recordTrainingData = false;
    trainingTimer.start( PRE_RECORDING_COUNTDOWN_TIME );
}

//--------------------------------------------------------------
void ofApp::trainClassifier() {
    if( pipeline.train( trainingData ) ){
        infoText = "Pipeline Trained";
        predictionModeActive = true;
    }else infoText = "WARNING: Failed to train pipeline";
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::save() {
    if( trainingData.save( ofToDataPath("TrainingData_faceRegressor.grt") ) ){
        infoText = "Training data saved to file";
    }else infoText = "WARNING: Failed to save training data to file";
}

//--------------------------------------------------------------
void ofApp::load() {
    if( trainingData.load( ofToDataPath("TrainingData_faceRegressor.grt") ) ){
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
    trainingData.setInputAndTargetDimensions( trainingInputs, 2 );
}

//--------------------------------------------------------------
void ofApp::gestureToggled(bool & state) {
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool;
    trainingData.setInputAndTargetDimensions( trainingInputs, 2 );
}

//--------------------------------------------------------------
void ofApp::orientationToggled(bool & state) {
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool;
    trainingData.setInputAndTargetDimensions( trainingInputs, 2 );
}


//--------------------------------------------------------------
bool ofApp::setRegressifier( const int type ){
    
    LinearRegression linearRegression;
    LogisticRegression logisticRegression;
    MLP mlp;
    
    this->regressifierType = type;
    
    pipeline.clear();
    
    switch( regressifierType ){
        case LINEAR_REGRESSION:
            pipeline << MultidimensionalRegression(linearRegression,true);
            break;
        case LOGISTIC_REGRESSION:
            pipeline << MultidimensionalRegression(logisticRegression,true);
            break;
        case NEURAL_NET:
        {
            unsigned int numInputNeurons = trainingData.getNumInputDimensions();
            unsigned int numHiddenNeurons = 10;
            unsigned int numOutputNeurons = 1; //1 as we are using multidimensional regression
            
            //Initialize the MLP
            mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::SIGMOID, Neuron::SIGMOID );
            
            //Set the training settings
            mlp.setMaxNumEpochs( 1000 ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
            mlp.setMinChange( 1.0e-10 ); //This sets the minimum change allowed in training error between any two epochs
            mlp.setLearningRate( 0.001 ); //This sets the rate at which the learning algorithm updates the weights of the neural network
            mlp.setNumRandomTrainingIterations( 5 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
            mlp.setUseValidationSet( true ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
            mlp.setValidationSetSize( 20 ); //Use 20% of the training data for validation during the training phase
            mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
            
            //The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e. [0.0 1.0])
            mlp.enableScaling( true );
            
            pipeline << MultidimensionalRegression(mlp,true);
        }
            break;
        default:
            return false;
            break;
    }
    return true;
}


//--------------------------------------------------------------
float ofApp:: getGesture (Gesture gesture){
    
    //Current issue: How to make it scale accordingly?
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
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

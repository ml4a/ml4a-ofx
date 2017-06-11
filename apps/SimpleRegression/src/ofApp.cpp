/*
 Boiled down example showing how to train a model and do real time regression with ofxGrt.
 The inputs are the x and y position of the mouse. The outputs are width and height of a rectangle.
 
 Instructions:
 1) Set the height and width of the rectangle using the sliders
 2) Press record and use the countdown time to position your mouse. Once recording, your training examples will contain the x and y position of your mouse and the current width and height of the rectangle.
 3) Repeat step 1-2 with different width and height values for the rectangle and different mouse positions
 4) Press train to train the model
 5) Move your mouse around the canvas and see the changes in the width and height of the rectangle
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
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
    
    //The input to the training data will be mouseX and mouseY
    trainingData.setInputAndTargetDimensions( 2, 2 );
    
    //set the default classifier
    setRegressifier( LINEAR_REGRESSION );
    
    ofSetVerticalSync(true);
    
    //GUI
    bRecord.addListener(this, &ofApp::record);
    bTrain.addListener(this, &ofApp::trainClassifier);
    bSave.addListener(this, &ofApp::save);
    bLoad.addListener(this, &ofApp::load);
    bClear.addListener(this, &ofApp::clear);
    trainingLabel.addListener(this, &ofApp::setTrainingLabel);
    bPause.addListener(this, &ofApp::pause);
    
    gui.setup();
    gui.add(rectWidth.setup("rect width", 100, 1, 500));
    gui.add(rectHeight.setup("rect height", 100, 1, 500));
    gui.add(trainingLabel.set("Training Label", 0, 0, regressifierNames.size()-1));
    gui.add(bRecord.setup("Record"));
    gui.add(bTrain.setup("Train"));
    gui.add(bSave.setup("Save"));
    gui.add(bLoad.setup("Load"));
    gui.add(bClear.setup("Clear"));
    gui.add(bPause.setup("Pause Prediction Mode"));
    
    //gui.add(tPause.setup("Pause Prediction Mode", false));
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    //Update the training mode if needed
    if( trainingModeActive ){
        
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
            //Add the current sample to the training data
            VectorFloat trainingSample(2);
            trainingSample[0] = ofGetMouseX();
            trainingSample[1] = ofGetMouseY();
            
            VectorFloat targetVector(2);
            targetVector[0] = rectWidth;
            targetVector[1] = rectHeight;
            
            if( !trainingData.addSample(trainingSample,targetVector) ){
                infoText = "WARNING: Failed to add training sample to training data!";
            }
        }
    }
    
    //if (tPause) predictionModeActive = false;
    
    //Update the prediction mode if active
    if( predictionModeActive ){
        VectorFloat inputVector(2);
        inputVector[0] = ofGetMouseX();
        inputVector[1] = ofGetMouseY();
        
        if( pipeline.predict( inputVector ) ){
            rectWidth = pipeline.getRegressionData()[0];
            rectHeight = pipeline.getRegressionData()[1];
        }else{
            infoText = "ERROR: Failed to run prediction!";
        }
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(225, 225, 225);
    
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(255);
    ofDrawRectangle(ofGetWidth()/2, ofGetHeight()/2, rectWidth, rectHeight);
    
    if( trainingModeActive ){
        char strBuffer[1024];
        if( !recordTrainingData ){
            ofSetColor(255,150,0);
            sprintf(strBuffer, "Training Mode Active \nGet Ready! Timer: %0.1f",trainingTimer.getSeconds());
        }else{
            ofSetColor(255,0,0);
            sprintf(strBuffer, "Training Mode Active \nRecording! Timer: %0.1f",trainingTimer.getSeconds());
        }
        std::string txt = strBuffer;
        ofRectangle bounds = hugeFont.getStringBoundingBox(txt,0,0);
        hugeFont.drawString(txt,ofGetWidth()/4,ofGetHeight()-25-bounds.height);
    }
    
    ofSetColor(0);
    ofDrawBitmapString("Num Samples: " + ofToString( trainingData.getNumSamples() ), 10, gui.getHeight() + 50 );
    ofDrawBitmapString(infoText, 10, gui.getHeight() + 70 );
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    infoText = "";
    bool buildTexture = false;
    
    switch ( key) {
        case 'r':
            predictionModeActive = false;
            trainingModeActive = true;
            recordTrainingData = false;
            trainingTimer.start( PRE_RECORDING_COUNTDOWN_TIME );
            break;
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";
                predictionModeActive = true;
            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.save( ofToDataPath("TrainingData_simpleRegression.grt") ) ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.load( ofToDataPath("TrainingData_simpleRegression.grt") ) ){
                infoText = "Training data loaded from file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            predictionModeActive = false;
            break;
            
        case OF_KEY_TAB:
            setRegressifier( ++this->regressifierType % NUM_REGRESSIFIERS );
            break;
            
        default:
            break;
    }
    
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
    if( trainingData.save( ofToDataPath("TrainingData_simpleRegression.grt") ) ){
        infoText = "Training data saved to file";
    }else infoText = "WARNING: Failed to save training data to file";
    
    
}

//--------------------------------------------------------------
void ofApp::load() {
    if( trainingData.load( ofToDataPath("TrainingData_simpleRegression.grt") ) ){
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
void ofApp::setTrainingLabel(int & label_) {
    trainingLabel.setName(regressifierNames[label_]);
    setRegressifier(label_);
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

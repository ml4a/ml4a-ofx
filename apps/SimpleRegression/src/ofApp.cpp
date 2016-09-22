/*
 Boiled down example showing how to train a model and do real time regression with ofxGrt.
 The inputs are the x and y position of the mouse. The outputs are width and height of a rectangle.
 
 Instructions:
 1) Set the height and width of the rectangle using [+], [-], [1] and [2] keys
 2) Press [r] to record some training examples containing the x and y position of your mouse and the width and height of the rectangle.
 3) Repeat step 1-2 with different width and height values for the rectangle and different mouse positions
 4) Press [t] to train the model
 5) Move your mouse around the canvas and see the changes in the width and height of the rectangle
*/

#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
    largeFont.load("verdana.ttf", 12, true, true);
    largeFont.setLineHeight(14.0f);
    smallFont.load("verdana.ttf", 10, true, true);
    smallFont.setLineHeight(12.0f);
    hugeFont.load("verdana.ttf", 36, true, true);
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
    
    rectW = 100;
    rectH = 100;
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
                targetVector[0] = rectW;
                targetVector[1] = rectH;
                
                if( !trainingData.addSample(trainingSample,targetVector) ){
                    infoText = "WARNING: Failed to add training sample to training data!";
                }
            }
        }
        
        //Update the prediction mode if active
        if( predictionModeActive ){
            VectorFloat inputVector(2);
            inputVector[0] = ofGetMouseX();
            inputVector[1] = ofGetMouseY();

            if( pipeline.predict( inputVector ) ){
                rectW = pipeline.getRegressionData()[0];
                rectH = pipeline.getRegressionData()[1];
            }else{
                infoText = "ERROR: Failed to run prediction!";
            }
        }
    }


//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofBackground(225, 225, 225);
    

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

    ofSetColor(255);
    ofDrawRectangle(ofGetWidth()/2, ofGetHeight()/2, rectW, rectH);
    
    
    //Draw the info text
    if( drawInfo ){
        float textX = 10;
        float textY = 25;
        float textSpacer = smallFont.getLineHeight() * 1.5;
        
        ofFill();
        ofSetColor(100,100,100);
        ofDrawRectangle( 5, 5, 250, 360 );
        ofSetColor( 255, 255, 255 );

        largeFont.drawString( "GRT Regressifier Example", textX, textY ); textY += textSpacer*2;
        smallFont.drawString( "[i]: Toogle Info", textX, textY ); textY += textSpacer;
        
        smallFont.drawString( "[-]: rectH-=10", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[+]: rectH+=10", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[1]: rectW-=10", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[2]: rectW+=10", textX, textY ); textY += textSpacer;
        
        smallFont.drawString( "[r]: Record Sample", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[t]: Train Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[c]: Clear Training Data", textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        
        smallFont.drawString( "Classifier: " + regressifierTypeToString( regressifierType ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[tab]: Select Regressifier", textX, textY ); textY += textSpacer;
        
        smallFont.drawString( "Recording: " + ofToString( recordTrainingData ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Num Samples: " + ofToString( trainingData.getNumSamples() ), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( infoText, textX, textY ); textY += textSpacer;
        
        smallFont.drawString( "rectW:" + ofToString(rectW), textX, textY ); textY += textSpacer;
        smallFont.drawString( "rectH:" + ofToString(rectH), textX, textY ); textY += textSpacer;
        
        
        ofSetColor(0);
        smallFont.drawString( " INSTRUCTIONS: \n \n Train a model and do real time regression with ofxGrt. Inputs: x and y position of the mouse. Outputs: width and height of a rectangle. \n \n 1) Set the height and width of the rectangle using [+], [-], [1] and [2] keys \n \n 2) Press [r] to record some training examples containing the x and y position of your mouse and the width and height of the rectangle \n \n 3) Repeat step 1-2 with different width and height values for the rectangle and different mouse positions \n \n 4) Press [t] to train the model \n \n 5)  Move your mouse around the canvas and see the changes in the width and height of the rectangle",  textX, 580 );
        
    }
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
            if( trainingData.save( ofToDataPath("TrainingData.grt") ) ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.load( ofToDataPath("TrainingData.grt") ) ){
                infoText = "Training data loaded from file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
        case 'i':
            drawInfo = !drawInfo;
            break;
        case '-':
            rectH-=10;
            std::cout << "rectH: " << rectH << std::endl;
            break;
        case '+':
            rectH+=10;
            std::cout << "rectH: " << rectH << std::endl;
            break;
        case '1':
            rectW-=10;
            std::cout << "rectW: " << rectW << std::endl;
            break;
        case '2':
            rectW+=10;
            std::cout << "rectW: " << rectW << std::endl;
            break;
        
        case OF_KEY_TAB:
            setRegressifier( ++this->regressifierType % NUM_REGRESSIFIERS );
            break;
            
        default:
            break;
    }
    
}

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
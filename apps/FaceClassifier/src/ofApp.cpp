/*
Description: gesture classification using ofxFaceTracker2 and ofxGrt. 
Supports a long range of classification algoritms, and allows users to switch between iputs and combine them on the fly. 
Also allows users to save and load models.
 
To do:
- Make the UI a bit more friendly
- Flexible graph area
- Enable users to delete the latest recording
 
 */

#include "ofApp.h"

const ofColor backgroundPlotColor = ofColor(50,50,50,255);

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
    trainingClassLabel = 1;
    predictedClassLabel = 0;
    trainingModeActive = false;
    recordTrainingData = false;
    predictionModeActive = false;
    drawInfo = true;

    //Select the inputs: Gestures, orientations or raw inputs
    rawBool = false;
    gestureBool = true;
    orientationBool = true;
    
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //RAWINPUTS=136, ORIENTATIONINPUTS=9, GESTUREINPUTS=5
    
    trainingData.setNumDimensions( trainingInputs );
    
    //set the default classifier
    setClassifier( MINDIST );
    
    //What to draw
    drawNumbers = false;
    drawFace = true;
    drawPose = false;
    drawVideo = true;
    
    // Setup grabber
    grabber.setup(1280,720);
    //grabber.setup(960,540);
    
    // Setup tracker
    tracker.setup("../../../../data/shape_predictor_68_face_landmarks.dat");

}

//--------------------------------------------------------------
void ofApp::update(){
    
    //FaceTracker2
    grabber.update();
    
    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }
    
    
    VectorFloat trainingSample(trainingInputs);
    VectorFloat inputVector(trainingInputs);
    
    if (tracker.size()>0) {
        
        //RAW (136 values)
        if (rawBool) {
            
            //Send all raw points (68 facepoints with x+y = 136 values) (only 1 face)
            auto facePoints = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::ALL_FEATURES);
            
            for (int i = 0; i<facePoints.size(); i++) {
                ofPoint p = facePoints.getVertices()[i].getNormalized(); //only values from 0-1. Experiment with this, and try to send non-normalized as well
                
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
    }
    
    inputVector = trainingSample;
 

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
                
                if( !trainingData.addSample(trainingClassLabel, trainingSample) ){
                    infoText = "WARNING: Failed to add training sample to training data!";
                }
            }
        }
        
        //Update the prediction mode if active
        if( predictionModeActive ){


            if( pipeline.predict( inputVector ) ){
                predictedClassLabel = pipeline.getPredictedClassLabel();
                predictionPlot.update( pipeline.getClassLikelihoods() );
                
            }else{
                infoText = "ERROR: Failed to run prediction!";
            }
        }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(225, 225, 225);
    
    //FaceTracker draw
    ofSetColor(255);
    // Draw camera image
    if (drawVideo) grabber.draw(0, 0); //Default
    
    // Draw estimated 3d pose
    if (drawPose) tracker.drawDebugPose();
    
    //Draw all the individual points / numbers for all tracked faces
    for (int i = 0; i<tracker.size(); i ++) {
        
        if(drawFace) {
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
        
        for (int j = 0; j< tracker.getInstances()[i].getLandmarks().getImagePoints().size(); j++) {
            if (drawNumbers) ofDrawBitmapString(j, tracker.getInstances()[i].getLandmarks().getImagePoint(j));
        }
    }
    
    int marginX = 5;
    int marginY = 5;
    int graphX = marginX;
    int graphY = marginY;
    int graphW = ofGetWidth() - graphX*2;
    int graphH = 150;

    //Draw the info text
    if( drawInfo ){
        float infoX = marginX;
        float infoW = 250;
        float textX = 10;
        float textY = marginY;
        float textSpacer = smallFont.getLineHeight() * 1.5;

        ofFill();
        ofSetColor(100,100,100);
        ofDrawRectangle( infoX, 5, infoW, 565 );
        ofSetColor( 255, 255, 255 );

        smallFont.drawString( "FACETRACKER2 CLASSIFIER EXAMPLE", textX, textY +20); textY += textSpacer*2;

        smallFont.drawString( "[i]: Toogle Info", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[r]: Toggle Recording", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[l]: Load Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[s]: Save Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[t]: Train Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[1,2,3...]: Set Class Label", textX, textY ); textY += textSpacer;
        smallFont.drawString( "Classifier: " + classifierTypeToString( classifierType ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[n] null rejection: " + ofToString(nullRejection), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( "Class Label: " + ofToString( trainingClassLabel ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Recording: " + ofToString( recordTrainingData ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Num Samples: " + ofToString( trainingData.getNumSamples() ), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( "[a] Input all raw points: "+ofToString(rawBool), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[g] Input gestures: "+ofToString(gestureBool), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[o] Input orientation: "+ofToString(orientationBool), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Total input values: "+ofToString(trainingInputs), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( "[n] draw numbers: "+ofToString(drawNumbers), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[f] draw face: "+ofToString(drawFace), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[p] draw pose: "+ofToString(drawPose), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[v] draw video: "+ofToString(drawVideo), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( infoText, textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( "Framerate : "+ofToString(ofGetFrameRate()), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Tracker thread framerate : "+ofToString(tracker.getThreadFps()), textX, textY ); textY += textSpacer;
        smallFont.drawString("Faces detected : "+ofToString(tracker.size()), textX, textY ); textY += textSpacer;

        //Update the graph position
        graphX = infoX + infoW + 15;
        graphW = ofGetWidth() - graphX - 15;
    }


    if( trainingModeActive ){
        char strBuffer[1024];
        if( !recordTrainingData ){
            ofSetColor(255,150,0);
            sprintf(strBuffer, "Training Mode Active - Get Ready! Timer: %0.1f",trainingTimer.getSeconds());
        }else{
            ofSetColor(255,0,0);
            sprintf(strBuffer, "Training Mode Active - Recording! Timer: %0.1f",trainingTimer.getSeconds());
        }
        std::string txt = strBuffer;
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        hugeFont.drawString(strBuffer, ofGetWidth()/2 - bounds.width*0.5, ofGetHeight() - bounds.height*3 );
    }

    //If the model has been trained, then draw this
    if( pipeline.getTrained() ){
        predictionPlot.draw( graphX, graphY, graphW, graphH ); graphY += graphH * 1.1;

        std::string txt = "Predicted Class: " + ofToString( predictedClassLabel );
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        ofSetColor(0,0,255);
        hugeFont.drawString( txt, ofGetWidth()/2 - bounds.width*0.5, ofGetHeight() - bounds.height*3 );
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
        case '1':
            trainingClassLabel = 1;
            break;
        case '2':
            trainingClassLabel = 2;
            break;
        case '3':
            trainingClassLabel = 3;
            break;
        case '4':
            trainingClassLabel = 4;
            break;
        case '5':
            trainingClassLabel = 5;
            break;
        case '6':
            trainingClassLabel = 6;
            break;
        case '7':
            trainingClassLabel = 7;
            break;
        case '8':
            trainingClassLabel = 8;
            break;
        case '9':
            trainingClassLabel = 9;
            break;
            
        case 'g': //Toogle gesture inputs and update length of trainingInputs
            gestureBool =! gestureBool;
            trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //GESTUREINPUTS=5, ORIENTATIONINPUTS=9, RAWINPUTS=136
            trainingData.setNumDimensions(trainingInputs);
            break;
            
        case 'o': //Toogle orientation inputs and update length of trainingInputs
            orientationBool =! orientationBool;
            trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //GESTUREINPUTS=5, ORIENTATIONINPUTS=9, RAWINPUTS=136
           trainingData.setNumDimensions(trainingInputs);
            break;
            
        case 'a': //Toogle raw inputs and update length of trainingInputs
            rawBool =! rawBool;
            trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //GESTUREINPUTS=5, ORIENTATIONINPUTS=9, RAWINPUTS=136
             trainingData.setNumDimensions(trainingInputs);
            break;
            
        case 'n':
            nullRejection =! nullRejection;
            setClassifier(this->classifierType % NUM_CLASSIFIERS ); //Is this solid?
            break;
            
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";
                std::cout << "getNumClasses: " << pipeline.getNumClasses() << std::endl;
                predictionPlot.setup( 500, pipeline.getNumClasses(), "prediction likelihoods" );
                predictionPlot.setDrawGrid( true );
                predictionPlot.setDrawInfoText( true );
                predictionPlot.setFont( smallFont );
                predictionPlot.setBackgroundColor( backgroundPlotColor );
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
        
        case 'u':
            drawNumbers = !drawNumbers;
        break;
        
        case 'f':
            drawFace = !drawFace;
        break;
        
        case 'p':
            drawPose = !drawPose;
        break;
            
        case 'v':
            drawVideo = !drawVideo;
        break;
            
            
        case OF_KEY_TAB:
            setClassifier( ++this->classifierType % NUM_CLASSIFIERS );
            break;
            
        default:
            break;
    }

}


bool ofApp::setClassifier( const int type ){
    
    AdaBoost adaboost;
    DecisionTree dtree;
    KNN knn;
    GMM gmm;
    ANBC naiveBayes;
    MinDist minDist;
    RandomForests randomForest;
    Softmax softmax;
    SVM svm;
    
    this->classifierType = type;
    
    switch( classifierType ){
        case ADABOOST:
            adaboost.enableNullRejection( nullRejection ); // The GRT AdaBoost algorithm does not currently support null rejection, although this will be added at some point in the near future.
            adaboost.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( adaboost );
            break;
        case DECISION_TREE:
            dtree.enableNullRejection( nullRejection );
            dtree.setNullRejectionCoeff( 3 );
            dtree.setMaxDepth( 10 );
            dtree.setMinNumSamplesPerNode( 3 );
            dtree.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( dtree );
            break;
        case KKN:
            knn.enableNullRejection( nullRejection );
            knn.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( knn );
            break;
        case GAUSSIAN_MIXTURE_MODEL:
            gmm.enableNullRejection( nullRejection );
            gmm.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( gmm );
            break;
        case NAIVE_BAYES:
            naiveBayes.enableNullRejection( nullRejection );
            naiveBayes.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( naiveBayes );
            break;
        case MINDIST:
            minDist.enableNullRejection( nullRejection );
            minDist.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( minDist );
            break;
        case RANDOM_FOREST_10:
            randomForest.enableNullRejection( nullRejection );
            randomForest.setNullRejectionCoeff( 3 );
            randomForest.setForestSize( 10 );
            randomForest.setNumRandomSplits( 2 );
            randomForest.setMaxDepth( 10 );
            randomForest.setMinNumSamplesPerNode( 3 );
            randomForest.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( randomForest );
            break;
        case RANDOM_FOREST_100:
            randomForest.enableNullRejection( nullRejection );
            randomForest.setNullRejectionCoeff( 3 );
            randomForest.setForestSize( 100 );
            randomForest.setNumRandomSplits( 2 );
            randomForest.setMaxDepth( 10 );
            randomForest.setMinNumSamplesPerNode( 3 );
            randomForest.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( randomForest );
            break;
        case RANDOM_FOREST_200:
            randomForest.enableNullRejection( nullRejection );
            randomForest.setNullRejectionCoeff( 3 );
            randomForest.setForestSize( 200 );
            randomForest.setNumRandomSplits( 2 );
            randomForest.setMaxDepth( 10 );
            randomForest.setMinNumSamplesPerNode( 3 );
            randomForest.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( randomForest );
            break;
        case SOFTMAX:
            softmax.enableNullRejection( false ); //Does not support null rejection
            softmax.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( softmax );
            break;
        case SVM_LINEAR:
            svm.enableNullRejection( nullRejection );
            svm.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( SVM(SVM::LINEAR_KERNEL) );
            break;
        case SVM_RBF:
            svm.enableNullRejection( nullRejection );
            svm.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( SVM(SVM::RBF_KERNEL) );
            break;
        default:
            return false;
            break;
    }
    
    return true;
}


//--------------------------------------------------------------
float ofApp:: getGesture (Gesture gesture){
    
    //Current issues: How to make it scale accordingly?
    
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

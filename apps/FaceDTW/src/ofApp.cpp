#include "ofApp.h"
#define FRAME_RATE 60

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate( FRAME_RATE );

    //Load the resources
    font.load(ofToDataPath("verdana.ttf"), 12, true, true);
    font.setLineHeight(14.0f);
    //shader.load("shaders/noise.vert", "shaders/noise.frag");
    
    //Initialize the training and info variables
    infoText = "";
    trainingClassLabel = 0;
    record = false;
    
    
    //Select the inputs: Gestures, orientations or raw inputs
    rawBool = false;
    gestureBool = false;
    orientationBool = true;
    
    trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //RAWINPUTS=136, ORIENTATIONINPUTS=9, GESTUREINPUTS=5
    
    //The input to the training data will be the sum of two gesture values for eye openess, so we set the number of dimensions to 1
    trainingData.setNumDimensions( trainingInputs );
    
    //Turn on null rejection, this lets the classifier output the predicted class label of 0 when the likelihood of a gesture is low
    dtw.enableNullRejection( true );
    
    //Set the null rejection coefficient. This controls the thresholds for the automatic null rejection
    //You can increase this value if you find that your real-time gestures are not being recognized
    //If you are getting too many false positives then you should decrease this value
    
    
    dtw.setNullRejectionCoeff( rejectCoeff );
    
    
    //Attempt to do post-processing -- see ClassLabelTimeOutFilterExample from ESP
    
    //Post-processing - Set the time between each prediction 
    pipeline.addPostProcessingModule( ClassLabelTimeoutFilter(timeOutFilter,ClassLabelTimeoutFilter::ALL_CLASS_LABELS) );
    
    
    //Turn on the automatic data triming, this will remove any sections of none movement from the start and end of the training samples
    dtw.enableTrimTrainingData(true, 0.1, 90);
    
    //Offset the timeseries data by the first sample, this makes your gestures (more) invariant to the location the gesture is performed
    dtw.setOffsetTimeseriesUsingFirstSample(true);

    //Allow the DTW algorithm to search the entire cost matrix
    dtw.setContrainWarpingPath( true );
    
    //Add the classifier to the pipeline (after we do this, we don't need the DTW classifier anymore)
    pipeline.setClassifier( dtw );
    
    
    //What to draw
    drawNumbers = false;
    drawFace = true;
    drawPose = false;
    drawVideo = false;
    
    
    // Setup grabber
    grabber.setup(1280,720);
    //grabber.setup(960,540);
    
    // Setup tracker
    tracker.setup(ofToDataPath("../../../../data/shape_predictor_68_face_landmarks.dat"));
//    tracker.setup(ofToDataPath("shape_predictor_68_face_landmarks.dat"));
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //FaceTracker2
    grabber.update();
    
    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }
    
    
//    //Grab the current mouse x and y position
//    VectorDouble sample(2);
//    sample[0] = mouseX;
//    sample[1] = mouseY;
    
   //Set the samples to RIGHT_EYE_OPENNESS and LEFT_EYE_OPENNESS
    
        VectorDouble trainingSample(trainingInputs);
        //sample[0] = getGesture(RIGHT_EYE_OPENNESS) + getGesture(LEFT_EYE_OPENNESS);

    
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
    

    
    
    //If we are recording training data, then add the current sample to the training data set
    if( record ){
        timeseries.push_back( trainingSample );
    }
    
    //If the pipeline has been trained, then run the prediction
    if( pipeline.getTrained() ){

        //Run the prediction
        pipeline.predict( trainingSample );

        //Update the plots
        predictedClassPlot.update( VectorFloat(1,pipeline.getPredictedClassLabel()) );
        classLikelihoodsPlot.update( pipeline.getClassLikelihoods() );
        
        gestureRecognised(pipeline.getPredictedClassLabel());
        if ( pipeline.getPredictedClassLabel() > 0) {
        lastGesture = pipeline.getPredictedClassLabel();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0, 0, 0);
    
    
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
    
    
    string text;
    const int MARGIN = 20;
    const int graphSpacer = 15;
    int textX = MARGIN;
    int textY = MARGIN;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
    text = "------------------- TrainingInfo -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    if( record ) ofSetColor(255, 0, 0);
    else ofSetColor(255, 255, 255);
    textY += 15;
    text = record ? "RECORDING" : "Not Recording";
    ofDrawBitmapString(text, textX,textY);
    
    ofSetColor(255, 255, 255);
    textY += 15;
    text = "TrainingClassLabel: " + ofToString(trainingClassLabel);
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "NumTrainingSamples: " + ofToString(trainingData.getNumSamples());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    
    textY += 15;
    text = "rejectCoeff: " + ofToString(rejectCoeff);
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "timeOutFilter: " + ofToString(timeOutFilter);
    ofDrawBitmapString(text, textX,textY);

    
    
    //Draw the prediction info
    textY += 30;
    text = "------------------- Prediction Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "SampleRate: " + ofToString(ofGetFrameRate(),2);
    ofDrawBitmapString(text, textX,textY);
    
    
    
    //Draw the control info
    textY += 30;
    text = "------------------- Control Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text =  "[r]: Start/stop recording";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text =  "[0-10]: Set class";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[l]: Load model";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[s]: Save model";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[t]: Train model";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[c]: Clear training data";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[+]: rejectCoeff+=0.01;";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[-]: rejectCoeff-=0.01;";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[w]: timeOutFilter +=50;";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "[q]: timeOutFilter -=50;";
    ofDrawBitmapString(text, textX,textY);
    
//    textY += 15;
//    ofDrawBitmapString( "[a] Input all raw points: "+ofToString(gestureBool), textX, textY );
    textY += 15;
    ofDrawBitmapString( "[g] Input gestures: "+ofToString(gestureBool), textX, textY );
    textY += 15;
    ofDrawBitmapString( "[o] Input orientation: "+ofToString(orientationBool), textX, textY );
    textY += 15;
    ofDrawBitmapString( "Total input values: "+ofToString(trainingInputs), textX, textY );
    
    
    
    //Draw the info text
    textY += 30;
    text = infoText;
    ofSetColor(255, 204, 0);
    ofDrawBitmapString(text, textX,textY);
    ofSetColor(255,255,255);
    
    
    
    //Draw the instructions info
    textY = 590;
    textX = ofGetWidth()/2+75;
    text = "------------------- Instructions -------------------";
    ofDrawBitmapString(text, textX,textY);
    textY += 15;
    ofDrawBitmapString("1. Press [r] to start recording a neutral gesture on class 0.", textX,textY);
    textY += 15;
    ofDrawBitmapString("   Press [r] to stop recording again.", textX,textY);
    textY += 15;
    ofDrawBitmapString("2. Repeat this a number of times.", textX,textY);
    textY += 15;
    ofDrawBitmapString("3. Press [1] to change class and [r] to record a distinct gesture.", textX,textY);
    textY += 15;
    ofDrawBitmapString("   Press [r] to stop recording again.", textX,textY);
    textY += 15;
    ofDrawBitmapString("4. Repeat this a number of times.", textX,textY);
    textY += 15;
    ofDrawBitmapString("5. Press [t] to train the model.", textX,textY);
    
    
    if( pipeline.getTrained() ){
        
        //Draw the data in the DTW input buffer
        DTW *dtw = pipeline.getClassifier< DTW >();
        
        if( dtw != NULL ){
            float x,y,w,h,r,g,b;
            float zoom = 5;

            Vector< VectorFloat > inputData = dtw->getInputDataBuffer();
            for(UINT i=0; i<inputData.getSize(); i++){
                x = inputData[i][0];
                y = inputData[i][1];
                r = ofMap(i,0,inputData.getSize(),0,255);
                g = 0;
                b = 255-r;
                
                ofSetColor(r,g,b);
                ofDrawEllipse(x,y,5,5);
            }

            //Draw the distance matrix for each class
            //drawDistanceMatrix();

            //Draw the predicted class label plot
            w = ofGetWidth() * 0.5;
            h = 100;
            x = MARGIN;
            y = ofGetHeight() - (h + graphSpacer)*2;
            predictedClassPlot.draw( x, y, w, h );

            //Draw the class likelihoods plot
            y += h + graphSpacer;
            classLikelihoodsPlot.draw( x, y, w, h );
        }
    }else{

        //If we get here, then we are in training mode

        //Draw the timeseries data
        if( record ){
            drawTimeseries();
        }

        //Draw any exisiting training samples
        drawTrainingData();
    }
    

    
    if (ellipseSize>10) ellipseSize -= 10;
    
    ofSetColor(0,255,0, ellipseSize);
    
    ofDrawEllipse(ofGetWidth()/2, 200, ellipseSize, ellipseSize);
    
    
    ofDrawBitmapString(lastGesture, ofGetWidth()/2, 100);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    string labelName = "";
    infoText = "";
    
    switch ( key) {
        case 'r':
            record = !record;
            if( !record ){
                trainingData.addSample(trainingClassLabel, timeseries);

                //Update the training data plot
                labelName = string("Class: ") + grt_to_str<unsigned int>( trainingClassLabel );
                trainingDataPlot.push_back( std::make_shared<ofxGrtTimeseriesPlot>() );
                trainingDataPlot.back()->setup( timeseries.getNumRows(), timeseries.getNumCols(), labelName );
                trainingDataPlot.back()->setFont( font );
                trainingDataPlot.back()->setData( timeseries );

                //Clear the timeseries for the next recording
                timeseries.clear();
            }
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
            trainingClassLabel = 4;
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
        case '0':
            trainingClassLabel = 0;
        break;
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";

                //Setup the distance matrix
                //distanceMatrixPlots.resize( pipeline.getNumClasses() );

                //Setup the plots for prediction
                predictedClassPlot.setup( FRAME_RATE * 5, 1, "predicted label" );
                predictedClassPlot.setFont( font );
                predictedClassPlot.setRanges( 0, pipeline.getNumClasses(), false, false, false);
                classLikelihoodsPlot.setup( FRAME_RATE * 5, pipeline.getNumClasses(), "class likelihoods" );
                classLikelihoodsPlot.setFont( font );
                classLikelihoodsPlot.setRanges( 0, 1, false, false, false);


            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.saveDatasetToFile("TrainingData_FaceDTW.grt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.loadDatasetFromFile("TrainingData_FaceDTW.grt") ){
                infoText = "Training data loaded from file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
            
        case'+':
            rejectCoeff+=0.01;
            dtw.setNullRejectionCoeff( rejectCoeff );
            break;

        case'q':
            if (rejectCoeff > 0) rejectCoeff-=0.01;
            dtw.setNullRejectionCoeff( rejectCoeff );
            break;
        
        case'w':
            timeOutFilter -=50;
            pipeline.setPostProcessingModule(ClassLabelTimeoutFilter(timeOutFilter,ClassLabelTimeoutFilter::ALL_CLASS_LABELS) );
            break;
            
        
        case'p':
            timeOutFilter +=50;
            pipeline.setPostProcessingModule(ClassLabelTimeoutFilter(timeOutFilter,ClassLabelTimeoutFilter::ALL_CLASS_LABELS) );
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
            
//        case 'a': //Toogle raw inputs and update length of trainingInputs
//            rawBool =! rawBool;
//            trainingInputs = GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //GESTUREINPUTS=5, ORIENTATIONINPUTS=9, RAWINPUTS=136
//            trainingData.setNumDimensions(trainingInputs);
//            break;
            
            
        default:
            break;
    }

}

void ofApp::drawTimeseries(){
    ofFill();
    for(UINT i=0; i<timeseries.getNumRows(); i++){
        double x = timeseries[i][0];
        double y = timeseries[i][1];
        double r = ofMap(i,0,timeseries.getNumRows(),0,255);
        double g = 0;
        double b = 255-r;
        
        ofSetColor(r,g,b);
        ofDrawEllipse(x,y,5,5);
    }
}

void ofApp::drawTrainingData(){

    if( trainingDataPlot.getSize() == 0 ) return;

    ofSetColor(255,255,255);
    ofFill();
    ofRectangle bounds = font.getStringBoundingBox("Training Examples",0,0);
    float w = 250;
    float h = 50;
    float x = ofGetWidth() - w - 10;
    float y = 10 + bounds.height;
    font.drawString( "Training Examples", x, y );
    y += 15;
    for(UINT i=0; i<trainingDataPlot.getSize(); i++){
        trainingDataPlot[i]->draw( x, y, w, h );
        y += h+5;
    }

}

//void ofApp::drawDistanceMatrix(){
//
//    //Get a pointer to the DTW classifier
//    DTW *dtw = pipeline.getClassifier< DTW >();
//
//    if( dtw == NULL ) return;
//
//    ofSetColor(255,255,255);
//    ofFill();
//
//    ofRectangle bounds = font.getStringBoundingBox("Distance Matrix",0,0);
//    float w = bounds.width;
//    float h = 100;
//    float x = ofGetWidth() - bounds.width - 10;
//    float y = 10 + bounds.height;
//    font.drawString( "Distance Matrix", x, y );
//    
//     //Draw the DTW cost matrix for each class
//    const Vector< MatrixFloat > &distanceMatrix = dtw->getDistanceMatrices();
//
//    if( distanceMatrixPlots.getSize() != distanceMatrix.getSize() ){
//        distanceMatrixPlots.resize( distanceMatrix.getSize() );
//    }
//
//    y += 15;
//    for(UINT i=0; i<distanceMatrix.getSize(); i++){
//        distanceMatrixPlots[i].update( distanceMatrix[i], distanceMatrix[i].getMinValue(), distanceMatrix[i].getMaxValue() );
//        shader.begin();
//        distanceMatrixPlots[i].draw( x, y, w, h );
//        shader.end();
//        y += h + 10;
//    }
//
//}


//--------------------------------------------------------------
float ofApp:: getGesture (Gesture gesture){
    
    //Current issues: How to make it scale accordingly?
    
    if(tracker.size()<1) {
        return 0;
    }
    int start = 0, end = 0;
    int gestureMultiplier = 10; //A bit arbitrary...
    
    
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
void ofApp::gestureRecognised(int g) {
    
    if (g > 0) {
    cout<<g<< "\n";
    ellipseSize = 255;
    }
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

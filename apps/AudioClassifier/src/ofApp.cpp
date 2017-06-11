

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(34, 34, 34);
    ofSetFrameRate(60);
    
    //MAXIM
    // This is stuff you always need when you use Maximilian. Don't change it.
    sampleRate 			= 44100;
    initialBufferSize	= 512;
    lAudioOut			= new float[initialBufferSize];
    rAudioOut			= new float[initialBufferSize];
    lAudioIn			= new float[initialBufferSize];
    rAudioIn			= new float[initialBufferSize];
    
    /* This is a nice safe piece of code */
    memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
    // Now you can put anything you would normally put in maximilian's 'setup' method in here.
    mfft.setup(fftSize, 512, 256); //just a forward FFT
    oct.setup(sampleRate, fftSize/2, nAverages);
    mfccs = (double*) malloc(sizeof(double) * 13);
    
    //512 bins, 42 filters, 13 coeffs, min/max freq 20/20000
    mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    ofSoundStreamSetup(2,2, this, sampleRate, initialBufferSize, 4);// Call this last !
    
    //Fonts
    smallFont.load(ofToDataPath("Arial.ttf"), 10, true, true);
    smallFont.setLineHeight(12.0f);
    hugeFont.load(ofToDataPath("Arial.ttf"), 36, true, true);
    hugeFont.setLineHeight(38.0f);
    
    //Grt
    infoText = "";
    predictedClassLabel = 0;
    trainingModeActive = false;
    predictionModeActive = false;
    trainingInputs = 13; //Number of mfcc's
    trainingData.setNumDimensions( trainingInputs );
    Softmax softmax; //Other classifiers: AdaBoost adaboost; DecisionTree dtree; KNN knn; GMM gmm; ANBC naiveBayes; MinDist minDist; RandomForests randomForest; Softmax softmax; SVM svm;
    pipeline.setClassifier( softmax );
    
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
    gui.add(tThresholdMode.setup("Threshold Mode", false));
    gui.add(triggerTimerThreshold.setup("Threshold timer (ms)", 10, 1, 1000));
    gui.add(volThreshold.setup("volThreshold", 0.6, 0.0, 10.0));
    
    gui.setPosition(10,10);
    
    startTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    trainingClassLabel = sliderClassLabel;
    
    long timer = ofGetElapsedTimeMillis() - startTime;
    
    //High volume trigger timer
    if (timer>triggerTimerThreshold) {
        singleTrigger = true;
    }
    
    //Grt
    VectorFloat trainingSample(trainingInputs);
    VectorFloat inputVector(trainingInputs);
    
    for (int i = 0; i < 13; i++) {
        trainingSample[i] = mfccs[i];
    }
    
    inputVector = trainingSample;
    
    
    if( tRecord && !tThresholdMode){
        trainingData.addSample( sliderClassLabel, trainingSample );
    } else if (tRecord && tThresholdMode && rms > volThreshold && singleTrigger) {
        trainingData.addSample( sliderClassLabel, trainingSample );
        singleTrigger = false;
        startTime = ofGetElapsedTimeMillis();
    }
    
    
    //Update the prediction mode if active
    if( predictionModeActive && !tThresholdMode){
        if( pipeline.predict( inputVector ) ){
            predictedClassLabel = pipeline.getPredictedClassLabel();
            predictionPlot.update( pipeline.getClassLikelihoods() );
            
            sendOSC();
            
        }else{
            infoText = "ERROR: Failed to run prediction!";
        }
    } else if (predictionModeActive && tThresholdMode && rms > volThreshold && singleTrigger) {
        if( pipeline.predict( inputVector ) ){
            predictedClassLabel = pipeline.getPredictedClassLabel();
            predictionPlot.update( pipeline.getClassLikelihoods() );
            singleTrigger = false;
            startTime = ofGetElapsedTimeMillis();
            predictionAlpha = 255;
            
            sendOSC();
        }
    }
    
    if (tThresholdMode && predictionAlpha > 0  ) predictionAlpha-=5;
    if (!tThresholdMode) predictionAlpha = 255;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //RMS
    ofFill();
    if (singleTrigger) {
        ofSetColor(255);
    }else {
        ofSetColor(100);
    }
    ofDrawRectangle(10, 210, ofClamp(20*(rms),0,200), 10);
    ofSetColor(255);
    char rmsString[255]; // an array of chars
    sprintf(rmsString, "RMS: %.2f", rms);
    smallFont.drawString(rmsString, 10, 240);
    
    //Threshold line
    if (tThresholdMode) {
        ofSetColor(255,0,0);
        ofSetLineWidth(5);
        ofDrawLine(volThreshold*20 + 10, 210, volThreshold*20 + 10, 220);
    }
    
    //Draw MFCCs
    ofSetColor(255);
    int mw = 200;
    int mfccGraphH = 20;
    float bin_w = (float) mw / 13;
    for (int i = 0; i < 13; i++){
        float bin_h = -1 * (mfccs[i] * mfccGraphH);
        ofDrawRectangle(i*bin_w + 10, 285, bin_w, bin_h);
    }
    
    
    //Draw the info text
    int marginX, marginY, graphX, graphY = 10;
    int graphW = ofGetWidth() - graphX*2;
    int graphH = 150;
    float infoX = 10;
    float infoW = 200;
    float textX = marginX;
    float textY = 300;
    float textSpacer = smallFont.getLineHeight() * 1.5;
    
    ofFill();
    ofSetColor( 255, 255, 255 );
    
    smallFont.drawString( "MFCCS CLASSIFIER EXAMPLE", textX, textY +20); textY += textSpacer*2;
    smallFont.drawString( "Num Samples: " + ofToString( trainingData.getNumSamples() ), textX, textY ); textY += textSpacer;
    textY += textSpacer;
    smallFont.drawString( "Total input values: "+ofToString(trainingInputs), textX, textY ); textY += textSpacer;
    ofSetColor(0,255,0);
    smallFont.drawString( infoText, textX, textY ); textY += textSpacer;
    textY += textSpacer;
    
    //Update the graph position
    graphX = infoX + infoW + 15;
    graphW = ofGetWidth() - graphX - 15;
    
    
    //If the model has been trained, then draw this
    if( pipeline.getTrained() ){
        ofSetLineWidth(1);
        predictionPlot.draw( graphX, graphY, graphW, graphH ); graphY += graphH * 1.1;
        std::string txt = "Predicted Class: " + ofToString( predictedClassLabel );
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        ofSetColor(255, predictionAlpha);
        hugeFont.drawString( txt, ofGetWidth()/2 - bounds.width*0.5, ofGetHeight() - bounds.height*3 );
    }
    gui.draw();
}



//--------------------------------------------------------------
void ofApp::sendOSC (){
    ofxOscMessage m;
    m.setAddress(oscAddress);
    m.addIntArg(pipeline.getPredictedClassLabel());
    sender.sendMessage(m, false);
}

//--------------------------------------------------------------
void ofApp::audioRequested 	(float * output, int bufferSize, int nChannels){
    //	static double tm;
    for (int i = 0; i < bufferSize; i++){
        wave = lAudioIn[i];
        
        //Calculate the mfccs
        if (mfft.process(wave)) { //is the window buffer full?
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
            mfcc.mfcc(mfft.magnitudes, mfccs);
        }
        
        memset(lAudioOut, 0, initialBufferSize * sizeof(float));
        memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    }
}

//--------------------------------------------------------------
void ofApp::audioReceived 	(float * input, int bufferSize, int nChannels){
    float sum = 0;
    for (int i = 0; i < bufferSize; i++){
        
        lAudioIn[i] = input[i*2];
        rAudioIn[i] = input[i*2+1];
        
        sum += input[i*2] * input[i*2];
    }
    rms = sqrt(sum);
}


//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamStop();
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
        case 'm':
            tThresholdMode =! tThresholdMode;
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

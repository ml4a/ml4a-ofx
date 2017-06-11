#include "ofApp.h"


//--------------------------------------------------------------
void GestureRecognitionPipelineThreaded::startTraining(RegressionData *trainingData) {
    this->trainingData = trainingData;
    startThread();
    training = true;
}

//--------------------------------------------------------------
void GestureRecognitionPipelineThreaded::threadedFunction() {
    while(isThreadRunning()) {
        if(lock()) {
            success = train(*trainingData);
            training = false;
            unlock();
            stopThread();
        } else {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

//--------------------------------------------------------------
void ofApp::setup() {
    
    string ccvPath = ofToDataPath("../../../../data/image-net-2012.sqlite3");
//    string ccvPath = ofToDataPath("image-net-2012.sqlite3");
    
    oscDestination = DEFAULT_OSC_DESTINATION;
    oscAddress = DEFAULT_OSC_ADDRESS;
    oscPort = DEFAULT_OSC_PORT;

    ofSetWindowShape(640, 480);
    ofBackground(150);
    
    // ccv
    ccv.setup(ccvPath);
    if (!ccv.isLoaded()) return;
    ccv.setEncode(true);
    ccv.start();
    
    cam.initGrabber(320, 240);
    
    //GUI
    bTrain.addListener(this, &ofApp::train);
    bSave.addListener(this, &ofApp::save);
    bLoad.addListener(this, &ofApp::load);
    bClear.addListener(this, &ofApp::clear);
    bAddSlider.addListener(this, &ofApp::addSlider);
    bOscSettings.addListener(this, &ofApp::eChangeOscSettings);
    
    gTraining.setName("Training");
    gTraining.add(numHiddenNeurons.set("hidden neurons", 10, 5, 50));
    gTraining.add(maxEpochs.set("epochs", 100, 20, 1000));
    
    gOscSettings.setName("OSC settings");
    gOscSettings.add(gOscDestination.set("IP", oscDestination));
    gOscSettings.add(gOscPort.set("port", ofToString(oscPort)));
    gOscSettings.add(gOscAddress.set("message", oscAddress));
    
    gui.setup();
    gui.setPosition(10,10);
    gui.setName("Convnet regressor");
    gui.add(gTraining);
    gui.add(tRecord.setup("Record", false));
    gui.add(bClear.setup("Clear training data"));
    gui.add(bTrain.setup("Train"));
    gui.add(tPredict.setup("Predict", false));
    gui.add(bSave.setup("Save model"));
    gui.add(bLoad.setup("Load model"));
    gui.add(lerpAmt.set("Prediction lerp", 0.2, 0.01, 1.0));
    gui.add(gOscSettings);
    gui.add(bOscSettings.setup("change OSC settings"));
    gui.loadFromFile(ofToDataPath("settings_convnetR.xml"));
    tPredict = false;
    
    guiSliders.setup();
    guiSliders.setPosition(10,gui.getHeight()+25);
    guiSliders.setName("Sliders");
    guiSliders.add(bAddSlider.setup("Add Slider"));
    addSlider();
    
    setupOSC();
}

//--------------------------------------------------------------
void ofApp::setupOSC() {
    sender.setup(oscDestination, oscPort);
}

//--------------------------------------------------------------
void ofApp::eChangeOscSettings() {
    string input = ofSystemTextBoxDialog("Send OSC to what destination IP", oscDestination);
    bool toSwitchOsc = false;
    if (input != "" && input != oscDestination) {
        oscDestination = input;
        gOscDestination.set(oscDestination);
        toSwitchOsc = true;
    }
    input = ofSystemTextBoxDialog("Send OSC to what destination port", ofToString(oscPort));
    if (ofToInt(input) > 0 && ofToInt(input) != oscPort) {
        oscPort = ofToInt(input);
        gOscPort.set(ofToString(oscPort));
        toSwitchOsc = true;
    }
    input = ofSystemTextBoxDialog("Send OSC with what message address", oscAddress);
    if (input != "" && input != oscAddress) {
        oscAddress = input;
        gOscAddress.set(oscAddress);
    }
    if (toSwitchOsc) {
        setupOSC();
    }
}

//--------------------------------------------------------------
void ofApp::eSlider(float & v) {
    sendOSC();
}

//--------------------------------------------------------------
void ofApp::exit() {
    gui.saveToFile(ofToDataPath("settings_convnetR.xml"));
    ccv.setEncode(false);
    ccv.stop();
    pipeline.stopThread();
}

//--------------------------------------------------------------
void ofApp::addSlider() {
    if (pipeline.getNumTrainingSamples() > 0) {
        ofLog(OF_LOG_ERROR, "Can't add slider, already have training samples");
        return;
    }
    ofParameter<float> newSlider;
    guiSliders.add(newSlider.set("y"+ofToString(1+values.size()), 0.5, 0.0, 1.0));
    newSlider.addListener(this, &ofApp::eSlider);
    values.push_back(newSlider);
    targetValues.resize(values.size());
    trainingData.setInputAndTargetDimensions( SIZE_INPUT_VECTOR, values.size() );
}

//--------------------------------------------------------------
void ofApp::setupRegressor() {
    unsigned int numInputNeurons = trainingData.getNumInputDimensions();
    unsigned int numOutputNeurons = 1; //1 as we are using multidimensional regression
    
    //Initialize the MLP
    MLP mlp;
    mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::SIGMOID, Neuron::SIGMOID );
    
    //Set the training settings
    mlp.setMaxNumEpochs( maxEpochs ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
    mlp.setMinChange( 1.0e-10 ); //This sets the minimum change allowed in training error between any two epochs
    mlp.setLearningRate( 0.01 ); //This sets the rate at which the learning algorithm updates the weights of the neural network
    mlp.setNumRandomTrainingIterations( 1 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
    mlp.setUseValidationSet( true ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
    mlp.setValidationSetSize( 15 ); //Use 20% of the training data for validation during the training phase
    mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
    
    //The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e. [0.0 1.0])
    mlp.enableScaling( true );
    
    pipeline << MultidimensionalRegression(mlp,true);
}

//--------------------------------------------------------------
void ofApp::updateParameters() {
    for (int i=0; i<values.size(); i++) {
        values[i] = ofLerp(values[i], targetValues[i], lerpAmt);
    }
    sendOSC();
}

//--------------------------------------------------------------
void ofApp::update() {
    if (!ccv.isLoaded()) {
        ofDrawBitmapString("Network file not found! Check your data folder to make sure it exists.", 20, 20);
        return;
    }
    
    if (isTraining) {
        if (!pipeline.training) {
            infoText = pipeline.success ? "Pipeline trained" : "WARNING: Failed to train pipeline";
            isTraining = false;
            ofBackground(150);
        } else if (ofGetFrameNum() % 15 == 0) {
            ofBackground(ofRandom(255),ofRandom(255),ofRandom(255));
        }
    }
    else if (tPredict) {
        updateParameters();
    }
    
    cam.update();
    if (!cam.isFrameNew()) {
        return;
    } else if (cam.isFrameNew() && ccv.isReady()) {
        ccv.update(cam, ccv.numLayers()-1);
    }
    
    if ((tRecord||tPredict)&&ccv.hasNewResults()) {
        featureEncoding = ccv.getEncoding();
        VectorFloat inputVector(featureEncoding.size());
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
        else if( tPredict ){
            if( pipeline.predict( inputVector ) ){
                for (int p=0; p<values.size(); p++) {
                    targetValues[p] = pipeline.getRegressionData()[p];
                }
            }else{
                infoText = "ERROR: Failed to run prediction!";
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);

    cam.draw(270, 10);
    ofDrawBitmapStringHighlight( "Num Samples: " + ofToString( trainingData.getNumSamples() ), 272, 30 + cam.getHeight() );
    ofDrawBitmapStringHighlight( infoText, 272, 50 + cam.getHeight() );
    
    gui.draw();
    guiSliders.draw();
}

//--------------------------------------------------------------
void ofApp::train() {
    ofLog(OF_LOG_NOTICE, "Training...");
    tRecord = false;
    tPredict = false;
    setupRegressor();
    pipeline.startTraining( &trainingData );
    pipeline.startThread();
    infoText = "Training!! please wait.";
    isTraining = true;
    
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::save() {
    if( trainingData.save( ofToDataPath("TrainingDataConvnetR.grt") ) ){
        infoText = "Training data saved to file";
    } else infoText = "WARNING: Failed to save training data to file";
}

//--------------------------------------------------------------
void ofApp::load() {
    if( trainingData.load( ofToDataPath("TrainingDataConvnetR.grt") ) ){
        infoText = "Training data loaded from file";
        train();
    } else infoText = "WARNING: Failed to load training data from file";
}

//--------------------------------------------------------------
void ofApp::clear() {
    trainingData.clear();
    pipeline.clear();
    infoText = "Training data cleared";
    tPredict = false;
}

//--------------------------------------------------------------
void ofApp::sendOSC() {
    ofxOscMessage m;
    m.setAddress(oscAddress);
    for (int i=0; i<values.size(); i++) {
        m.addFloatArg(values[i]);
    }
    sender.sendMessage(m, false);
}

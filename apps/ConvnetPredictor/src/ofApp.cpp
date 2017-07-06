#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
    
#ifdef RELEASE
    string ccvPath = ofToDataPath("image-net-2012.sqlite3");
#else
    string ccvPath = ofToDataPath("../../../../data/image-net-2012.sqlite3");
#endif
    
    oscDestination = DEFAULT_OSC_DESTINATION;
    oscAddress = DEFAULT_OSC_ADDRESS;
    oscPort = DEFAULT_OSC_PORT;
    
    ofSetWindowShape(800, 360);
    ofBackground(150);
    
    // ccv
    ccv.setup(ccvPath);
    if (!ccv.isLoaded()) return;
    ccv.setEncode(true);
    ccv.start();
    
    //GUI
    bTrain.addListener(this, &ofApp::train);
    bSave.addListener(this, &ofApp::save);
    bLoad.addListener(this, &ofApp::load);
    bClear.addListener(this, &ofApp::clear);
    bAddSlider.addListener(this, &ofApp::addSlider);
    bAddCategorical.addListener(this, &ofApp::addCategorical);
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
    gui.setName("Convnet predictor");
    gui.add(gDeviceId.set("deviceId", ofToString(DEFAULT_DEVICE_ID)));
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
    gui.loadFromFile(ofToDataPath("settings_convnetP.xml"));
    tPredict = false;
    
    guiSliders.setup();
    guiSliders.setPosition(580, 10);
    guiSliders.setName("Outputs");
    guiSliders.add(bAddSlider.setup("Add Slider"));
    guiSliders.add(bAddCategorical.setup("Add Categorical"));
    addSlider();
    
    // osc
    setupOSC();
    
    // camera
    int idx = ofToInt(gDeviceId.get());
    if (idx > cam.listDevices().size()-1) {
        cam.setDeviceID(0);
    } else {
        cam.setDeviceID(idx);
    }
    cam.initGrabber(320, 240);
    
    tRecord = false;
    tPredict = false;
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
void ofApp::eCategorical(int & v) {
    sendOSC();
}

//--------------------------------------------------------------
void ofApp::exit() {
    gui.saveToFile(ofToDataPath("settings_convnetP.xml"));
    ccv.setEncode(false);
    ccv.stop();
    for (int p=0; p<learners.size(); p++) {
        learners[p]->stopThread();
    }
}

//--------------------------------------------------------------
void ofApp::addSlider() {
    RegressionThreaded *learner = new RegressionThreaded();
    string name = "y"+ofToString(1+learners.size());
    learner->setup(name, &guiSliders, SIZE_INPUT_VECTOR);
    learners.push_back(learner);
}

//--------------------------------------------------------------
void ofApp::addCategorical() {
    CategoricalThreaded *learner = new CategoricalThreaded();
    string name = "y"+ofToString(1+learners.size());
    learner->setup(name, &guiSliders, SIZE_INPUT_VECTOR);
    learners.push_back(learner);
}

//--------------------------------------------------------------
void ofApp::updateParameters() {
    for (int i=0; i<learners.size(); i++) {
        learners[i]->update();
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
        
        isTraining = false;
        for (int p=0; p<learners.size(); p++) {
            if (learners[p]->training) {
                isTraining = true;
            }
        }
        
        if (!isTraining) {
            
            // not for all of them?
            infoText = learners[0]->success ? "Pipeline trained" : "WARNING: Failed to train pipeline";
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
    
    
    
    if ((tRecord||tPredict) && ccv.hasNewResults()) {
        featureEncoding = ccv.getEncoding();
        VectorFloat inputVector(featureEncoding.size());
        for (int i=0; i<featureEncoding.size(); i++) {
            inputVector[i] =  featureEncoding[i];
        }
        
        if( tRecord ) {

            
            for (int p=0; p<learners.size(); p++) {
                VectorFloat targetVector(1);
                if( !learners[p]->addSample(&inputVector) ){

                    // this overwrites all of them.... fix this?
                    infoText = "WARNING: Failed to add training sample to training data!";
                }
                
            }
            
            
            
            
        }
        else if( tPredict ){
            
            
            
            for (int p=0; p<learners.size(); p++) {
                
                if (learners[p]->predict(&inputVector)) {
                    
                    //targetSliders[p] = learners[p]->getRegressionData()[0];
                    
                    //cout << " got to "<<p << " " << targetSliders[p] << endl;
                    
                }else{
                    
                    // this overwrites all of them.... fix this?
                    infoText = "ERROR: Failed to run prediction!";
                }
            
                
            }
            

            
            
            
        }
        
        
        
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);
    
    cam.draw(235, 10);
    //ofDrawBitmapStringHighlight( "Num Samples: " + ofToString( trainingData.getNumSamples() ), 237, 30 + cam.getHeight() );
    ofDrawBitmapStringHighlight( infoText, 237, 50 + cam.getHeight() );
    
    gui.draw();
    guiSliders.draw();
}

//--------------------------------------------------------------
void ofApp::train() {
    ofLog(OF_LOG_NOTICE, "Training...");
    tRecord = false;
    tPredict = false;

    for (int p=0; p<learners.size(); p++) {
        learners[p]->setupModel();
        learners[p]->startTraining();
        learners[p]->startThread();
    }

    infoText = "Training!! please wait.";
    isTraining = true;
    
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::save() {
    // add classifier data
//    if( trainingData.save( ofToDataPath("TrainingDataConvnetR.grt") ) ){
//        infoText = "Training data saved to file";
//    } else infoText = "WARNING: Failed to save training data to file";
}

//--------------------------------------------------------------
void ofApp::load() {
    // add classifier data
//    if( trainingData.load( ofToDataPath("TrainingDataConvnetR.grt") ) ){
//        infoText = "Training data loaded from file";
//        train();
//    } else infoText = "WARNING: Failed to load training data from file";
}

//--------------------------------------------------------------
void ofApp::clear() {
    for (int p=0; p<learners.size(); p++) {
        learners[p]->clear();
        learners[p]->clear();
    }
    infoText = "Training data cleared";
    tPredict = false;
}

//--------------------------------------------------------------
void ofApp::sendOSC() {
    ofxOscMessage msg;
    msg.setAddress(oscAddress);
    for (int p=0; p<learners.size(); p++) {
        msg.addFloatArg(learners[p]->getValue());
    }
    sender.sendMessage(msg, false);
}

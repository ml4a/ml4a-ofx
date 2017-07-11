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
    largeFont.load(ofToDataPath("verdana.ttf"), 20, true, true);
    largeFont.setLineHeight(38.0f);
    
    // ccv
    ccv.setup(ccvPath);
    if (!ccv.isLoaded()) return;
    ccv.setEncode(true);
    ccv.start();
    
    //GUI
    bTrain.addListener(this, &ofApp::train);
    bSave.addListener(this, &ofApp::eSave);
    bLoad.addListener(this, &ofApp::eLoad);
    bClear.addListener(this, &ofApp::clear);
    bAddSlider.addListener(this, &ofApp::eAddSlider);
    bAddCategorical.addListener(this, &ofApp::eAddCategorical);
    bOscSettings.addListener(this, &ofApp::eChangeOscSettings);
    bCameraSettings.addListener(this, &ofApp::eChangeCamera);

    gTraining.setName("Training");
    //gTraining.add(numHiddenNeurons.set("hidden neurons", 10, 5, 50));
    //gTraining.add(maxEpochs.set("epochs", 100, 20, 1000));
    
    gOscSettings.setName("OSC settings");
    gOscSettings.add(gOscDestination.set("IP", oscDestination));
    gOscSettings.add(gOscPort.set("port", ofToString(oscPort)));
    gOscSettings.add(gOscAddress.set("message", oscAddress));
    
    gui.setup();
    gui.setPosition(10,10);
    gui.setName("Convnet predictor");
    gui.add(gDeviceId.set("deviceId", ofToString(DEFAULT_DEVICE_ID)));
    gui.add(bCameraSettings.setup("change Camera settings"));
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
    
    guiSliders.setup();
    guiSliders.setPosition(580, 10);
    guiSliders.setName("Outputs");
    guiSliders.add(bAddSlider.setup("Add Slider"));
    guiSliders.add(bAddCategorical.setup("Add Categorical"));
    
    // osc
    oscDestination = gOscDestination.get();
    oscAddress = gOscAddress.get();
    oscPort = ofToInt(gOscPort.get());
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
    numSamples = 0;
    
    //addSlider();
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
void ofApp::eChangeCamera() {
    string msg = "Select camera:";
    int idx = 0;
    for (auto d : cam.listDevices()) {
        msg += "\n "+ofToString(idx++)+": "+d.deviceName;
    }
    string selection = ofSystemTextBoxDialog(msg);
    if (selection != "") {
        int newDeviceId = ofToInt(selection);
        cam.setDeviceID(newDeviceId);
        cam.initGrabber(320, 240);
        gDeviceId.set(ofToString(newDeviceId));
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
RegressionThreaded * ofApp::addSlider() {
    RegressionThreaded *learner = new RegressionThreaded();
    string name = "y"+ofToString(1+learners.size());
    learner->setup(name, &guiSliders, SIZE_INPUT_VECTOR);
    learner->slider.addListener(this, &ofApp::eSlider);
    learners.push_back(learner);
    return learner;
}

//--------------------------------------------------------------
void ofApp::eAddSlider() {
    addSlider();
}

//--------------------------------------------------------------
void ofApp::eAddCategorical() {
    string numClasses = ofSystemTextBoxDialog("How many classes?");
    if (numClasses != "") {
        addCategorical(ofToInt(numClasses));
    }
}

//--------------------------------------------------------------
CategoricalThreaded * ofApp::addCategorical(int numClasses) {
    CategoricalThreaded *learner = new CategoricalThreaded();
    string name = "y"+ofToString(1+learners.size());
    learner->setup(name, &guiSliders, SIZE_INPUT_VECTOR, numClasses);
    learner->slider.addListener(this, &ofApp::eCategorical);
    learners.push_back(learner);
    return learner;
}

//--------------------------------------------------------------
void ofApp::updateParameters() {
    for (int i=0; i<learners.size(); i++) {
        if (learners[i]->getTrained()) {
            learners[i]->update(lerpAmt);
        }
    }
    sendOSC();
}

//--------------------------------------------------------------
void ofApp::update() {
    // check if ccv loaded
    if (!ccv.isLoaded()) {
        return;
    }
    
    // update training status
    if (isTraining) {
        isTraining = false;
        for (int p=0; p<learners.size(); p++) {
            if (learners[p]->training) {
                isTraining = true;
            }
        }
        if (!isTraining) {
            infoText = "Pipeline trained";
            for (int p=0; p<learners.size(); p++) {
                if (!learners[0]->success) {
                    infoText =  "WARNING: Failed to train pipeline";
                }
            }
            ofBackground(150);
        }
        else if (ofGetFrameNum() % 15 == 0) {
            ofBackground(ofRandom(255),ofRandom(255),ofRandom(255));
        }
    }
    
    // update parameters
    else if (tPredict) {
        updateParameters();
    }
    
    // update cam and ccv
    cam.update();
    if (!cam.isFrameNew()) {
        return;
    }
    else if (cam.isFrameNew() && ccv.isReady()) {
        ccv.update(cam, ccv.numLayers()-1);
    }
    
    // record or predict
    if ((tRecord||tPredict) && ccv.hasNewResults()) {
        featureEncoding = ccv.getEncoding();
        VectorFloat inputVector(featureEncoding.size());
        for (int i=0; i<featureEncoding.size(); i++) {
            inputVector[i] = featureEncoding[i];
        }
        if(tRecord) {
            for (int p=0; p<learners.size(); p++) {
                bool success = learners[p]->addSample(&inputVector);
                if (!success){
                    infoText = "WARNING: Failed to add training sample to training data!";
                }
            }
            numSamples++;
        }
        else if (tPredict){
            for (int p=0; p<learners.size(); p++) {
                bool success = learners[p]->predict(&inputVector);
                if (!success) {
                    infoText = "ERROR: Failed to run prediction!";
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    // check if ccv loaded
    if (!ccv.isLoaded()) {
        ofDrawBitmapString("Network file not found! Check your data folder to make sure it exists.", 20, 20);
        return;
    }
    
    // draw interface
    ofSetColor(255);
    cam.draw(235, 10);
    
    ofDrawBitmapStringHighlight( "Num samples recorded: " + ofToString(numSamples), 237, 30 + cam.getHeight() );
    if (infoText != ""){
        ofDrawBitmapStringHighlight( infoText, 237, 50 + cam.getHeight() );
    }
    
    if (learners.size() == 1 && learners[0]->isClassifier() && learners[0]->getTrained() && tPredict) {
        string txt = "Predicted Class: " + ofToString(((CategoricalThreaded *) learners[0])->slider);
        ofSetColor(0,255,0);
        largeFont.drawString(txt, 237, 92 + cam.getHeight());
    }
    
    gui.draw();
    guiSliders.draw();
}

//--------------------------------------------------------------
void ofApp::train() {
    ofLog(OF_LOG_NOTICE, "Training...");
    tRecord = false;
    tPredict = false;

    for (int p=0; p<learners.size(); p++) {
        learners[p]->setupModel(0);
        learners[p]->startTraining();
        learners[p]->startThread();
    }

    infoText = "Training!! please wait.";
    isTraining = true;
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::save(string modelName) {
    ofSystem("mkdir "+ofToDataPath(modelName+"/"));
    for (int p=0; p<learners.size(); p++) {
        string modelType = learners[p]->isClassifier() ? "c" : "r";
        learners[p]->save(ofToDataPath(modelName+"/p"+ofToString(p)+"_"+modelType+".grt"));
    }
    gui.saveToFile(ofToDataPath(modelName+"/settings.xml"));
}

//--------------------------------------------------------------
void ofApp::load(string modelPath) {
    ofDirectory dir;
    dir.open(modelPath);
    int n = dir.listDir();
    for (int i=0; i<n; i++) {
        string fullPath = dir.getFile(i).getAbsolutePath();
        vector<string> path = ofSplitString(dir.getFile(i).getFileName(), "_");
        if (path.size() < 2) {
            continue;
        }
        string fullName = path[path.size()-1];
        if (fullName == "r.grt") {
            RegressionThreaded *learner = addSlider();
            learner->load(fullPath);
        }
        else if (fullName == "c.grt") {
            CategoricalThreaded * learner = addCategorical(2);
            learner->load(fullPath);
            learner->slider.set(learner->slider.getName(), 1, 1, learner->getNumClasses());
        }
    }
    if (n > 0) {
        tPredict = true;
        numSamples = learners[0]->getNumTrainingSamples();
    }
    gui.loadFromFile(modelPath+"/settings.xml");

    oscDestination = gOscDestination.get();
    oscAddress = gOscAddress.get();
    oscPort = ofToInt(gOscPort.get());
    setupOSC();
}

//--------------------------------------------------------------
void ofApp::clear() {
    for (int p=0; p<learners.size(); p++) {
        learners[p]->clear();
        learners[p]->clear();
    }
    infoText = "Training data cleared";
    tPredict = false;
    numSamples = 0;
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

//--------------------------------------------------------------
void ofApp::eSave() {
    string modelName = ofSystemTextBoxDialog("Name the model");
    if (modelName != "") {
        save(modelName);
    }
}

//--------------------------------------------------------------
void ofApp::eLoad() {
    ofFileDialogResult result = ofSystemLoadDialog("Which model to load?", true);
    if (result.bSuccess) {
        load(result.filePath);
    }
}

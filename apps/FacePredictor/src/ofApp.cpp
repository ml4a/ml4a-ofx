#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {
    
#ifdef RELEASE
    string landmarksPath = ofToDataPath("shape_predictor_68_face_landmarks.dat");
#else
    string landmarksPath = ofToDataPath("../../../../data/shape_predictor_68_face_landmarks.dat");
#endif
    
    oscDestination = DEFAULT_OSC_DESTINATION;
    oscAddress = DEFAULT_OSC_ADDRESS;
    oscPort = DEFAULT_OSC_PORT;
    
    ofSetWindowShape(800, 360);
    ofBackground(150);
    largeFont.load(ofToDataPath("verdana.ttf"), 20, true, true);
    largeFont.setLineHeight(38.0f);
    
    // facetracker
    numInputs = SIZE_INPUT_VECTOR;  //GESTUREINPUTS*gestureBool+ORIENTATIONINPUTS*orientationBool+RAWINPUTS*rawBool; //RAWINPUTS=136, ORIENTATIONINPUTS=9, GESTUREINPUTS=5
    tracker.setup(landmarksPath);
    
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
    gui.setName("Face predictor");
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
    gui.loadFromFile(ofToDataPath("settings_faceP.xml"));
    
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
    gui.saveToFile(ofToDataPath("settings_faceP.xml"));
    for (int p=0; p<learners.size(); p++) {
        learners[p]->stopThread();
    }
}

//--------------------------------------------------------------
RegressionThreaded * ofApp::addSlider() {
    RegressionThreaded *learner = new RegressionThreaded();
    string name = "y"+ofToString(1+learners.size());
    learner->setup(name, &guiSliders, numInputs);
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
    learner->setup(name, &guiSliders, numInputs, numClasses);
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
        } else if (ofGetFrameNum() % 15 == 0) {
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
    else if (cam.isFrameNew()) {
        tracker.update(cam);
    }
    
    // record or predict
    if (tRecord||tPredict) {
        VectorFloat inputVector(numInputs);

        if (tracker.size()>0) {
            auto facePoints = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::ALL_FEATURES);
            for (int i = 0; i<facePoints.size(); i++) {
                ofPoint p = facePoints.getVertices()[i].getNormalized();
                inputVector[2*i] = p.x;
                inputVector[2*i+1] = p.y;
            }
        }

        if( tRecord ) {
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
    ofPushMatrix();
    ofTranslate(235, 10);
    
    ofSetColor(255);
    cam.draw(0, 0);
    
    for (int i = 0; i<tracker.size(); i ++) {
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
    ofPopMatrix();
    
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

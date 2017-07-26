#include "ofApp.h"


void ofApp::setup(){
    ofSetWindowShape(320, 180);
    oscPort = OSC_PORT;
    oscAddressRoot = OSC_ADDRESS_ROOT;
    fadeStep = 255 / stayOnScreenFrames;
    setupOSC();
}

void ofApp::setupOSC() {
    receiver.setup(oscPort);
}

void ofApp::loadSettings() {
    ofLog() << "reloading settings....";
    ofXml xml;
    xml.load(ofToDataPath("settings_keyboardOsc.xml"));
    xml.setTo("OSCKeySimulator");
    oscPort = ofToInt(xml.getValue("port"));
    oscAddressRoot = xml.getValue("address");
    oscAddressEnable = xml.getValue("enable");
    oscAddressDisable = xml.getValue("disable");
    keyDuration = ofToInt(xml.getValue("key-duration"));
    // Key inputs
    keyInputs.clear();
    xml.setTo("keys");
    totalKeyInputs = xml.getNumChildren();
    xml.setTo("input[0]");
    for (int i = 0; i < totalKeyInputs; i++) {
        int oscId = ofToInt(xml.getValue("osc-id"));
        string keyString = ofToString(xml.getValue("key"));
        int keyCode = keyCodes.convertStringToKeyCode(keyString);
        
        KeyInput keyInput;
        keyInput.setup(keyCode, keyString, oscId);
        keyInputs.push_back(keyInput);
        
        xml.setToSibling();
    }
    
    setupOSC();
}

void ofApp::checkSettings(){
    boost::filesystem::path filePath = ofToDataPath("settings_keyboardOsc.xml");
    time_t t = boost::filesystem::last_write_time(filePath);
    if (t != tSettings) {
        tSettings = t;
        loadSettings();
    }
}


void ofApp::update(){
    if (ofGetFrameNum() % 30 == 0) {
        checkSettings();
    }
    
    if (!canPressKey) {
        if (ofGetElapsedTimeMillis() - timer >= keyDuration) {
            triggerKeyUp();
        }
    }

    if (receivingOSC && frameCounter < stayOnScreenFrames) {
        frameCounter++;
    }else{
        receivingOSC = false;
    }
    
    while (receiver.hasWaitingMessages()) {
        
        receiver.getNextMessage(oscMessage );
        if (canPressKey) {
            if (oscMessage.getAddress() == oscAddressEnable) {
                allowVirtualKeystrokes = true;
            }
            
            if (oscMessage.getAddress() == oscAddressDisable) {
                allowVirtualKeystrokes = false;
            }
        
            if (oscMessage.getAddress() == oscAddressRoot) {
                receivingOSC = true;
                frameCounter = 0;
                
                if (allowVirtualKeystrokes) {
                    if (oscMessage.getArgType(0) == OFXOSC_TYPE_INT32 ||
                        oscMessage.getArgType(0) == OFXOSC_TYPE_INT64) {
                        oscId = (int) oscMessage.getArgAsInt(0);
                    } else if (oscMessage.getArgType(0) == OFXOSC_TYPE_FLOAT) {
                        oscId = (int) oscMessage.getArgAsFloat(0);
                    }
                    
                    int keyCode;
                    for (int i = 0; i < totalKeyInputs; i++) {
                        if (keyInputs[i].oscId == oscId) {
                            keyCode = keyInputs[i].keyCode;
                        }
                    }
                    sendKeyCode(keyCode);
                }
                
            }
            
        }else{
            oscMessage.clear();
        }
    }
}

void ofApp::draw(){
    ofBackground(0, 0, 0);
    
    
    ofSetColor(255, 255, 255);
    ofDrawRectangle(20, 20, 20, 20);

    if (allowVirtualKeystrokes) {
        ofSetColor(0, 150, 0);
        ofDrawRectangle(22, 22, 16, 16);
    }
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Allow keyboard simulation", 50, 34);

    
    ofSetColor(255, 255, 255);
    ofDrawRectangle(20, 50, 20, 20);
    
    if (triggerKeysAsToggles) {
        ofSetColor(0, 150, 0);
        ofDrawRectangle(22, 52, 16, 16);
    }
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Trigger keys as toggles", 50, 63);

    
    ofSetColor(255, 255, 255);
    ofDrawRectangle(20, 80, 20, 20);
    if (ofGetMousePressed() &&
        ofRectangle(20, 80, 20, 20).inside(ofGetMouseX(),ofGetMouseY())) {
        ofSetColor(0, 150, 0);
        ofDrawRectangle(22, 82, 16, 16);
    }
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Change settings", 50, 93);

    
    
    if (receivingOSC) {
        ofSetColor(0, 150, 0);
    }else{
        ofSetColor(150, 0, 0);
    }
    
    ofDrawCircle(30, 120, 5);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Receiving OSC on port "+ofToString(oscPort)+",\naddress: "+oscAddressRoot, 50, 124);
    
    if (oscId > -1) {
        
        string keyString = "";
        
        for (int i = 0; i < totalKeyInputs; i++) {
            if (keyInputs[i].oscId == oscId) {
                keyString = keyInputs[i].keyString;
            }
        }
        ofSetColor(255 - (fadeStep * frameCounter), 255 - (fadeStep * frameCounter), 255 - (fadeStep * frameCounter));
        ofDrawBitmapString("Keystroke: " + keyString, 20, 160);
    }
}

void ofApp::triggerKeyDown() {
    
    CGEventRef e = CGEventCreateKeyboardEvent (NULL, (CGKeyCode) charKey, true);
    CGEventPost(kCGSessionEventTap, e);
    CFRelease(e);
    
    if (triggerKeysAsToggles) {
        bool found = false;
        
        for (int i = 0; i < pressedKeys.size(); i++) {
            if (pressedKeys[i] == charKey) {
                found = true;
            }
        }
        
        if (found) {
            triggerKeyUp();
        }else{
            pressedKeys.push_back(charKey);
        }
        canPressKey = true;
    }else{
        timer = ofGetElapsedTimeMillis();
        canPressKey = false;
    }
}

void ofApp::triggerKeyUp() {
    CGEventRef a = CGEventCreateKeyboardEvent (NULL, (CGKeyCode) charKey, false);
    CGEventPost(kCGSessionEventTap, a);
    CFRelease(a);
    
    if (triggerKeysAsToggles) {
        int index = -1;
        for (int i = 0; i < pressedKeys.size(); i++) {
            if (pressedKeys[i] == charKey) {
                index = i;
            }
        }
        if (index > -1) {
            pressedKeys.erase(pressedKeys.begin() + index);
            ofLog() << "removed " + ofToString(charKey);
        }

    }
    canPressKey = true;
}

void ofApp::sendKeyCode(int key) {
    if (canPressKey) {
        charKey = (char) key;

        triggerKeyDown();
    }
}

void ofApp::mouseReleased(int x, int y, int button){
    if (x >= 20 && x <= 40 && y >= 20 && y <= 40) {
        allowVirtualKeystrokes = !allowVirtualKeystrokes;
    }
    
    else if (x >= 20 && x <= 40 && y >= 50 && y <= 70) {
        triggerKeysAsToggles = !triggerKeysAsToggles;
    }

    else if (x >= 20 && x <= 40 && y >= 80 && y <= 100) {
        ofSystem("open "+ofToDataPath("settings_keyboardOsc.xml"));
    }
}

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(720, 420);
    
    leap.setup();
    gui.setName("LeapOSC");
    gui.setPosition(520, 0);
    gui.addToggle("Left hand", &sendLeft);
    gui.addToggle("Right hand", &sendRight);
    
    // default
    oscAddress = DEFAULT_LEAP_OSC_ADDRESS;
    sendLeft = false;
    sendRight = true;
    oscOut = DEFAULT_LEAP_OSC_IP;
    portOut = DEFAULT_LEAP_OSC_PORT;
    
    ofXml xml;
    if (xml.load("settings_leap.xml")) {
        xml.setTo("LeapOSC");
        if (xml.exists("destination")) {
            oscOut = xml.getValue<string>("destination");
        }
        if (xml.exists("port")) {
            portOut = xml.getValue<int>("port");
        }
        if (xml.exists("address")) {
            oscAddress = xml.getValue<string>("address");
        }
    }
    
    osc.setup(oscOut, portOut);
}

//--------------------------------------------------------------
void ofApp::exit(){
    leap.close();
}

//--------------------------------------------------------------
void ofApp::update(){
    leap.update();
    
    ofxOscMessage msg;
    msg.setAddress(oscAddress);
    
    if (sendLeft) {
        // get data left
        ofPoint lhp = leap.getLeftHandPosition();
        ofPoint lhv = leap.getLeftHandVelocity();
        vector<ofPoint> & ltips = leap.getLeftHandFingerTips();
        float lohs = leap.getLeftOpenHandSize();
        ofPoint lhn = leap.getLeftHandNormal();
        ofPoint lhd = leap.getLeftHandDirection();
        float lhr = leap.getLeftHandRoll();
        float lhpt = leap.getLeftHandPitch();
        float lhw = leap.getLeftHandYaw();
        
        msg.addFloatArg(lhp.x);
        msg.addFloatArg(lhp.y);
        msg.addFloatArg(lhp.z);
        msg.addFloatArg(lhv.x);
        msg.addFloatArg(lhv.y);
        msg.addFloatArg(lhv.z);
        for (int f=0; f<5; f++) {
            msg.addFloatArg(ltips[f].x);
            msg.addFloatArg(ltips[f].y);
            msg.addFloatArg(ltips[f].z);
        }
        msg.addFloatArg(lohs);
        msg.addFloatArg(lhn.x);
        msg.addFloatArg(lhn.y);
        msg.addFloatArg(lhn.z);
        msg.addFloatArg(lhd.x);
        msg.addFloatArg(lhd.y);
        msg.addFloatArg(lhd.z);
        msg.addFloatArg(lhr);
        msg.addFloatArg(lhpt);
        msg.addFloatArg(lhw);
    }
    
    if (sendRight) {
        // get data right
        ofPoint rhp = leap.getRightHandPosition();
        ofPoint rhv = leap.getRightHandVelocity();
        vector<ofPoint> & rtips = leap.getRightHandFingerTips();
        float rohs = leap.getRightOpenHandSize();
        ofPoint rhn = leap.getRightHandNormal();
        ofPoint rhd = leap.getRightHandDirection();
        float rhr = leap.getRightHandRoll();
        float rhpt = leap.getRightHandPitch();
        float rhw = leap.getRightHandYaw();
        
        msg.addFloatArg(rhp.x);
        msg.addFloatArg(rhp.y);
        msg.addFloatArg(rhp.z);
        msg.addFloatArg(rhv.x);
        msg.addFloatArg(rhv.y);
        msg.addFloatArg(rhv.z);
        for (int f=0; f<5; f++) {
            msg.addFloatArg(rtips[f].x);
            msg.addFloatArg(rtips[f].y);
            msg.addFloatArg(rtips[f].z);
        }
        msg.addFloatArg(rohs);
        msg.addFloatArg(rhn.x);
        msg.addFloatArg(rhn.y);
        msg.addFloatArg(rhn.z);
        msg.addFloatArg(rhd.x);
        msg.addFloatArg(rhd.y);
        msg.addFloatArg(rhd.z);
        msg.addFloatArg(rhr);
        msg.addFloatArg(rhpt);
        msg.addFloatArg(rhw);
    }
    numArgs = msg.getNumArgs();
    osc.sendMessage(msg);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    leap.draw(0, 0, 512, 384);
    
    ofPushStyle();
    ofSetColor(255);
    ofDrawBitmapStringHighlight("streaming "+ofToString(numArgs)+" values (address = \""+oscAddress+"\") to "+oscOut+", port "+ofToString(portOut), 20, ofGetHeight()-4);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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

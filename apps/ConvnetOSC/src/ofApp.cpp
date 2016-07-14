#include "ofApp.h"


void ofApp::setup() {
    ofSetWindowShape(640, 480);
    cam.initGrabber(320, 240);
    
    ccv.setup("image-net-2012.sqlite3");
    if (!ccv.isLoaded()) return;
    
    oscPort = 6448;
    oscAddress = "/wek/inputs";
    oscHost = "localhost";
    
    osc.setup(oscHost, oscPort);
    sending = false;
}

void ofApp::update() {
    cam.update();
}

void ofApp::sendOsc() {
    featureEncoding = ccv.encode(cam, ccv.numLayers()-1);
    
    msg.clear();
    msg.setAddress(oscAddress);
    for (int i=0; i<featureEncoding.size(); i++) {
        msg.addFloatArg(featureEncoding[i]);
    }
    osc.sendMessage(msg);
    
}

void ofApp::keyPressed(int key) {
    if (key==' ') {
        sending = !sending;
    }
}

void ofApp::draw() {
    if (!ccv.isLoaded()) {
        ofDrawBitmapString("Network file not found! Check your data folder to make sure it exists.", 20, 20);
        return;
    }
    

    if (sending) {
        ofBackground(0, 255, 0);
        ofSetColor(0, 100);
        ofDrawRectangle(10, 240, 400, 42);
        ofSetColor(255);
        ofDrawBitmapString("sending "+ofToString(msg.getNumArgs())+" values", 10, 260);
        ofDrawBitmapString("to "+oscHost+" port "+ofToString(oscPort)+", address \""+oscAddress+"\"", 10, 280);
        sendOsc();
    }
    else {
        ofBackground(255, 0, 0);
    }
    
    ofSetColor(255);
    cam.draw(0, 0);

}

#include "ofApp.h"


void ofApp::setup() {
    ofSetWindowShape(540, 360);
    ofSetWindowTitle("ConvnetOSC");
    cam.initGrabber(320, 240);
    
    ccv.setup("../../../../models/image-net-2012.sqlite3");
    if (!ccv.isLoaded()) return;
    
    // default settings
    oscDestination = OSC_DESTINATION_DEFAULT;
    oscAddressRoot = OSC_ADDRESS_ROOT_DEFAULT;
    oscPort = OSC_PORT_DEFAULT;
    
    // load settings from file
    ofXml xml;
    xml.load("settings.xml");
    xml.setTo("ConvnetOSC");
    oscDestination = xml.getValue("ip");
    oscPort = ofToInt(xml.getValue("port"));
    oscAddressRoot = xml.getValue("address");

    osc.setup(oscDestination, oscPort);
    sending = false;
}

void ofApp::update() {
    cam.update();
}

void ofApp::sendOsc() {
    featureEncoding = ccv.encode(cam, ccv.numLayers()-1);
    
    msg.clear();
    msg.setAddress(oscAddressRoot);
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
        ofDrawBitmapString("Network file not found!\nCheck your data folder to make sure it exists.", 20, 20);
        return;
    }

    if (sending) {
        ofBackground(0, 255, 0);
        
        ofSetColor(0, 100);
        ofDrawRectangle(10, 280, 520, 66);
        ofSetColor(255);
        string txt = "sending "+ofToString(msg.getNumArgs())+" values.\n";
        txt += "to "+oscDestination+", port "+ofToString(oscPort)+",\n";
        txt += "osc address \""+oscAddressRoot+"\"\n";
        txt += "press spacebar to turn off sending.";
        ofDrawBitmapString(txt, 20, 296);

        sendOsc();
    }
    else {
        ofBackground(255, 0, 0);

        ofSetColor(0, 100);
        ofDrawRectangle(10, 280, 520, 66);
        ofSetColor(255);
        string txt = "press spacebar to turn on sending.";
        ofDrawBitmapString(txt, 20, 296);
    }
    
    ofSetColor(255);
    cam.draw(110, 10);

}

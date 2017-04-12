#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(640, 480);

    // default settings
    normalize = true;
    sendScreen = true;
    oscDestination = OSC_DESTINATION_DEFAULT;
    oscAddress = OSC_ADDRESS_ROOT_DEFAULT;
    oscPort = OSC_PORT_DEFAULT;

    // load settings from file
    ofXml xml;
    xml.load("settings_kinect.xml");
    xml.setTo("KinectOSC");
    oscDestination = xml.getValue("ip");
    oscPort = ofToInt(xml.getValue("port"));
    oscAddress = xml.getValue("address");

    // setup kinect
    kinect.setup();
    kinect.addDepthGenerator();
    kinect.addUserGenerator();
    kinect.setMaxNumUsers(1);
    kinect.start();
    
    // addresses + setup osc
    osc.setup(oscDestination, oscPort);
    
    // setup gui
    gui.setName("KinectOSC");
    gui.addToggle("2d-projective", &sendScreen);
    gui.addToggle("normalize", &normalize);
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if (!kinect.isNewFrame())   return;
    
    // trick to reset user generator if no tracked users found
    if (kinect.getNumTrackedUsers() > 0) {
        hadUsers = true;
    }
    else if (kinect.getNumTrackedUsers() == 0 && hadUsers){
        hadUsers = false;
        kinect.setPaused(true);
        kinect.removeUserGenerator();
        kinect.addUserGenerator();
        kinect.setPaused(false);
    }
    
    if (kinect.getNumTrackedUsers() == 0)   return;

    
    ofxOscMessage msg;
    msg.setAddress(oscAddress);

    ofxOpenNIUser & user = kinect.getTrackedUser(0);
    ofPoint torso = user.getJoint((Joint) 0).getWorldPosition();
    
    // send screen coordinates
    if (sendScreen) {
        torso = user.getJoint((Joint) 0).getProjectivePosition();
        for (int j=0; j<user.getNumJoints(); j++) {
            ofxOpenNIJoint & joint = user.getJoint((Joint) j);
            ofPoint p = joint.getProjectivePosition();
            if (normalize) {
                msg.addFloatArg(p.x-torso.x);
                msg.addFloatArg(p.y-torso.y);
            }
            else {
                msg.addFloatArg(p.x);
                msg.addFloatArg(p.y);
            }
        }
    }
    
    // or else send real world coordinates
    else {
        for (int j=0; j<user.getNumJoints(); j++) {
            ofxOpenNIJoint & joint = user.getJoint((Joint) j);
            ofPoint w = joint.getWorldPosition();
            if (normalize) {
                msg.addFloatArg(w.x-torso.x);
                msg.addFloatArg(w.y-torso.y);
                msg.addFloatArg(w.z-torso.z);
            }
            else {
                msg.addFloatArg(w.x);
                msg.addFloatArg(w.y);
                msg.addFloatArg(w.z);
            }
        }
    }
    
    osc.sendMessage(msg);
    oscMessageString = "Sending OSC to "+oscDestination+", port "+ofToString(oscPort)+": address "+oscAddress+" -> "+ofToString(msg.getNumArgs())+" values";
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    // draw skeleton
    ofSetColor(255);
    if (kinect.getNumTrackedUsers() > 0) {
        kinect.drawSkeleton(0);
    }
    ofDrawBitmapStringHighlight(oscMessageString, 15, ofGetHeight() - 4);
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

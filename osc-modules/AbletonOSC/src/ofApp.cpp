#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    live.setup("localhost", this, &ofApp::setupGui);
}

//--------------------------------------------------------------
void ofApp::update(){
    live.update();
    gui.update();
}

void ofApp::setupGui() {
    addressRoot = DEFAULT_INPUT_ADDRESS;
    portIn = DEFAULT_PORT_IN;
    
    ofXml xml;
    xml.load("settings_ableton.xml");
    xml.setTo("AbletonOSC");
    portIn = ofToInt(xml.getValue("port"));
    addressRoot = xml.getValue("address");
    
    gui.setup(&live, addressRoot, portIn);
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (!live.isLoaded()) {
        ofDrawBitmapString("loading ableton set...", 20, 20);
        return;
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
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

#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    // this should point to the json file containing your audio files and tsne coordinates
    // follow instructions in ml4a.github.io/guides/AudioTSNEViewer/ 
    tsnePath = ofToDataPath("audiotsne.json");
    
    gui.setup();
    gui.setName("Audio t-SNE");
    gui.add(maxDuration.set("maxDuration", 1.0, 0.1, 2.0));
    gui.add(mouseRadius.set("mouseRadius", 250, 100, 500));
    gui.add(pauseLength.set("pauseLength", 2.0, 0.2, 5.0));
    
    ofxJSONElement result;
    parsingSuccessful = result.open(tsnePath);
    for (int i=0; i<result.size(); i++) {
        string path = result[i]["path"].asString();
        float x = result[i]["point"][0].asFloat();
        float y = result[i]["point"][1].asFloat();
        AudioClip newSound;
        newSound.sound.load(path);
        newSound.point.set(x, y);
        newSound.t = 0;
        sounds.push_back(newSound);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!parsingSuccessful) {
        return;
    }
    
    for (int i=0; i<sounds.size(); i++) {
        if (sounds[i].sound.isPlaying() && sounds[i].sound.getPositionMS() > maxDuration*1000) {
            sounds[i].sound.stop();
        }
    }
    ofSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(100), ofColor(20));
    if (!parsingSuccessful) {
        ofDrawBitmapString("Could not find file "+tsnePath+"\nSee the instructions for how to create one.", 50, 50);
        return;
    }
    for (int i=0; i<sounds.size(); i++) {
        if (sounds[i].sound.isPlaying()) {
            ofSetColor(0, 255, 0, 180);
        }
        else {
            ofSetColor(255, 180);
        }
        ofDrawCircle(ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y, 4);

    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    for (int i=0; i<sounds.size(); i++) {
        float distanceToMouse = ofDistSquared(x, y, ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y);
        if (distanceToMouse < mouseRadius && !sounds[i].sound.isPlaying() && (ofGetElapsedTimef() - sounds[i].t > pauseLength)) {
            sounds[i].t = ofGetElapsedTimef();
            sounds[i].sound.play();
        }
    }
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

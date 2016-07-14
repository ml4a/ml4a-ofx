#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    save.addListener(this, &ofApp::saveScreenshot);
    
    gui.setup();
    gui.setName("Image t-SNE");
    gui.add(scale.set("scale", 4.0, 0.0, 10.0));
    gui.add(imageSize.set("imageSize", 1.0, 0.0, 2.0));
    gui.add(save.setup("save screenshot"));
    
    string file = "points.json";
    ofxJSONElement result;
    bool parsingSuccessful = result.open(file);
    for (int i=0; i<result.size(); i++) {
        string path = result[i]["path"].asString();
        float x = result[i]["point"][0].asFloat();
        float y = result[i]["point"][1].asFloat();
        ImageThumb thumb;
        thumb.image.load(path);
        thumb.point.set(x, y);
        thumbs.push_back(thumb);
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(0), ofColor(100));
    ofPushMatrix();
    ofTranslate(-ofGetMouseX() * (scale - 1.0), -ofGetMouseY() * (scale - 1.0));
    for (int i=0; i<thumbs.size(); i++) {
        float x = ofMap(thumbs[i].point.x, 0, 1, 0, scale * ofGetWidth());
        float y = ofMap(thumbs[i].point.y, 0, 1, 0, scale * ofGetHeight());
        thumbs[i].image.draw(x, y, imageSize * thumbs[i].image.getWidth(), imageSize * thumbs[i].image.getHeight());
    }
    ofPopMatrix();
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::saveScreenshot(){
    
    ofFbo fbo;
    fbo.allocate(scale * ofGetWidth() + 50, scale * ofGetHeight() + 50);
    fbo.begin();
    ofClear(0, 0);
    ofBackground(0);
    for (int i=0; i<thumbs.size(); i++) {
        float x = ofMap(thumbs[i].point.x, 0, 1, 0, scale * ofGetWidth());
        float y = ofMap(thumbs[i].point.y, 0, 1, 0, scale * ofGetHeight());
        thumbs[i].image.draw(x, y, imageSize * thumbs[i].image.getWidth(), imageSize * thumbs[i].image.getHeight());
    }
    fbo.end();
    ofPixels pix;
    ofImage img;
    fbo.readToPixels(pix);
    img.setFromPixels(pix);
    img.save("out.png");
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

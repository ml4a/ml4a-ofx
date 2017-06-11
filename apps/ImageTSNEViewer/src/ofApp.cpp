#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // this should point to the json file containing your image files and tsne coordinates
    tsnePath = ofToDataPath("imagetsne.json");
    maxDim = 200;

    // listen for scroll events, and save screenshot button press
    ofAddListener(ofEvents().mouseScrolled, this, &ofApp::mouseScrolled);
    save.addListener(this, &ofApp::saveScreenshot);
    
    gui.setup();
    gui.setName("Image t-SNE");
    gui.add(scale.set("scale", 4.0, 0.0, 10.0));
    gui.add(imageSize.set("imageSize", 1.0, 0.0, 2.0));
    gui.add(save.setup("save screenshot"));
    
    ofxJSONElement result;
    parsingSuccessful = result.open(tsnePath);
    for (int i=0; i<result.size(); i++) {
        string path = result[i]["path"].asString();
        float x = result[i]["point"][0].asFloat();
        float y = result[i]["point"][1].asFloat();
        ImageThumb thumb;
        thumb.point.set(x, y);
        thumb.image.load(path);
        if (thumb.image.getWidth() > thumb.image.getHeight()) {
            thumb.image.resize(maxDim, maxDim * thumb.image.getHeight() / thumb.image.getWidth());
        } else {
            thumb.image.resize(maxDim * thumb.image.getWidth() / thumb.image.getHeight(), maxDim);
        }
        thumbs.push_back(thumb);
    }
    
    position.set(-0.5 * ofGetWidth(), -0.5 * ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(0), ofColor(100));
    if (!parsingSuccessful) {
        ofDrawBitmapString("Could not find file "+tsnePath+"\nSee the instructions for how to create one.", 50, 50);
        return;
    }
    
    ofPushMatrix();
    ofTranslate(position.x * (scale - 1.0), position.y * (scale - 1.0));
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
    fbo.allocate(scale * ofGetWidth() + 100, scale * ofGetHeight() + 100);
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
    position.x = position.x - (ofGetMouseX()-ofGetPreviousMouseX());
    position.y = position.y - (ofGetMouseY()-ofGetPreviousMouseY());
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(ofMouseEventArgs &evt) {
    scale.set(ofClamp(scale + 0.01 * (evt.scrollY), 0.0, 10.0));
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

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1280, 800);
    ccv.setup(ofToDataPath("../../../../data/image-net-2012.sqlite3"));
//    ccv.setup(ofToDataPath("image-net-2012.sqlite3"));

    if (ccv.isLoaded()) {
        grab.initGrabber(640, 480);
        ccv.setEncode(true);
        ccv.start();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!ccv.isLoaded()) {
        return;
    }
    
    grab.update();
    if (inputMode == 0 && grab.isFrameNew() && ccv.isReady()) {
        ccv.update(grab, ccv.numLayers());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);

    if (!ccv.isLoaded()) {
        ofPushStyle();
        ofSetColor(ofColor::red);
        ofDrawBitmapString("Can't find network file! Check to make sure it's in your data folder.", 20, 20);
        ofPopStyle();
        return;
    }
    
    if (inputMode == 0) {   // webcam
        grab.draw(0, 0, 320, 240);
    }
    else {  // image
        pic.draw(0, 0, 320, pic.getHeight() * 320 / pic.getWidth());
    }
    
    // draw menu
    ofNoFill();
    inputMode == 0 ? ofSetColor(0, 255, 0) : ofSetColor(0);
    ofDrawRectangle(10, 245, 200, 20);
    ofDrawBitmapString("Webcam", 15, 262);
    inputMode == 1 ? ofSetColor(0, 255, 0) : ofSetColor(0);
    ofDrawRectangle(10, 268, 200, 20);
    ofDrawBitmapString("Load picture", 15, 285);
    viewMode == 0 ? ofSetColor(0, 255, 0) : ofSetColor(0);
    ofDrawRectangle(10, 310, 200, 20);
    ofDrawBitmapString("View Feature maps", 15, 327);
    viewMode == 1 ? ofSetColor(0, 255, 0) : ofSetColor(0);
    ofDrawRectangle(10, 333, 200, 20);
    ofDrawBitmapString("View Filters", 15, 350);
    vector<string> layerNames = ccv.getLayerNames();
    for (int i=0; i<layerNames.size(); i++) {
        i == layer ? ofSetColor(0, 255, 0) : ofSetColor(0);
        ofDrawRectangle(10, 375+23*i, 200, 20);
        ofDrawBitmapString(layerNames[i], 15, 392+23*i);
    }
    ofSetColor(0);
    ofDrawBitmapString("drag feature maps to scroll", 5, ofGetHeight()-5);
    
    
    ofTranslate(325, 5);
    
    // draw feature maps
    if (viewMode == 0) {
        maps = ccv.getFeatureMaps(layer);
        ofImage img;
        img.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        for (int m=0; m<maps.size(); m++) {
            ofPushMatrix();
            ofSetColor(255);
            if (maps.size() > 1) {
                maps[m].getImage(img);
                ofTranslate((m%8)*105, floor(m/8)*105 - scroll);
                img.draw(0, 0, 100, 100);
            }
            else {
                float mul = 255.0 / maps[m].max;
                int cols = 32;
                int rows = 128;
                ofPixels pix;
                pix.allocate(rows, cols, OF_PIXELS_GRAY);
                for (int i=0; i<rows*cols; i++) {
                    pix[i] = mul * maps[m].acts[i];
                }
                img.setFromPixels(pix);
                img.draw(0, 0, ofGetWidth() - 330, 100);
            }
            ofPopMatrix();
        }
        
        // draw highlighted
        if (highlighted != -1 && highlighted < maps.size() && maps.size() > 1) {
            ofxCcv::FeatureMap map = maps[highlighted];
            map.getImage(img);
            ofPushMatrix();
            ofTranslate(ofClamp((highlighted%8)*105 - 50, 0, ofGetWidth()-220), ofClamp(floor(highlighted/8)*105 - scroll - 50, 20, ofGetHeight()-220));
            ofSetColor(0, 255, 0);
            ofFill();
            ofDrawRectangle(-10, -25, 220, 235);
            ofSetColor(0);
            ofDrawBitmapString(ccv.getLayerNames()[layer]+" - "+ofToString(highlighted), 0, -2);
            ofSetColor(255);
            img.draw(0, 0, 200, 200);
            ofPopMatrix();
        }
        
        if (layer == ccv.getLayerNames().size()-1) {
            results = ccv.getResults();
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(10, 120);
            for(int i = 0; i < results.size(); i++) {
                ofSetColor(ofColor::black);
                ofFill();
                ofDrawRectangle(0, 0, 100, 20);
                ofSetColor(ofColor::red);
                ofDrawRectangle(1, 2, (100-2) * results[i].confidence, 15);
                ofSetColor(ofColor::blue);
                ofDrawBitmapStringHighlight(results[i].imageNetName, 106, 15);
                ofTranslate(0, 25);
            }
            ofPopMatrix();
            ofPopStyle();
        }
    }
    
    // draw weights
    else if (viewMode == 1) {
        ofSetColor(0);
        ofDrawBitmapString("just first layer filters for now", 2, 15);
        ofSetColor(255);
        vector<ofImage> weightImgs = ccv.getWeights();
        for (int i=0; i<weightImgs.size(); i++) {
            ofPushMatrix();
            ofTranslate((i%12)*65, 24 + floor(i/12)*65);
            weightImgs[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            weightImgs[i].draw(0, 0, 60, 60);
            ofPopMatrix();
        }
    }
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    for (int i=0; i<8; i++) {
        for (int j=0; j<32; j++) {
            if (ofRectangle(325 + 105*i, 5 + 105*j - scroll, 100, 100).inside(x, y)) {
                highlighted = i + 8 * j;
                return;
            }
        }
    }
    highlighted = -1;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    scroll = ofClamp(scroll - (ofGetMouseY() - ofGetPreviousMouseY()), 0, 32 * 110);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if      (ofRectangle(10, 310, 200, 20).inside(x, y)) viewMode = 0;
    else if (ofRectangle(10, 333, 200, 20).inside(x, y)) viewMode = 1;
    else if (ofRectangle(10, 245, 200, 20).inside(x, y)) inputMode = 0;
    else if (ofRectangle(10, 268, 200, 20).inside(x, y)) {
        ofFileDialogResult result = ofSystemLoadDialog("Select an image");
        if (result.bSuccess) {
            inputMode = 1;
            pic.load(result.filePath);
            ccv.update(pic, ccv.numLayers());
        }
    }
    else {
        for (int i=0; i<ccv.getLayerNames().size(); i++) {
            if (ofRectangle(10, 375+23*i, 200, 20).inside(x, y)) {
                layer = i;
                if (layer == ccv.getLayerNames().size()-1) {
                    ccv.setClassify(true, 20);
                }
                else {
                    ccv.setClassify(false);
                }
                if (inputMode == 1) {
                    ccv.update(pic, ccv.numLayers());
                }
                return;
            }
        }
    }
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

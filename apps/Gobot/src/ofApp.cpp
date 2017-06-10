#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    debugDraw = true;
    debug = false;
    retina = true;  // if using macbook retina
    confidenceMin = 90;
    
    ofSetWindowShape(1280, 800);
    
    string cfgfile = ofToDataPath("darknet/go.test.cfg");
    string weightfile = ofToDataPath("darknet/go.weights");
    //string cfgfile = ofToDataPath("../../../darknet/go.test.cfg");
    //string weightfile = ofToDataPath("../../../darknet/go.weights");
    darknet.setup(cfgfile, weightfile);
    darknet.setMouseActive(true);
    if (debugDraw) {
        darknet.setDrawPosition(700, 60, 550, 550);
    } else {
        darknet.setDrawPosition(30, 60, 550, 550);
    }
    
    screen.setup(ofGetWidth(), ofGetHeight(), retina);
    screen.loadCorners(ofToDataPath("corners_gobot.xml"));
    toggleScreenDebug(false);
    
    objects.clear();
    addObject(ofToDataPath("blacksquare.png"));
    addObject(ofToDataPath("blacksquare2.png"));
    addObject(ofToDataPath("whitesquare.png"));
    addObject(ofToDataPath("whitesquare2.png"));
}

//--------------------------------------------------------------
void ofApp::addObject(string path) {
    ofImage img;
    ofxCvColorImage imgC;
    ofxCvGrayscaleImage imgG;
    img.load(path);
    img.setImageType(OF_IMAGE_COLOR);
    imgC.setFromPixels(img.getPixels());
    imgG = imgC;
    objects.push_back(MatchObject());
    objects.back().allocate(imgG.getWidth(), imgG.getHeight());
    objects.back().setFromPixels(imgG.getPixels());
    
}

//--------------------------------------------------------------
void ofApp::toggleScreenDebug(bool b) {
    debug = b;
    screen.setDebug(debug);
    if (!debug) {
        int width = screen.getGrabber().getTextureReference().getWidth();
        int height = screen.getGrabber().getTextureReference().getHeight();
        pixels.allocate(width, height, 4);
        colorImage.allocate(width, height);
        grayImage.allocate(width, height);
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    screen.saveCorners(ofToDataPath("corners_gobot.xml"));
}

//--------------------------------------------------------------
void ofApp::update() {
    screen.update();
    screen.getGrabber().getTextureReference().readToPixels(pixels);
    pixels.setImageType(OF_IMAGE_COLOR);
    colorImage.setFromPixels(pixels);
    grayImage = colorImage;
    if (ofGetFrameNum() % 5 == 0) {
        analyzeGoImage();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeGoImage() {
    matches.clear();
    for (int i=0; i<objects.size(); i++) {
        int nFound = matcher.findMatches(grayImage, objects[i], confidenceMin, true, 180);
        for (int j=0; j<matcher.matches.size(); j++) {
            matches.push_back(matcher.matches[j]);
            matches.back().objectID = i;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    if (screen.isDebug()) {
        screen.drawDebug();
    }
    else {
        if (debugDraw) {
            ofPushMatrix();
            ofScale(0.5, 0.5);
            ofSetColor(255);
            grayImage.draw(0, 0);
            for (int i=0; i<matches.size(); i++) {
                matches[i].draw();
            }
            ofPopMatrix();
            if (matches.size() != nMatches) {
                nMatches = matches.size();
                runGoRecommender();
            }
        }
        darknet.draw();
    }
}

//--------------------------------------------------------------
void ofApp::runGoRecommender() {
    float* board = darknet.getBoard();
    for (int i=0; i<19*19;i++) {
        board[i] = 0;
    }
    float cellWidth = grayImage.getWidth() / 19.0;
    float cellHeight = grayImage.getHeight() / 19.0;
    for (int i=0; i<matches.size(); i++) {
        ofPoint center = matches[i].getCenter();
        int id = matches[i].objectID;
        int col = int(center.x / cellWidth);
        int row = int(center.y / cellHeight);
        int color = (id == 0 || id == 1) ? 1 : -1;
        int idx = col + 19 * row;
        board[idx] = color;
    }
    darknet.getRecommendations();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='1') {
        toggleScreenDebug(!debug);
    }
    if(key == OF_KEY_UP) {
        if(confidenceMin<100) confidenceMin ++;
    }
    if(key == OF_KEY_DOWN) {
        if(confidenceMin>0) confidenceMin --;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
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


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    screen.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    screen.mouseDragged(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    screen.mousePressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    screen.mouseReleased(x, y);
}


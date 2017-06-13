#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    retina = true;  // if using macbook retina
    
    debugDrawScale = 0.15;
    confidenceMin = 85;
    boardWidth = boardHeight = 2 * 680;
    debug = false;

    ofSetWindowShape(1280, 800);
    
    //string cfgfile = ofToDataPath("darknet/go.test.cfg");
    //string weightfile = ofToDataPath("darknet/go.weights");
    string cfgfile = ofToDataPath("../../../../data/darknet/go.test.cfg");
    string weightfile = ofToDataPath("../../../../data/darknet/go.weights");
    
    darknet.setup(cfgfile, weightfile);
    darknet.setNumRecommendations(10);
    darknet.setMouseActive(true);
    darknet.setDrawPosition(50, 250, 500, 500);
    
    screen.setup(ofGetWidth(), ofGetHeight(), retina);
    screen.loadCorners(ofToDataPath("corners_gobot.xml"));
    toggleScreenDebug(false);
    
    objects.clear();
    addObject(ofToDataPath("blacksquare.png"));
    addObject(ofToDataPath("blacksquare2.png"));
    addObject(ofToDataPath("whitesquare.png"));
    addObject(ofToDataPath("whitesquare2.png"));
    
    colorImage.allocate(boardWidth, boardHeight);
    grayImage.allocate(boardWidth, boardHeight);
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
    screen.setDebug(b);
    if (!screen.isDebug()) {
        int width = screen.getGrabber().getTextureReference().getWidth();
        int height = screen.getGrabber().getTextureReference().getHeight();
        pixels.allocate(width, height, 4);
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    screen.saveCorners(ofToDataPath("corners_gobot.xml"));
}

//--------------------------------------------------------------
void ofApp::update() {
    screen.update();
    if (ofGetFrameNum() % 5 == 0) {
        screen.getGrabber().getTextureReference().readToPixels(pixels);
        pixels.setImageType(OF_IMAGE_COLOR);
        pixels.resize(boardWidth, boardHeight);
        colorImage.setFromPixels(pixels);
        grayImage = colorImage;
        cvDebug.setFromPixels(grayImage.getPixels());
        cvDebug.resize(grayImage.getWidth() * debugDrawScale,
                       grayImage.getHeight() * debugDrawScale);
        analyzeGoImage();
        if (matches.size() != nMatches) {
            nMatches = matches.size();
            runGoRecommender();
        }
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
void ofApp::drawDebug(int x, int y, float scale) {
    ofPushMatrix();
    ofTranslate(x, y);
    ofSetColor(255);
    if (cvDebug.isAllocated()) {
        cvDebug.draw(0, 0);
    }
    ofScale(scale, scale);
    for (int i=0; i<matches.size(); i++) {
        matches[i].draw();
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    if (debug) {
        screen.drawDebug();
    }
    else { // presentation view
        drawDebug(20, 16, debugDrawScale);
        ofDrawBitmapStringHighlight("CV debug", 22, 15);
        darknet.drawRecommendations(300, 0);
        darknet.drawBoard();
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
    if (key=='d'){
        debug = !debug;
        toggleScreenDebug(debug);
        darknet.setMouseActive(!debug);
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


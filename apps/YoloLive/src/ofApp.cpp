#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowShape(1280, 800);
    
    string cfgfile = ofToDataPath("../../../../data/darknet/yolo9000.cfg");
    string weightfile = ofToDataPath("../../../../data/darknet/yolo9000.weights");
    string nameslist = ofToDataPath("../../../../data/darknet/9k.names");
    
//    string cfgfile = ofToDataPath("darknet/yolo9000.cfg");
//    string weightfile = ofToDataPath("darknet/yolo9000.weights");
//    string nameslist = ofToDataPath("darknet/9k.names");

    darknet.init( cfgfile, weightfile, nameslist );
   
    tWebcam.addListener(this, &ofApp::useWebcam);
    tVideo.addListener(this, &ofApp::useVideo);
    tScreen.addListener(this, &ofApp::useScreen);
    tRetina.addListener(this, &ofApp::toggleRetina);
    tScreenDebug.addListener(this, &ofApp::toggleScreenDebug);

    gui.setup();
    gui.setName("YoloLive");
    gui.add(thresh.set("threshold", 0.24, 0.0, 1.0));
    gui.add(tWebcam.setup("webcam", false));
    gui.add(tVideo.setup("video", false));
    gui.add(tScreen.setup("screengrab", false));
    gui.add(tRetina.set("macbook retina?", false));
    gui.add(tScreenDebug.setup(" -> set window", false));
    gui.setPosition(ofGetWidth()-200, 0);
    
    // on startup
    tWebcam = true;
    useWebcam((bool&) tWebcam);
}

void ofApp::update()
{
    if (tWebcam) {
        cam.update();
        detections = darknet.yolo(cam.getPixels(), thresh);
    }
    else if (tVideo) {
        movie.update();
        detections = darknet.yolo(movie.getPixels(), thresh);
    }
    else if (tScreen) {
        screen.update();
        screen.getGrabber().getTextureReference().readToPixels(pixels);
        if (!tScreenDebug) {
            detections = darknet.yolo(pixels, thresh);
        }
    }
}

void ofApp::draw()
{
    // draw input source
    if (tWebcam) {
        cam.draw(0, 0);
    }
    else if (tVideo) {
        movie.draw(0, 0);
    }
    else if (tScreen) {
        if (screen.isDebug()) {
            screen.drawDebug();
        } else {
            screen.draw(0, 0);
        }
    }

    // draw detected objects
    ofPushStyle();
    for(detected_object d : detections ){
        ofSetColor( d.color );
        glLineWidth( ofMap( d.probability, 0, 1, 0, 8 ) );
        ofNoFill();
        ofRectangle rect = d.rect;
        if (tScreen && tRetina) {    // adjust for screengrab on retina returning pixels at twice the scale
            rect.set(0.5 * rect.x, 0.5 * rect.y, 0.5 * rect.width, 0.5 * rect.height);
        }
        ofDrawRectangle( rect );
        ofDrawBitmapStringHighlight( d.label + ": " /*+ ofToString(d.probability)*/, rect.x, rect.y+20 );
    }
    ofPopStyle();

    // draw gui
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::useWebcam(bool & b) {
    if (!b) return;
    tScreen = false;
    tVideo = false;
    tScreenDebug = false;
    cam.initGrabber(640, 480);
    movie.stop();
    movie.close();
}

//--------------------------------------------------------------
void ofApp::useVideo(bool & b) {
    if (!b) return;
    ofFileDialogResult result = ofSystemLoadDialog();
    if (result.bSuccess) {
        tWebcam = false;
        tScreen = false;
        tScreenDebug = false;
        cam.close();
        movie.load(result.filePath);
        movie.play();
    }
}

//--------------------------------------------------------------
void ofApp::useScreen(bool & b) {
    if (!b) return;
    tWebcam = false;
    tVideo = false;
    cam.close();
    movie.stop();
    movie.close();
    screen.setup(ofGetWidth(), ofGetHeight(), tRetina);
}

//--------------------------------------------------------------
void ofApp::toggleScreenDebug(bool & b) {
    if(b && !tScreen) {
        tScreen = true;
        useScreen(b);
    }
    screen.setDebug(b);
    if (!b) {
        pixels.allocate(screen.getGrabber().getTextureReference().getWidth(),
                        screen.getGrabber().getTextureReference().getHeight(), 4);
        
    }
}

//--------------------------------------------------------------
void ofApp::toggleRetina(bool & b) {
    if(tScreen) {
        tScreen = true;
        screen.setup(ofGetWidth(), ofGetHeight(), tRetina);
        pixels.allocate(screen.getGrabber().getTextureReference().getWidth(),
                        screen.getGrabber().getTextureReference().getHeight(), 4);
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

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


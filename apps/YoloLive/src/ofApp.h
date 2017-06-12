#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxDarknet.h"
#include "ofxScreenGrabCropped.h"

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    
    void useWebcam(bool & b);
    void useVideo(bool & b);
    void useScreen(bool & b);
    void toggleScreenDebug(bool & b);
    void toggleRetina(bool & b);

    void keyPressed(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    ofxDarknet darknet;
    vector<detected_object> detections;
    ofPixels pixels;
    
    ofVideoPlayer movie;
    ofVideoGrabber cam;
    ofxScreenGrabCropped screen;
    ofParameter<bool> tRetina;
    ofParameter<float> thresh;
    
    ofxPanel gui;
    ofxToggle tWebcam, tVideo, tScreen, tScreenDebug;
};

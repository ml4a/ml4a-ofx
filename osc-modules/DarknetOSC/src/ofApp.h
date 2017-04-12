#pragma once

#include "ofMain.h"

//#include "ofxGui.h"
//#include "ofxOsc.h"
//#include "ofxDarknet.h"

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    
//    ofxDarknet darknet;
    ofVideoGrabber video;
//    std::vector< classification > classifications;
    
    
};


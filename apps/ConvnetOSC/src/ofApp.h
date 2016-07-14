#pragma once

#include "ofxCcv.h"
#include "ofxOsc.h"


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void sendOsc();
    void keyPressed(int key);
    
    ofxOscSender osc;
    ofxOscMessage msg;
    
    ofVideoGrabber cam;
    
    ofxCcv ccv;
    ofVideoPlayer video;
    
    vector<float> classifierEncoding;
    vector<float> featureEncoding;
    
    string oscHost, oscAddress;
    int oscPort;
    
    bool sending;
};
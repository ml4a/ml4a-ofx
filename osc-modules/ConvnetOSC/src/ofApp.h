#pragma once

#include "ofxCcv.h"
#include "ofxOsc.h"
#include "ofxGui.h"

#define OSC_DESTINATION_DEFAULT "localhost"
#define OSC_ADDRESS_ROOT_DEFAULT "/wek/inputs"
#define OSC_PORT_DEFAULT 6448
#define DEVICE_ID_DEFAULT 0

//#define RELEASE

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void sendOsc();
    void keyPressed(int key);
    
    void setFc2(bool & b);
    void setClassifier(bool & b);
    
    ofxOscSender osc;
    ofxOscMessage msg;
    
    ofVideoGrabber cam;
    
    ofxCcv ccv;
    ofVideoPlayer video;
    
    vector<float> classifierEncoding;
    vector<float> featureEncoding;
    
    string oscDestination, oscAddressRoot;
    int oscPort;
    int deviceId;
    
    ofParameter<bool> sending;
    
    ofxPanel gui;
    ofxToggle sendClassifications;
};

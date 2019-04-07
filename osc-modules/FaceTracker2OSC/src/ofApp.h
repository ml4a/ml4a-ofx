#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxGui.h"

#define OSC_DESTINATION_DEFAULT "localhost"
#define OSC_ADDRESS_ROOT_DEFAULT "/wek/inputs"
#define OSC_PORT_DEFAULT 6448

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    ofxFaceTracker2 tracker;
    ofVideoGrabber grabber;
    
    ofxOscSender osc;
    string oscDestination;
    string oscAddress;
    int oscPort;
    string oscMessageString;
    
    ofParameter<bool> normalize;
    
    ofxPanel gui;
};

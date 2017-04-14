#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxDarknet.h"

#define OSC_DESTINATION_DEFAULT "localhost"
#define OSC_ADDRESS_ROOT_DEFAULT "/wek/inputs"
#define OSC_PORT_DEFAULT 6448

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    
    void sendOsc();
    
    void keyPressed(int key);
    
    ofxDarknet darknet;
    ofVideoGrabber cam;
    std::vector< classification > classifications;
    
    string oscDestination, oscAddressRoot;
    int oscPort;
    
    ofParameter<bool> sending;

    ofxOscSender osc;
    ofxOscMessage msg;

    
    ofxPanel gui;
    ofxToggle sendClassifications;

};


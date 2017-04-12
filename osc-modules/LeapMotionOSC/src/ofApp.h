#pragma once

#include "ofMain.h"
#include "ofxControl.h"
#include "ofxOsc.h"
#include "LeapMotion.h"


#define DEFAULT_LEAP_OSC_ADDRESS "/wek/inputs"
#define DEFAULT_LEAP_OSC_IP "localhost"
#define DEFAULT_LEAP_OSC_PORT 6448

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    LeapMotion leap;
    ofxOscSender osc;
    ofxControlWidget gui;
    string oscAddress;
    string oscOut;
    int portOut;
    bool sendLeft, sendRight;
    int numArgs;
};

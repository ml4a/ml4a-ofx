#pragma once

#include "ofMain.h"
#include "ofxAbletonLive.h"
#include "AbletonGui.h"

#define DEFAULT_INPUT_ADDRESS "/wek/outputs"
#define DEFAULT_PORT_IN 12000


class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void setupGui();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxAbletonLive live;
    AbletonGui gui;
    
    string addressRoot;
    int portIn;
};


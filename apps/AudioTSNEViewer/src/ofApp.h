#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"

struct AudioClip {
public:
    ofSoundPlayer sound;
    ofPoint point;
    float t;
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void eLoad();
    void load(string file);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    vector<AudioClip> sounds;
    
    ofParameter<float> maxDuration;
    ofParameter<float> mouseRadius;
    ofParameter<float> pauseLength;
    ofxButton bLoad;
    ofxPanel gui;
    
    string tsnePath;
    bool parsingSuccessful;
};

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxTemplateMatching.h"
#include "ofxScreenGrabCropped.h"
#include "ofxDarknet.h"


class ofApp : public ofBaseApp{
    
public:
    
    void setup();
    void update();
    void draw();
    
    void exit();
    
    void addObject(string path);
    void toggleScreenDebug(bool b);
    void analyzeGoImage();
    void runGoRecommender();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxDarknetGo darknet;
    
    ofxTemplateMatching matcher;
    vector <MatchObject> objects;
    vector <TemplateMatch> matches;
    int confidenceMin;
    int nMatches;
    
    ofxCvColorImage colorImage;
    ofxCvGrayscaleImage grayImage;
    ofxScreenGrabCropped screen;
    ofPixels pixels;
    bool retina;
    bool debug;
    bool debugDraw;
};

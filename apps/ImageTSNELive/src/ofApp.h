#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxTSNE.h"
#include "ofxAssignment.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void analyzeImages();
    void runTSNEAndGridding();
    
    
    void analyzeNew() {
        ofFileDialogResult result = ofSystemLoadDialog("Select folder of images to analyze", true);
        if (result.bSuccess) {
            imageDir = result.getPath();
        }
    }
    
    
    void saveToImage();
    void scan_dir_imgs(ofDirectory dir);

    void mouseDragged(int x, int y, int button);
    void mouseScrolled(ofMouseEventArgs &evt);
    
    ofxCcv ccv;
    ofxTSNE tsne;
    ofxAssignment solver;
    
    vector<ofFile> imageFiles;
    vector<ofImage> images;
    vector<vector<float> > encodings;
    vector<vector<double> > tsneVecs;
    vector<ofVec2f> solvedGrid;
    
    string imageDir;
    int resizeWidth, resizeHeight;
    int displayW, displayH;
    float perplexity, theta;
    
    ofParameter<float> scale, zoom;
    ofParameter<int> numGridRows, numGridCols;
    
    ofxPanel gui;
    ofxButton bAnalyzeNew;
    ofParameter<bool> tViewGrid;
    ofVec2f position;
};

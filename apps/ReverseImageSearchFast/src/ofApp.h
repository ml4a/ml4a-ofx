#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCcv.h"
#include "ofxLearn.h"
#include "ofxKDTree.h"

#include "ofMain.h"


struct Image {
    vector<float> encoding;
    vector<double> projectedEncoding;
    string filename;
};


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void drawResults();
    
    void keyPressed(int key);
    
    void analyzeWebcam();
    void analyzeImage();
    void queryResults();

    void getImagePathsRecursive(ofDirectory dir);
    void extractFeaturesForDirectory(string directory);

    void runPCAonImageSet();
    void runKDTree();
    
    void save(string path);
    void load(string path);
    void saveKDTree(string path);
    void loadKDTree(string path);

    void saveDefault();
    void loadDefault();
    void extractMSCOCO();
    
    vector<Image> images;
    vector<ofFile> candidateFiles;
    
    ofxCcv ccv;
    ofxLearnPCA pca;
    ofxKDTree kdTree;
    
    ofVideoGrabber cam;
    ofImage activeImage;
    
    vector<size_t> indexes;
    vector<double> distances;
    vector<float> activeEncoding;

    ofImage queryImage;
    vector<ofImage> resultImages;
    
    ofxPanel gui;
    ofxButton bAnalyze, bExtractDir, bKDTree, bSave, bLoad;
    ofParameter<bool> tRecord;
    ofParameter<int> numResults, numPCAcomponents, maxPCASamples;
    ofParameter<float> thumbHeight, headerHeight;
};


#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCcv.h"
#include "ofxLearn.h"
#include "ofxKDTree.h"

#include "ofMain.h"


// to-do
//  - close file dialog
//  - update status/progress on screen (or thread process)


class ofApp : public ofBaseApp{
    
public:
    
    struct Image {
        vector<float> encoding;
        vector<double> projectedEncoding;
        string filename;
    };

    void setup();
    void update();
    void draw();
    void drawResults();
    
    void analyzeWebcam();
    void analyzeImage();
    void queryResults();

    void getImagePathsRecursive(ofDirectory dir);
    void extractFeaturesForDirectory(string directory);

    void runPCAonImageSet();
    void runKDTree();
    
    void save(string path);
    void load(string path);
    void saveDialog();
    void loadDialog();

    void saveKDTree(string path);
    void loadKDTree(string path);

    void extractDirectory();
    
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

    vector<ofImage> resultImages;
    
    ofxPanel guiOptions, guiView;
    ofxButton bExtractDir, bSave, bLoad, bSampleImage;
    ofParameter<bool> tWebcam;
    ofParameter<int> numResults, numPCAcomponents, maxPCASamples;
    ofParameter<float> thumbHeight, headerHeight;
};


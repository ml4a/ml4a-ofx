#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxLearn.h"
#include "ofxKDTree.h"
#include "ofxDarknet.h"

class ofApp : public ofBaseApp{

public:
    
    struct Image {
        vector<float> encoding;
        vector<double> projectedEncoding;
        string filename;
        ofRectangle crop;
        float probability;
    };
    
    void setup();
    void update();
    void draw();
    void drawResults();
    
    void analyzeWebcam();
    void analyzeVideo();
    void analyzeImage();
    void queryResults();
    
    void enableWebcam(bool & enable);
    void enableVideo(bool & enable);
    
    void getImagePathsRecursive(ofDirectory dir);
    void extractFeaturesForDirectory(string directory);
    
    void runDimReduction();
    void runKDTree();
    
    void save(string path, bool featuresOnly=false);
    void load(string path, string baseDir, bool featuresOnly=false);
    void saveDialog();
    void loadDialog();
    
    void saveKDTree(string path);
    void loadKDTree(string path);
    
    void extractDirectory();
    
    vector<Image> images;
    map<string, bool> imageMap;
    vector<ofFile> candidateFiles;
    string baseDir;
    
    ofxDarknet darknet;
    ofxLearnRandomProjection rp;
    ofxKDTree kdTree;
    
    ofVideoGrabber cam;
    ofVideoPlayer movie;
    ofImage activeImage;
    
    vector<detected_object> detections;
    vector<size_t> indexes;
    vector<double> distances;
    
    ofImage queryImage;
    vector<vector<ofImage> > resultImages;
    
    ofxPanel guiOptions, guiView;
    ofxButton bExtractDir, bSave, bLoad, bSampleImage;
    ofParameter<bool> tWebcam, tVideo;
    ofParameter<int> numResults, numComponents, maxSamples;
    ofParameter<float> thumbHeight, headerHeight;
    ofParameter<float> yoloThreshold, yoloMaxOverlap;
    
    bool toLoad, toExtract;
    string toLoadModel, toExtractDir;

};

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCcv.h"
#include "ofxLearn.h"
#include "ofxKDTree.h"
#include "ofxScreenGrabCropped.h"

#include "ofMain.h"


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
    void analyzeScreen();
    void analyzeVideo();
    void analyzeImage();
    void queryResults();
    
    void enableWebcam(bool & enable);
    void enableScreenGrab(bool & enable);
    void enableVideo(bool & enable);
    void toggleScreenGrabDebug(bool & debug);
    void toggleMBPRetina(bool & retina);

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
    
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    vector<Image> images;
    map<string, bool> imageMap;
    vector<ofFile> candidateFiles;
    string baseDir;
    
    ofxCcv ccv;
    ofxLearnRandomProjection rp;
    ofxKDTree kdTree;
    
    ofxScreenGrabCropped screen;
    ofPixels screenPixels;
    ofVideoGrabber cam;
    ofVideoPlayer movie;
    ofImage activeImage;
    
    vector<size_t> indexes;
    vector<double> distances;
    vector<float> activeEncoding;

    vector<ofImage> resultImages;
    
    ofxPanel guiOptions, guiView;
    ofxButton bExtractDir, bSave, bLoad, bSampleImage;
    ofParameter<bool> tWebcam, tVideo, tScreen, tScreenDebug;
    ofParameter<bool> tRetina;
    ofParameter<int> numResults, numComponents, maxSamples;
    ofParameter<float> thumbHeight, headerHeight;
    
    bool toLoad, toExtract;
    string toLoadModel, toExtractDir;
};



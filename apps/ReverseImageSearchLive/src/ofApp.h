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
    
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    vector<Image> images;
    vector<ofFile> candidateFiles;
    
    ofxCcv ccv;
    ofxLearnPCA pca;
    ofxKDTree kdTree;
    
    ofxScreenGrabCropped screen;
    ofPixels screenPixels;
    ofVideoGrabber cam;
    ofVideoPlayer movie;
    ofImage activeImage;
    
    vector<size_t> indexes;
    vector<double> distances;
    vector<float> activeEncoding;
    
    ofImage queryImage;
    vector<ofImage> resultImages;
    
    ofxPanel guiOptions, guiView;
    ofxButton bExtractDir, bSave, bLoad, bSampleImage;
    ofParameter<bool> tWebcam, tVideo, tScreen, tScreenDebug;
    ofParameter<int> numResults, numPCAcomponents, maxPCASamples;
    ofParameter<float> thumbHeight, headerHeight;
};


#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCcv.h"
#include "ofxLearn.h"
#include "ofxKDTree.h"
#include "ofxScreenGrabCropped.h"

#include "ofMain.h"
#include "ofThread.h"


struct Image {
    vector<float> encoding;
    vector<double> projectedEncoding;
    string filename;
};



class ReverseImageSearchThreaded : public ofThread, public ofxCcv {
public:
    ReverseImageSearchThreaded();
    
    void update(ofBaseHasPixels &pix, int layer);
    void threadedFunction();
    
    vector<string> & getResults() {return results;}
    bool isReady() {return ready;}
    vector<Image> & getImages() {return images;}
    bool hasNewResults();
    
    void runKDTree();

    void save(string path);
    void load(string path);
    void saveDialog();
    void loadDialog();
    
    void saveKDTree(string path);
    void loadKDTree(string path);

    void runPCAonImageSet();
    void getImagePathsRecursive(ofDirectory dir);
    void extractFeaturesForDirectory(string directory);
    void extractDirectory();

    ofParameter<int> numResults, numPCAcomponents, maxPCASamples;
    ofxButton bExtractDir, bSave, bLoad;

protected:
    
    ofImage img;
    
    vector<Image> images;
    vector<float> encoding;

    vector<size_t> indexes;
    vector<double> distances;
    vector<float> activeEncoding;
    
    vector<ofFile> candidateFiles;
    vector<string> results;
    ofxLearnPCA pca;
    ofxKDTree kdTree;

    bool isNew;
    bool ready;
    int layer;
    bool hasImage;
};



class ofApp : public ofBaseApp{
public:
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
    void toggleRunning(bool & running);
    
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    ReverseImageSearchThreaded ris;
    
    vector<ofImage> resultImages;
    ofxScreenGrabCropped screen;
    ofPixels screenPixels;
    ofVideoGrabber cam;
    ofVideoPlayer movie;
    ofImage activeImage;
    
    ofxPanel guiOptions, guiView;
    ofxButton bSampleImage;
    ofxToggle tRunning;
    ofParameter<bool> tWebcam, tVideo, tScreen, tScreenDebug;
    ofParameter<float> thumbHeight, headerHeight;
};


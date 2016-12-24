#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxGrt.h"
#include "ofxCcv.h"
#include "ofxOsc.h"

using namespace ofxCv;
using namespace cv;


// where to send osc messages
#define OSC_DESTINATION "localhost"
#define OSC_ADDRESS "/classification"
#define OSC_PORT 5000

// define your classes here
const vector<string> classNames =
{
    "circle",
    "star",
    "arrow"
};


struct FoundSquare {
    ofImage img;
    int label = -1;
    bool isPrediction = false;
    cv::Rect rect;
    float area;
    void draw();
};


class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void setTrainingLabel(int & label_);
    void addSamplesToTrainingSet();
    void gatherFoundSquares();
    void trainClassifier();
    void classifyCurrentSamples();
    
    void addSamplesToTrainingSetNext();
    void classifyNext();
    
    void save();
    void load();
    
    int width, height;
    
    ofVideoGrabber cam;
    ContourFinder contourFinder, contourFinder2;
    ofFbo fbo;
    ofxCvGrayscaleImage grayImage;
    ofxCvColorImage colorImage;
    
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
    
    ofxPanel gui;
    ofxToggle bRunning;
    ofxButton bAdd, bTrain, bClassify, bSave, bLoad;
    ofParameter<float> minArea, maxArea, threshold;
    ofParameter<int> nDilate;
    ofParameter<int> trainingLabel;
    
    vector<FoundSquare> foundSquares;
    
    ClassificationData trainingData;
    GestureRecognitionPipeline pipeline;
    ofxCcv ccv;
    bool isTrained, toAddSamples, toClassify;
};

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

#define RELEASE


// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/classification"
#define DEFAULT_OSC_PORT 5000
#define DEFAULT_CAM_DEVICE_ID 0

struct FoundSquare {
    ofImage img;
    string label;
    bool isPrediction = false;
    cv::Rect rect;
    float area;
    void draw();
};


class ofApp : public ofBaseApp
{
public:

    // default class names if none found in settings_doodleclassifier.xml
    string classNamesStr = "circle,star,arrow";
    vector<string> classNames = {"circle", "star", "arrow"};

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
    
    void setupCamera();
    void setupClasses();
    void setupOSC();
    void eChangeSettings();
    
    void save();
    void load();
    
    int width, height;
    
    ofVideoGrabber cam;
    ContourFinder contourFinder, contourFinder2;
    ofFbo fbo;
    ofxCvGrayscaleImage grayImage;
    ofxCvColorImage colorImage;
    
    ofxOscSender sender;
    
    ofxPanel gui;
    ofParameterGroup gSettings, gCv;
    ofxToggle bRunning;
    ofxButton bSettings, bClasses;
    ofxButton bAdd, bTrain, bClassify, bSave, bLoad;
    ofParameter<string> gOscDestination, gOscAddress, gOscPort, gClassesStr, gDeviceId;
    ofParameter<float> minArea, maxArea, threshold;
    ofParameter<int> nDilate;
    ofParameter<int> trainingLabel;
    
    vector<FoundSquare> foundSquares;
    
    ClassificationData trainingData;
    GestureRecognitionPipeline pipeline;
    ofxCcv ccv;
    bool isTrained, toAddSamples, toClassify;
};

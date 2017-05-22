#pragma once

#include "ofMain.h"
#include "ofxCcv.h"
#include "ofxGrt.h"
#include "ofxGui.h"
#include "ofxOsc.h"

// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/classification"
#define DEFAULT_OSC_PORT 8000

#define SIZE_INPUT_VECTOR 4096

class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    
    void setupRegressor();
    void addSlider();
    void trainClassifier();
    void save();
    void load();
    void clear();
    
    void sendOSC();
    
    void keyPressed(int key);
    
    ofVideoGrabber cam;
    ofxCcv ccv;
    vector<float> featureEncoding;
    
    RegressionData trainingData;
    GestureRecognitionPipeline pipeline;
    GRT::VectorFloat targetVector;

    string infoText;                            //This string will be used to draw some info messages to the main app window
    ofTrueTypeFont largeFont;
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;
    ofxGrtTimeseriesPlot predictionPlot;
    
    //OSC
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
    
    //GUI
    ofxPanel gui;
    ofxButton bTrain, bSave, bLoad, bClear, bAddSlider;
    ofxToggle tRecord, tPredict;
    
    vector<ofParameter<float> > values;
};

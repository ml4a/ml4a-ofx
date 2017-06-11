#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxCcvThreaded.h"

// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/wek/outputs"
#define DEFAULT_OSC_PORT 12000
#define SIZE_INPUT_VECTOR 4096



class GestureRecognitionPipelineThreaded : public ofThread, public GestureRecognitionPipeline {
public:
    void startTraining(RegressionData *trainingData);
    void threadedFunction();
    RegressionData *trainingData;
    bool training=false;
    bool success=false;
};


class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    
    void updateParameters();
    void setupRegressor();
    void addSlider();
    void train();
    
    void setupOSC();
    void sendOSC();

    void save();
    void load();
    void clear();
    
    void eSlider(float & v);
    void eChangeOscSettings();
    
    void exit();
    
    // input/cv
    ofVideoGrabber cam;
    ofxCcvThreaded ccv;
    vector<float> featureEncoding;
    
    // learning
    RegressionData trainingData;
    GestureRecognitionPipelineThreaded pipeline;
    GRT::VectorFloat targetVector;
    vector<ofParameter<float> > values;
    vector<float> targetValues;
    bool isTraining;

    // draing/ui
    string infoText;
    ofTrueTypeFont largeFont;
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;
    
    // parameters
    ofParameter<float> lerpAmt;
    ofParameter<int> maxEpochs, numHiddenNeurons;
    ofParameter<string> gOscDestination, gOscAddress, gOscPort;

    // gui
    ofxPanel gui, guiSliders;
    ofParameterGroup gOscSettings, gTraining;
    ofxButton bTrain, bSave, bLoad, bClear, bAddSlider, bOscSettings;
    ofxToggle tRecord, tPredict;
    
    // osc
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
};

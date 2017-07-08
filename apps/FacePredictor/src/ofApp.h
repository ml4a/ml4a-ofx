#pragma once

// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/wek/outputs"
#define DEFAULT_OSC_PORT 12000
#define DEFAULT_DEVICE_ID 0
#define SIZE_INPUT_VECTOR 136

//#define RELEASE

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "ofxGrt.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "PipelineThreaded.h"



class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    
    void updateParameters();
    RegressionThreaded * addSlider();
    CategoricalThreaded * addCategorical(int numClasses);
    void train();
    
    void setupOSC();
    void sendOSC();
    
    void save(string modelName);
    void load(string modelPath);
    void clear();
    
    void eSlider(float & v);
    void eAddSlider();
    void eAddCategorical();
    void eCategorical(int & v);
    void eChangeOscSettings();
    void eChangeCamera();
    void eSave();
    void eLoad();
    
    void exit();
    
    // learning
    vector<PipelineThreaded*> learners;
    vector<ofParameter<float> > sliders, targetSliders;
    vector<int> categoricals;
    
    // input/cv
    ofVideoGrabber cam;
    ofxFaceTracker2 tracker;
    int numInputs;
    bool isTraining;
    int numSamples;
    
    // draing/ui
    string infoText;
    ofTrueTypeFont largeFont;
    
    // parameters
    ofParameter<float> lerpAmt;
    ofParameter<int> maxEpochs, numHiddenNeurons;
    ofParameter<string> gOscDestination, gOscAddress, gOscPort, gDeviceId;
    
    // gui
    ofxPanel gui, guiSliders;
    ofParameterGroup gOscSettings, gTraining;
    ofxButton bTrain, bSave, bLoad, bClear, bAddSlider, bAddCategorical, bOscSettings, bCameraSettings;
    ofxToggle tRecord, tPredict;
    
    // osc
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
};

#pragma once

// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/wek/outputs"
#define DEFAULT_OSC_PORT 12000
#define DEFAULT_DEVICE_ID 0
#define SIZE_INPUT_VECTOR 4096

//#define RELEASE

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxCcvThreaded.h"
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
    
    void addNextSampleFromDirectory();
    void scanDirectoryRecursive(ofDirectory dir);

    void setupOSC();
    void sendOSC();
    
    void save(string modelName);
    void load(string modelPath);
    void clear();
    
    void eSlider(float & v);
    void eAddSlider();
    void eAddCategorical();
    void eAddFromDir();
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
    ofxCcvThreaded ccv;
    vector<float> featureEncoding;
    bool isTraining;
    bool toRecordFromDir;
    int numSamples;
    vector<ofFile> imageFiles;
    
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
    ofxButton bAddFromDir, bTrain, bSave, bLoad, bClear, bAddSlider, bAddCategorical, bOscSettings, bCameraSettings;
    ofxToggle tRecord, tPredict;
    
    // osc
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
};

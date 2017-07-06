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


// connect learning parameters to
// experiment with different learners,
// send osc on manual change
//----
// infoTxt messages
// set camera
// how many categories?
// dont .predict if not trained


class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    
    void updateParameters();
    void addSlider();
    void addCategorical();
    void train();
    
    void setupOSC();
    void sendOSC();
    
    void save();
    void load();
    void clear();
    
    void eSlider(float & v);
    void eCategorical(int & v);
    void eChangeOscSettings();
    
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
    
    // draing/ui
    string infoText;
    ofTrueTypeFont largeFont;
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;
    
    // parameters
    ofParameter<float> lerpAmt;
    ofParameter<int> maxEpochs, numHiddenNeurons;
    ofParameter<string> gOscDestination, gOscAddress, gOscPort, gDeviceId;
    
    // gui
    ofxPanel gui, guiSliders;
    ofParameterGroup gOscSettings, gTraining;
    ofxButton bTrain, bSave, bLoad, bClear, bAddSlider, bAddCategorical, bOscSettings;
    ofxToggle tRecord, tPredict;
    
    // osc
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
};

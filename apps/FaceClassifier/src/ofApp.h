#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxFaceTracker2.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include <stdio.h>

//State that we want to use the GRT namespace
using namespace GRT;

// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/classification"
#define DEFAULT_OSC_PORT 8000

class ofApp : public ofBaseApp{

public:
    enum ClassifierType{ ADABOOST=0, DECISION_TREE, KKN, GAUSSIAN_MIXTURE_MODEL, NAIVE_BAYES, MINDIST, RANDOM_FOREST_10, RANDOM_FOREST_100, RANDOM_FOREST_200, SOFTMAX, SVM_LINEAR, SVM_RBF, NUM_CLASSIFIERS };
    
    void setup();
    void update();
    void draw();
    
    void trainClassifier();
    void save();
    void load();
    void clear();
    void pause();
    
    void rawToogled(bool & state);
    void gestureToggled(bool & state);
    void orientationToggled(bool & state);

    void sendOSC();
    
    void keyPressed  (int key);
    
    bool setClassifier( const int type );
    
    //Create some variables for the demo
    ClassificationData trainingData;      		//This will store our training data
    GestureRecognitionPipeline pipeline;        //This is a wrapper for our classifier and any pre/post processing modules 
    bool recordTrainingData;                                //This is a flag that keeps track of when we should record training data
    bool trainingModeActive;
    bool predictionModeActive;
    bool drawInfo;

    
    UINT trainingClassLabel;                    //This will hold the current label for when we are training the classifier
    UINT predictedClassLabel;
    string infoText;                            //This string will be used to draw some info messages to the main app window
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;
    ofxGrtTimeseriesPlot predictionPlot;
    
    int classifierType;
    
    //FaceTracker2 stuff
    enum inputSelector{ GESTUREINPUTS=5, ORIENTATIONINPUTS=9, RAWINPUTS=136 };
    
    int trainingInputs;
    
    enum Gesture {
        MOUTH_WIDTH, MOUTH_HEIGHT,
        LEFT_EYEBROW_HEIGHT, RIGHT_EYEBROW_HEIGHT,
        LEFT_EYE_OPENNESS, RIGHT_EYE_OPENNESS,
        JAW_OPENNESS,
        NOSTRIL_FLARE
    };
    
    float getGesture (Gesture gesture);
    
    ofxFaceTracker2 tracker;
    ofVideoGrabber grabber;
    
    //OSC
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
    
    //GUI
    ofxPanel gui;
    ofxIntSlider sliderClassLabel;
    ofxButton  bTrain, bSave, bLoad, bClear, bRecord, bPause;
    ofxToggle tRecord, rawBool, gestureBool, orientationBool;
};

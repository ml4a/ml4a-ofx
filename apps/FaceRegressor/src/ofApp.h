#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxFaceTracker2.h"
#include "ofxGui.h"
#include <stdio.h>

using namespace GRT;

#define PRE_RECORDING_COUNTDOWN_TIME 500
#define RECORDING_TIME 2000

class ofApp : public ofBaseApp{
    
public:
    
    enum RegressifierType{ LINEAR_REGRESSION=0, LOGISTIC_REGRESSION, NEURAL_NET, NUM_REGRESSIFIERS };
    void setup();
    void update();
    void draw();
    
    void record();
    void trainClassifier();
    void save();
    void load();
    void clear();
    void pause();
    
    void rawToogled(bool & state);
    void gestureToggled(bool & state);
    void orientationToggled(bool & state);
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    bool setRegressifier( const int type );
    
    RegressionData trainingData;      		//This will store our training data
    GestureRecognitionPipeline pipeline;        //This is a wrapper for our classifier and any pre/post processing modules
    bool recordTrainingData;                                //This is a flag that keeps track of when we should record training data
    bool trainingModeActive;
    bool predictionModeActive;
    bool drawInfo;
    
        GRT::VectorFloat targetVector;             //This will hold the current label for when we are training the classifier
    
    string infoText;                            //This string will be used to draw some info messages to the main app window
    
    int regressifierType;
    ofTrueTypeFont largeFont;
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;

    Timer trainingTimer;

    string regressifierTypeToString( const int type ){
        switch( type ){
            case LINEAR_REGRESSION:
                return "LINEAR_REGRESSION";
                break;
            case LOGISTIC_REGRESSION:
                return "LOGISTIC_REGRESSION";
                break;
            case NEURAL_NET:
                return "NEURAL_NET";
                break;
        }
        return "UNKOWN_CLASSIFIER";
    }
    
    
    //FaceTracker2
    enum inputSelector{ GESTUREINPUTS=5, ORIENTATIONINPUTS=9, RAWINPUTS=136 };
    
    int trainingInputs = 5;
    
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

    
    //GUI
    ofxPanel gui;
    ofxIntSlider rectWidth;
    ofxIntSlider rectHeight;
    ofxButton  bTrain, bSave, bLoad, bClear, bRecord, bPause;
    ofxToggle rawBool, gestureBool, orientationBool;
    
};

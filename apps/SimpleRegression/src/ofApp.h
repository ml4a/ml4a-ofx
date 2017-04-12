#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxGui.h"
#include <stdio.h>

//State that we want to use the GRT namespace
using namespace GRT;

#define PRE_RECORDING_COUNTDOWN_TIME 1500
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
    
    //Create some variables for the demo
    RegressionData trainingData;      		//This will store our training data
    GestureRecognitionPipeline pipeline;        //This is a wrapper for our classifier and any pre/post processing modules
    bool recordTrainingData;                     //This is a flag that keeps track of when we should record training data
    bool trainingModeActive;
    bool predictionModeActive;
    bool drawInfo;
    
    GRT::VectorFloat targetVector;              //This will hold the current label for when we are training the classifier
    
    string infoText;                            //This string will be used to draw some info messages to the main app window
    
    int regressifierType;
    ofTrueTypeFont largeFont;
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;

    Timer trainingTimer;
    
    vector<string> regressifierNames =
    {
        "LINEAR_REGRESSION",
        "LOGISTIC_REGRESSION",
        "NEURAL_NET"
    };
    
        void setTrainingLabel(int & label_);
    
    
    //GUI
    ofxPanel gui;
    ofParameter<int> trainingLabel;
    ofxIntSlider rectWidth;
    ofxIntSlider rectHeight;
    ofxButton  bTrain, bSave, bLoad, bClear, bRecord, bPause;
    ofxToggle tPause;

    
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
    
    
};

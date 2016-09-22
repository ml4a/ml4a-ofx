/*
  This example demonstrates how to use the GRT Dynamic Time Warping algorithm in openFrameworks with the ofxFacetracker2 to detect gestures over time. 
 
 Accurately training gestures can be tricky, but it should be possible to make a somewhat stable "nod" vs "head shake" vs "no movement" classifier using the orientation inputs or a gesture . Try to save class [0] for your neutral position (no expressions) and make sure to record something in the class.
 
 To do:
 - The app is currently a bit slow or lagging... Find out what is causing this.
 - Using raw points as inputs is currently disabled, since it makes the framerate drop a lot
 - Clean up the way the recordings are drawn to the screen - does not work well with the big nuber of inputs that is currently used
 - Remove or improve the distance matrix
 - Make a more intitive example?
 
 */

#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxFaceTracker2.h"
#include <stdio.h>

//State that we want to use the GRT namespace
using namespace GRT;

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void drawTimeseries();
    void drawTrainingData();
//    void drawDistanceMatrix();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //costume function called when a gesture is recognised
    void gestureRecognised(int g);
    
    //Create some variables for the demo
    TimeSeriesClassificationData trainingData;      		//This will store our training data
    MatrixFloat timeseries;                                 //This will store a single training sample
    GestureRecognitionPipeline pipeline;                    //This is a wrapper for our classifier and any pre/post processing modules 
    bool record;                                            //This is a flag that keeps track of when we should record training data
    UINT trainingClassLabel;                                //This will hold the current label for when we are training the classifier
    string infoText;                                        //This string will be used to draw some info messages to the main app window
    ofTrueTypeFont font;
    ofShader shader;
    ofxGrtTimeseriesPlot predictedClassPlot;
    ofxGrtTimeseriesPlot classLikelihoodsPlot;
    Vector< std::shared_ptr< ofxGrtTimeseriesPlot > > trainingDataPlot;
    Vector< ofxGrtMatrixPlot > distanceMatrixPlots;
    
    
    //    //Initialize the DTW classifier
        DTW dtw;
    
    float rejectCoeff = 0.0000001;
    
    
    //FaceTracker2 stuff
    
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
    
    bool rawBool;
    bool gestureBool;
    bool orientationBool;
    
    bool drawNumbers;
    bool drawFace;
    bool drawPose;
    bool drawVideo;
    
    //Blink test
    int ellipseSize = 255;
    
    int timeOutFilter = 300;
    
    int lastGesture = 0;
    
};

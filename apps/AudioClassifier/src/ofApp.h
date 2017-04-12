#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGrt.h"
#include "ofxGui.h"
#include "ofxOsc.h"

// where to send osc messages by default
#define DEFAULT_OSC_DESTINATION "localhost"
#define DEFAULT_OSC_ADDRESS "/classification"
#define DEFAULT_OSC_PORT 8000

//State that we want to use the GRT namespace
using namespace GRT;

class ofApp : public ofBaseApp{
    
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void trainClassifier();
    void save();
    void load();
    void clear();
    
    void sendOSC();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //OSC
    ofxOscSender sender;
    string oscDestination, oscAddress;
    int oscPort;
    
    
    //MAXIM SOUND
    void audioRequested 	(float * input, int bufferSize, int nChannels); // output method
    void audioReceived 	(float * input, int bufferSize, int nChannels); // input method
    
    float 	* lAudioOut; //outputs
    float   * rAudioOut;
    
    float * lAudioIn; //inputs
    float * rAudioIn;
    
    int		initialBufferSize; //buffer size
    int		sampleRate;
    
    double wave;
    ofxMaxiFFTOctaveAnalyzer oct;
    int nAverages = 12;
    
    ofxMaxiIFFT ifft;
    ofxMaxiFFT mfft;
    int fftSize = 1024;
    int bins, dataSize;
    
    maxiMFCC mfcc;
    double *mfccs;
    float rms = 0;
    
    //GRT
    ClassificationData trainingData;
    GestureRecognitionPipeline pipeline;
    
    bool trainingModeActive;
    bool predictionModeActive;
    
    UINT trainingClassLabel;
    UINT predictedClassLabel;
    string infoText;
    ofTrueTypeFont smallFont;
    ofTrueTypeFont hugeFont;
    ofxGrtTimeseriesPlot predictionPlot;
    int trainingInputs;
    
    //GUI
    ofxPanel gui;
    ofxFloatSlider volThreshold;
    ofxIntSlider triggerTimerThreshold;
    ofxIntSlider sliderClassLabel;
    
    ofxButton  bTrain, bSave, bLoad, bClear;
    ofxToggle tThresholdMode, tRecord; 
    
    //Threshold & triggermode
    bool thresholdMode = false;
    bool singleTrigger = false;
    float predictionAlpha = 255;
    long startTime;
};

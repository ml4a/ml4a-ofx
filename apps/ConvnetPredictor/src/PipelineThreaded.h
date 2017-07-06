#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxGui.h"


class PipelineThreaded : public ofThread, public GestureRecognitionPipeline {
public:
    PipelineThreaded();
    
    virtual void setup(string name, ofxPanel *panelRef, int inputSize) {}
    virtual void setupModel() {}
    virtual void clear() {}
    
    virtual bool addSample(VectorFloat *inputVector) {return false;}
    virtual bool predict(VectorFloat *inputVector) {}
    virtual void update() {}
    virtual float getValue() {return 0.0;}
    
    virtual void threadedFunction() {}
    void startTraining();
    
    bool training;
    bool success;
};

class CategoricalThreaded : public PipelineThreaded {
public:
    void setup(string name, ofxPanel *panelRef, int inputSize);
    void setupModel();
    void clear();

    bool addSample(VectorFloat *inputVector);
    bool predict(VectorFloat *inputVector);
    void update();
    float getValue() {return (float)slider;}
    
    void threadedFunction();
    
    ClassificationData trainingData;
    ofParameter<int> slider;
};

class RegressionThreaded : public PipelineThreaded {
public:
    void setup(string name, ofxPanel *panelRef, int inputSize);
    void setupModel();
    void clear();
    
    bool addSample(VectorFloat *inputVector);
    bool predict(VectorFloat *inputVector);
    void update();
    float getValue() {return slider;}
    
    void threadedFunction();
    
    RegressionData trainingData;
    ofParameter<float> slider;
    float target;
};



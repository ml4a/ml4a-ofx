#pragma once

#include "ofMain.h"
#include "ofxGrt.h"
#include "ofxGui.h"


enum ClassifierType { SUPPORT_VECTOR_MACHINE=0, KNearestNeighbors };
enum RegressifierType { LOGISTIC_REGRESSION=0, NEURAL_NET };


class PipelineThreaded : public ofThread, public GestureRecognitionPipeline {
public:
    PipelineThreaded();
    virtual bool isClassifier(){return false;}
    
    virtual void setup(string name, ofxPanel *panelRef, int inputSize) {}
    virtual void setupModel(const int type) {}
    virtual void clear() {}
    
    virtual bool addSample(VectorFloat *inputVector) {return false;}
    virtual bool predict(VectorFloat *inputVector) {}
    virtual void update(float lerpAmt) {}
    virtual float getValue() {return 0.0;}

    virtual void threadedFunction() {}
    
    void startTraining();
    
    bool training;
    bool success;
};


class CategoricalThreaded : public PipelineThreaded {
public:
    bool isClassifier(){return true;}
    
    void setup(string name, ofxPanel *panelRef, int inputSize, int numClasses);
    void setupModel(const int type);
    void clear();

    bool addSample(VectorFloat *inputVector);
    bool predict(VectorFloat *inputVector);
    void update(float lerpAmt);
    float getValue() {return (float)slider;}
    
    void threadedFunction();
    
    ClassificationData trainingData;
    ofParameter<int> slider;
};


class RegressionThreaded : public PipelineThreaded {
public:
    void setup(string name, ofxPanel *panelRef, int inputSize);
    void setupModel(const int type);
    void clear();
    
    bool addSample(VectorFloat *inputVector);
    bool predict(VectorFloat *inputVector);
    void update(float lerpAmt);
    float getValue() {return slider;}
    
    void threadedFunction();
    
    RegressionData trainingData;
    ofParameter<float> slider;
    float target;
};



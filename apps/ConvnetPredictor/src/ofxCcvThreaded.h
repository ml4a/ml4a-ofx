#pragma once

#include "ofThread.h"
#include "ofxCcv.h"


class ofxCcvThreaded : public ofThread, public ofxCcv {
public:
    ofxCcvThreaded() {
        ready = true;
        isNew = false;
    }
    
    void start() {
        startThread();
    }
    
    void stop() {
        ready = true;
        stopThread();
    }
    
    void setClassify(bool toClassify, int numResults=20) {
        this->toClassify = toClassify;
        this->numResults = numResults;
    }
    
    void setEncode(bool toEncode) {
        this->toEncode = toEncode;
    }
    
    void update(ofBaseHasPixels &pix, int layer) {
        this->layer = layer;
        ready = false;
        img.setFromPixels(pix.getPixels());
    }
    
    void threadedFunction() {
        while(isThreadRunning()) {
            if(lock()) {
                if (!ready) {
                    if (toEncode) {
                        features = encode(img, layer);
                    }
                    if (toClassify) {
                        results = classify(img, numResults);
                    }
                    ready = true;
                    isNew = true;
                }
                unlock();
            } else {
                ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
            }
        }
    }
    
    vector<ofxCcv::Classification> getResults() {
        //unique_lock<std::mutex> lock(mutex);
        return results;
    }
    
    vector<float> getEncoding() {
        return features;
    }
    
    bool isReady() {
        return ready;
    }
    
    bool hasNewResults() {
        if (isNew) {
            isNew = false;
            return true;
        }
        else {
            return false;
        }
    }
    
protected:
    
    ofImage img;
    vector<ofxCcv::Classification> results;
    vector<float> features;
    
    bool isNew;
    bool ready;
    int layer;
    
    int numResults;
    bool toClassify;
    bool toEncode;
};

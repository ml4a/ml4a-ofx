#pragma once

#include "ofxLeapMotion2.h"
#include "ofxControl.h"


enum Handedness { LEFT, RIGHT };


class LeapMotion
{
public:
    ~LeapMotion();
    void              setup();
    void              setGuiPosition(int x, int y);
    void              close() { if (leap.isConnected()) leap.close(); }
    void              setVelocityTracking(bool trackVelocity, int numFrames=10);
    void              setNormalizeTips(bool normalizeTips) {this->normalizeTips = normalizeTips;}
    void              update();
    void              draw(int x, int y, int w, int h);
    void              drawVelocityGraph(Handedness hand, int x, int y, int w, int h);
    
    // left hand getters
    ofPoint           getLeftHandPosition() {return lHandPosition;}
    vector<ofPoint> & getLeftHandFingerTips() {return leftHandTips;}
    ofPoint           getLeftHandFingerTip(int index) {return leftHandTips[index];}
    ofPoint           getLeftHandVelocity() {return handVelocity[0];}
    float             getLeftOpenHandSize() {return lOpenHandSize;}
    ofPoint           getLeftHandNormal() {return lHandNormal;}
    ofPoint           getLeftHandDirection() {return lHandDirection;}
    float             getLeftHandRoll() {return lHandRoll;}
    float             getLeftHandPitch() {return lHandPitch;}
    float             getLeftHandYaw() {return lHandYaw;}
    
    // right hand getters
    ofPoint           getRightHandPosition() {return rHandPosition;}
    vector<ofPoint> & getRightHandFingerTips() {return rightHandTips;}
    ofPoint           getRightHandFingerTip(int index) {return rightHandTips[index];}
    ofPoint           getRightHandVelocity() {return handVelocity[1];}
    float             getRightOpenHandSize() {return rOpenHandSize;}
    ofPoint           getRightHandNormal() {return rHandNormal;}
    ofPoint           getRightHandDirection() {return rHandDirection;}
    float             getRightHandRoll() {return rHandRoll;}
    float             getRightHandPitch() {return rHandPitch;}
    float             getRightHandYaw() {return rHandYaw;}

    // left hand ref getters
    ofPoint *         getLeftHandPositionRef() {return &lHandPosition;}
    vector<ofPoint> * getLeftHandFingerTipsRef() {return &leftHandTips;}
    ofPoint *         getLeftHandFingerTipRef(int index) {return &leftHandTips[index];}
    ofPoint *         getLeftHandVelocityRef() {return &handVelocity[0];}
    float *           getLeftOpenHandSizeRef() {return &lOpenHandSize;}
    ofPoint *         getLeftHandNormalRef() {return &lHandNormal;}
    ofPoint *         getLeftHandDirectionRef() {return &lHandDirection;}
    float *           getLeftHandRollRef() {return &lHandRoll;}
    float *           getLeftHandPitchRef() {return &lHandPitch;}
    float *           getLeftHandYawRef() {return &lHandYaw;}
    
    // right hand ref getters
    ofPoint *         getRightHandPositionRef() {return &rHandPosition;}
    vector<ofPoint> * getRightHandFingerTipsRef() {return &rightHandTips;}
    ofPoint *         getRightHandFingerTipRef(int index) {return &rightHandTips[index];}
    ofPoint *         getRightHandVelocityRef() {return &handVelocity[1];}
    float *           getRightOpenHandSizeRef() {return &rOpenHandSize;}
    ofPoint *         getRightHandNormalRef() {return &rHandNormal;}
    ofPoint *         getRightHandDirectionRef() {return &rHandDirection;}
    float *           getRightHandRollRef() {return &rHandRoll;}
    float *           getRightHandPitchRef() {return &rHandPitch;}
    float *           getRightHandYawRef() {return &rHandYaw;}
    
    bool *            hasFoundHands() {return &foundHands;}
    bool *            hasFoundLeftHand() {return &foundLeftHand;}
    bool *            hasFoundRightHand() {return &foundRightHand;}
    bool              isConnected() {return leap.isConnected();}
    
private:
    
    void              updateFingerTips(bool isNormalized=false);
    void              updateHandStats();
    
    // stats
    vector<ofPoint>   leftHandTips, rightHandTips;
    ofPoint           lHandPosition, rHandPosition;
    ofPoint           lHandNormal, rHandNormal;
    ofPoint           lHandDirection, rHandDirection;
    float             lOpenHandSize, lHandRoll, lHandPitch, lHandYaw;
    float             rOpenHandSize, rHandRoll, rHandPitch, rHandYaw;
    bool              foundHands, foundLeftHand, foundRightHand;
    
    // velocity stats
    vector<ofPoint>   vLeftHandTips, vRightHandTips;
    ofPoint           vLHandNormal, vLHandDirection;
    ofPoint           vRHandNormal, vRHandDirection;
    float             vLOpenHandSize, vLHandRoll, vLHandPitch, vLHandYaw;
    float             vROpenHandSize, vRHandRoll, vRHandPitch, vRHandYaw;
    ofPoint           handVelocity[2];
    
    // internals
    ofxLeapMotion     leap;
    vector            <ofxLeapMotionSimpleHand> simpleHands;
    float             frameWeight;
    vector<ofPoint>   handVelocityList[2];
    ofPoint           prevHandPosition[2];
    int               idxFrame;
    ofEasyCam         cam;
    ofFbo             fbo;
    fingerType        finger[5];
    
    // parameters
    ofxControlWidget  control;
    bool              trackVelocity, pTrackVelocity;
    bool              normalizeTips;
    int               numFrames, pNumFrames;
    float             vLerpRate;
};

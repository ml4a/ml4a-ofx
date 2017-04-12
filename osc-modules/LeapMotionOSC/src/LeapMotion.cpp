#include "LeapMotion.h"

//-----------
void LeapMotion::setup(){
    leap.open();
    
    cam.setOrientation(ofPoint(-20, 0, 0));
    fbo.allocate(1024, 768, GL_RGBA);
    
    normalizeTips = true;
    
    control.setName("LeapMotion");
    control.addToggle("trackVelocity", &trackVelocity);
    control.addSlider("numFrames", &numFrames, 1, 60);
    control.addToggle("normalizeTips", &normalizeTips);
    control.addSlider("vLerpRate", &vLerpRate, 0.0f, 1.0f);
    
    trackVelocity = false;
    numFrames = 15;
    vLerpRate = 0.2;
    
    for (int i=0; i<5; i++) {
        leftHandTips.push_back(ofPoint(0, 0, 0));
        rightHandTips.push_back(ofPoint(0, 0, 0));
        vLeftHandTips.push_back(ofPoint(0, 0, 0));
        vRightHandTips.push_back(ofPoint(0, 0, 0));
    }
    
    finger[0] = THUMB;
    finger[1] = INDEX;
    finger[2] = MIDDLE;
    finger[3] = RING;
    finger[4] = PINKY;
}

//-----------
void LeapMotion::setGuiPosition(int x, int y) {
    control.setPosition(x, y);
}

//-----------
void LeapMotion::setVelocityTracking(bool trackVelocity, int numFrames) {
    this->trackVelocity = trackVelocity;
    if (trackVelocity) {
        this->numFrames = numFrames;
        frameWeight = 1.0f / (float) numFrames;
        idxFrame = 0;
        prevHandPosition[0] = ofPoint(0,0,0);
        prevHandPosition[1] = ofPoint(0,0,0);
        handVelocityList[0].resize(numFrames);
        handVelocityList[1].resize(numFrames);
        handVelocity[0] = ofPoint(0, 0);
        handVelocity[1] = ofPoint(0, 0);
        for (int i=0; i<numFrames; i++) {
            handVelocityList[0][i] = ofPoint(0, 0);
            handVelocityList[1][i] = ofPoint(0, 0);
        }
    }
}

//-----------
void LeapMotion::update(){
    if (!leap.isConnected())    return;
    
    // check if parameters have changed to setVelocity tracking parameters
    if (trackVelocity != pTrackVelocity || numFrames != pNumFrames) {
        setVelocityTracking(trackVelocity, numFrames);
        pTrackVelocity = trackVelocity;
        pNumFrames = numFrames;
    }
    
    simpleHands = leap.getSimpleHands();
    
    // update velocity
    if (trackVelocity) {
        for (int i = 0; i < simpleHands.size(); i++) {
            int idx = simpleHands[i].isLeft ? 0 : 1;
            handVelocity[idx] -= frameWeight * handVelocityList[idx][idxFrame];
            handVelocity[idx] += frameWeight * (simpleHands[i].handPos - prevHandPosition[idx]);
            handVelocityList[idx][idxFrame] = simpleHands[i].handPos - prevHandPosition[idx];
            prevHandPosition[idx] = simpleHands[i].handPos;
        }
        idxFrame = (idxFrame+1) % numFrames;
    }
    
    // update stats
    updateFingerTips(normalizeTips);
    updateHandStats();
}

//-----------
void LeapMotion::updateFingerTips(bool isNormalized) {
    for (int i = 0; i < simpleHands.size(); i++) {
        if (simpleHands[i].isLeft) {
            for (int j=0; j<5; j++) {
                vLeftHandTips[j] = vLeftHandTips[j] * (1.0-vLerpRate) + (simpleHands[i].fingers[finger[j]].tip - leftHandTips[j]) * vLerpRate;
                leftHandTips[j].set(simpleHands[i].fingers[finger[j]].tip);
                if (isNormalized) {
                    leftHandTips[j] -= simpleHands[i].handPos;
                }
            }
        }
        else {
            for (int j=0; j<5; j++) {
                vRightHandTips[j] = vRightHandTips[j] * (1.0-vLerpRate) + (simpleHands[i].fingers[finger[j]].tip - rightHandTips[j]) * vLerpRate;
                rightHandTips[j].set(simpleHands[i].fingers[finger[j]].tip);
                if (isNormalized) {
                    rightHandTips[j] -= simpleHands[i].handPos;
                }
            }
        }
    }
}

//-----------
void LeapMotion::updateHandStats() {
    foundHands = simpleHands.size() > 0;
    foundLeftHand = false;
    foundRightHand = false;
    
    for (int i = 0; i < simpleHands.size(); i++) {
        ofPoint minPos = ofPoint( 999,  999,  999);
        ofPoint maxPos = ofPoint(-999, -999, -999);
        
        for (int j=0; j<5; j++) {
            ofPoint f = simpleHands[i].fingers[finger[j]].tip;
            if      (f.x < minPos.x)    minPos.x = f.x;
            else if (f.x > maxPos.x)    maxPos.x = f.x;
            if      (f.y < minPos.y)    minPos.y = f.y;
            else if (f.y > maxPos.y)    maxPos.y = f.y;
            if      (f.z < minPos.z)    minPos.z = f.z;
            else if (f.z > maxPos.z)    maxPos.z = f.z;
        }
        if (simpleHands[i].isLeft) {
            foundLeftHand = true;
            lHandPosition = simpleHands[i].handPos;
            lOpenHandSize = (maxPos.x-minPos.x) * (maxPos.y-minPos.y) * (maxPos.z-minPos.z);
            lHandNormal = simpleHands[i].handNormal;
            lHandDirection = simpleHands[i].direction;
            lHandRoll = simpleHands[i].roll;
            lHandPitch = simpleHands[i].pitch;
            lHandYaw = simpleHands[i].yaw;
        }
        else {
            foundRightHand = true;
            rHandPosition = simpleHands[i].handPos;
            rOpenHandSize = (maxPos.x-minPos.x) * (maxPos.y-minPos.y) * (maxPos.z-minPos.z);
            rHandNormal = simpleHands[i].handNormal;
            rHandDirection = simpleHands[i].direction;
            rHandRoll = simpleHands[i].roll;
            rHandPitch = simpleHands[i].pitch;
            rHandYaw = simpleHands[i].yaw;
        }
    }
}

//-----------
void LeapMotion::drawVelocityGraph(Handedness hand, int x, int y, int w, int h) {
    ofPoint velocity = (hand == LEFT) ? handVelocity[0] : handVelocity[1];
    
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(x, y);
    
    ofSetColor(0);
    
    if (hand == LEFT)
        ofDrawBitmapString("Left velocity XY", 0, -4);
    else
        ofDrawBitmapString("Right velocity XY", 0, -4);
    ofDrawBitmapString("Z", w+2, -4);
    ofNoFill();
    ofSetLineWidth(2);
    
    ofDrawRectangle(0, 0, w, h);
    ofDrawLine(-5+w/2, h/2, 5+w/2, h/2);
    ofDrawLine(w/2, -5+h/2, w/2, 5+h/2);
    
    
    ofFill();
    float vx = ofMap(velocity.x, -10, 10, 0, w);
    float vy = ofMap(velocity.y, -10, 10, h, 0);
    float vz = ofMap(velocity.z, -10, 10, 0, h);
    ofDrawEllipse(vx, vy, 16, 16);
    ofDrawEllipse(w+8, vz, 16, 16);
    
    ofPopStyle();
    ofPopMatrix();
}

//-----------
void LeapMotion::draw(int x, int y, int w, int h){
    fbo.begin();
    
    //ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
    ofFill();
    ofSetColor(90);
    ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
    
    ofSetColor(200);
    ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);
    
    cam.begin();
    
    ofPushMatrix();
    ofRotate(90, 0, 0, 1);
    ofSetColor(20);
    ofDrawGridPlane(800, 20, false);
    ofPopMatrix();
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    for(int i = 0; i < simpleHands.size(); i++){
        bool isLeft        = simpleHands[i].isLeft;
        ofPoint handPos    = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;
        
        ofSetColor(0, 0, 255);
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
        ofSetColor(255, 255, 0);
        ofDrawArrow(handPos, handPos + 100*handNormal);
        
        for (int f=0; f<5; f++) {
            ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
            
            ofSetColor(0, 255, 0);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
            ofDrawSphere(pip.x, pip.y, pip.z, 12);
            ofDrawSphere(dip.x, dip.y, dip.z, 12);
            ofDrawSphere(tip.x, tip.y, tip.z, 12);
            
            ofSetColor(255, 0, 0);
            ofSetLineWidth(20);
            ofDrawLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
            ofDrawLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofDrawLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
        }
    }
    ofSetColor(255);
    
    cam.end();
    fbo.end();
    fbo.draw(x, y, w, h);
}

//-----------
LeapMotion::~LeapMotion(){
    close();
}

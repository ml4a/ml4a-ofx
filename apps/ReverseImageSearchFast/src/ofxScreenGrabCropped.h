#pragma once

#include "ofMain.h"
#include "ofxScreenGrab.h"

class ofxScreenGrabCropped {
public:
    void setup(int width, int height, bool retina) {
        this->width = width;
        this->height = height;
        this->retina = retina;
        grabber.setup(ofGetScreenWidth(), ofGetScreenHeight(), retina);
        tl.set(0, 0);
        br.set(width, height);
        mouseRadius = 10;
        debug = false;
    }
    
    void update() {
        if (debug) {
            grabber.grabScreen(0, 0);
            grabber.getTextureReference().readToPixels(pixels);
            cropped.setFromPixels(pixels);
            cropped.crop(ofMap(tl.x, 0, width, 0, grabber.getWidth()),
                         ofMap(tl.y, 0, height, 0, grabber.getHeight()),
                         ofMap(br.x-tl.x, 0, width, 0, grabber.getWidth()),
                         ofMap(br.y-tl.y, 0, height, 0, grabber.getHeight()));
        }
        else {
            grabber.grabScreen(ofMap(tl.x, 0, width, 0, ofGetScreenWidth()),
                               ofMap(tl.y, 0, height, 0, ofGetScreenHeight()));
        }
    }
    
    void drawDebug() {
        ofPushStyle();
        ofSetColor(255, 127);
        grabber.draw(0, 0, width, height);
        ofSetColor(255);
        cropped.draw(tl.x, tl.y, br.x-tl.x, br.y-tl.y);
        ofSetColor(highlightingTl ? ofColor(0, 255, 0) : ofColor(255, 0, 0));
        ofDrawCircle(tl.x, tl.y, mouseRadius);
        ofSetColor(highlightingBr ? ofColor(0, 255, 0) : ofColor(255, 0, 0));
        ofDrawCircle(br.x, br.y, mouseRadius);
        ofPopStyle();
    }
    
    void draw(int x, int y, int w=-1, int h=-1) {
        if (w==-1 || h==-1) {
            grabber.draw(x, y);
        } else {
            grabber.draw(x, y, w, h);
        }
    }
    
    void mouseMoved(int x, int y){
        if (!debug || draggingTl || draggingBr || dragging) return;
        highlightingTl = false;
        highlightingBr = false;
        float dtl = ofDist(x, y, tl.x, tl.y);
        float dtr = ofDist(x, y, br.x, br.y);
        if (dtl < dtr && dtl < mouseRadius) {
            highlightingTl = true;
        } else if (dtr < dtl && dtr < mouseRadius) {
            highlightingBr = true;
        }
    }
    
    void mouseDragged(int x, int y){
        if (!debug) return;
        if (dragging) {
            float dx = x-anchor.x;
            float dy = y-anchor.y;
            tl.set(ofClamp(tl.x + dx, 0, width), ofClamp(tl.y + dy, 0, height));
            br.set(ofClamp(br.x + dx, 0, width), ofClamp(br.y + dy, 0, height));
            anchor.set(x, y);
        } else if (draggingTl) {
            tl.set(ofClamp(x, 0, width), ofClamp(y, 0, height));
        } else if (draggingBr) {
            br.set(ofClamp(x, 0, width), ofClamp(y, 0, height));
        }
    }
    
    void mousePressed(int x, int y){
        if (!debug) return;
        if (highlightingTl) {
            draggingTl = true;
        } else if (highlightingBr) {
            draggingBr = true;
        } else if (x > tl.x && x < br.x && y > tl.y && y < br.y) {
            dragging = true;
            anchor.set(x, y);
        }
    }
    
    void mouseReleased(int x, int y){
        if (!debug) return;
        draggingTl = false;
        draggingBr = false;
        dragging = false;
    }
    
    void setDebug(bool debug) {
        this->debug = debug;
        if (debug) {
            grabber.setup(ofGetScreenWidth(), ofGetScreenHeight(), retina);
        } else {
            grabber.setup(ofMap(br.x - tl.x, 0, width, 0, ofGetScreenWidth()),
                          ofMap(br.y - tl.y, 0, height, 0, ofGetScreenHeight()), retina);
        }
    }
    
    ofxScreenGrab & getGrabber() {return grabber;}
    
    void toggleDebug() {setDebug(!isDebug());}
    
    bool isDebug() {return debug;}
    
private:
    
    ofxScreenGrab grabber;
    ofPixels pixels;
    ofImage cropped;
    ofPoint tl, br, anchor;
    bool highlightingTl, highlightingBr;
    bool draggingTl, draggingBr, dragging;
    int width, height;
    bool retina;
    float mouseRadius;
    bool debug;
    
};

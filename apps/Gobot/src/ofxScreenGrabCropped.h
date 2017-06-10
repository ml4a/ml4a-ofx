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
        tlx = 0;
        tly = 0;
        marginx = 0;
        marginy = 0;
        brx = width;
        bry = height;
        anchorx = width/2;
        anchory = height/2;
        setMargins();
        mouseRadius = 10;
        debug = false;
    }
    
    void setMargins() {
        marginx = int(float(brx-tlx)/36.0);
        marginy = int(float(bry-tly)/36.0);
    }
    
    void update() {
        if (debug) {
            grabber.grabScreen(0, 0);
            grabber.getTextureReference().readToPixels(pixels);
            cropped.setFromPixels(pixels);
            cropped.crop(ofMap(tlx, 0, width, 0, grabber.getWidth()),
                         ofMap(tly, 0, height, 0, grabber.getHeight()),
                         ofMap(brx-tlx, 0, width, 0, grabber.getWidth()),
                         ofMap(bry-tly, 0, height, 0, grabber.getHeight()));
        }
        else {
            grabber.grabScreen(ofMap(tlx-marginx, 0, width, 0, ofGetScreenWidth()),
                               ofMap(tly-marginy, 0, height, 0, ofGetScreenHeight()));
        }
    }
    
    void saveCorners(string path) {
        ofXml xml;
        xml.addChild("corners");
        xml.setTo("corners");
        xml.addValue("tlx", tlx);
        xml.addValue("tly", tly);
        xml.addValue("blx", brx);
        xml.addValue("bry", bry);
        xml.addValue("anchorx", anchorx);
        xml.addValue("anchory", anchory);
        xml.save(path);
    }
    
    void loadCorners(string path) {
        ofXml xml;
        xml.load(path);
        xml.setTo("corners");
        tlx = xml.getValue<int>("tlx");
        tly = xml.getValue<int>("tly");
        brx = xml.getValue<int>("blx");
        bry = xml.getValue<int>("bry");
        anchorx = xml.getValue<int>("anchorx");
        anchory = xml.getValue<int>("anchory");
        setMargins();
    }
    
    void drawDebug() {
        ofPushStyle();
        ofSetColor(255, 127);
        grabber.draw(0, 0, width, height);
        ofSetColor(255, 255);
        cropped.draw(tlx, tly, brx-tlx, bry-tly);
        ofSetColor(highlightingTl ? ofColor(0, 255, 0) : ofColor(255, 0, 0));
        ofDrawCircle(tlx, tly, mouseRadius);
        ofSetColor(highlightingBr ? ofColor(0, 255, 0) : ofColor(255, 0, 0));
        ofDrawCircle(brx, bry, mouseRadius);
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
        float dtl = ofDist(x, y, tlx, tly);
        float dtr = ofDist(x, y, brx, bry);
        if (dtl < dtr && dtl < mouseRadius) {
            highlightingTl = true;
        } else if (dtr < dtl && dtr < mouseRadius) {
            highlightingBr = true;
        }
    }
    
    void mouseDragged(int x, int y){
        if (!debug) return;
        if (dragging) {
            float dx = x-anchorx;
            float dy = y-anchory;
            tlx = ofClamp(tlx + dx, 0, width);
            tly = ofClamp(tly + dy, 0, height);
            brx = ofClamp(brx + dx, 0, width);
            bry = ofClamp(bry + dy, 0, height);
            anchorx = x;
            anchory = y;
        } else if (draggingTl) {
            tlx = ofClamp(x, 0, width);
            tly = ofClamp(y, 0, height);
        } else if (draggingBr) {
            brx = ofClamp(x, 0, width);
            bry = ofClamp(y, 0, height);
        }
        setMargins();
    }
    
    void mousePressed(int x, int y){
        if (!debug) return;
        if (highlightingTl) {
            draggingTl = true;
        } else if (highlightingBr) {
            draggingBr = true;
        } else if (x > tlx && x < brx && y > tly && y < bry) {
            dragging = true;
            anchorx = x;
            anchory = y;
        }
    }
    
    void mouseReleased(int x, int y){
        if (!debug) return;
        draggingTl = false;
        draggingBr = false;
        dragging = false;
        setMargins();
    }
    
    void setDebug(bool debug) {
        this->debug = debug;
        if (debug) {
            grabber.setup(ofGetScreenWidth(), ofGetScreenHeight(), retina);
        } else {
            int factor = 2;
            grabber.setup(factor * marginx + ofMap(brx - tlx, 0, width, 0, ofGetScreenWidth()),
                          factor * marginy + ofMap(bry - tly, 0, height, 0, ofGetScreenHeight()), retina);
        }
    }
    
    ofxScreenGrab & getGrabber() {return grabber;}
    
    void toggleDebug() {setDebug(!isDebug());}
    
    bool isDebug() {return debug;}
    
    int tlx, tly, brx, bry, anchorx, anchory;
    int marginx, marginy;
    
private:
    
    ofxScreenGrab grabber;
    ofPixels pixels;
    ofImage cropped;
    bool highlightingTl, highlightingBr;
    bool draggingTl, draggingBr, dragging;
    int width, height;
    bool retina;
    float mouseRadius;
    bool debug;
};

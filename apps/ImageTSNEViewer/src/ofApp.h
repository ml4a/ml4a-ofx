#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"


struct ImageThumb {
public:
    ofImage image;
    ofPoint point;
    float t;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void saveScreenshot();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
        void mouseScrolled(ofMouseEventArgs &evt);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    vector<ImageThumb> thumbs;

    float maxDim;
    ofVec2f position;

    ofParameter<float> scale;
    ofParameter<float> imageSize;
    ofxButton save;
    ofxPanel gui;
    
    string tsnePath;
    bool parsingSuccessful;
};


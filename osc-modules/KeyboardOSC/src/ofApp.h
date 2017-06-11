#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "KeyCodes.h"
#include "KeyInput.h"

#define OSC_ADDRESS_ROOT "/wek/outputs"
#define OSC_PORT 12000

class ofApp : public ofBaseApp{

	public:
		void setup();
        void setupOSC();
        void loadSettings();
		void update();
		void draw();
        void mouseReleased(int x, int y, int button);
        int convertStringToKeycode(string key);
        void checkSettings();
        void sendKeyCode(int key);
        void triggerKeyDown();
        void triggerKeyUp();
    
    KeyCodes keyCodes;
    
    vector<KeyInput> keyInputs;
    int keyDuration = 100;
    
    ofxOscReceiver receiver;
    ofxOscMessage oscMessage;
    int oscPort;
    string oscAddressRoot;
    string oscAddressDisable;
    string oscAddressEnable;
    
    bool receivingOSC = false;
    bool allowVirtualKeystrokes = false;
    bool triggerKeysAsToggles = false;
    
    int oscValue;
    int oscId = -1;
    int totalKeyInputs = 0;
    
    // Fading text
    int frameCounter = 0;
    int stayOnScreenFrames = 60;
    float fadeStep;
    
    bool canPressKey = true;
    int timer = 0;
    
    char charKey;
    vector<char> pressedKeys;
    
    time_t tSettings;
};

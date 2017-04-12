#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxControl.h"
#include "AudioUnitController.h"


#define DEFAULT_INPUT_ADDRESS "/wek/outputs"
#define DEFAULT_PORT_IN 12000

struct ParamterAudioUnitPair {
    ParamterAudioUnitPair(AudioUnitController *au, string name);
    void setValue(float value);
    
    AudioUnitController *au;
    string name;
};


class Selection {
public:
    void add(AudioUnitController *au, string name);
    void remove(AudioUnitController *au, string name);
    void removeAudioUnit(string audioUnitName);
    void setValue(int idx, float value);
    void draw(int x, int y);
    
    vector<ParamterAudioUnitPair> parameters;
};


class Section {
public:
    Section(AudioUnitController *au, string audioUnitName, string sectionName, vector<AudioUnitParameterInfo> & parameters, ofxControlWidget *gui, Selection *selection);
    void eventSelectParameter(ofxControlMenuEventArgs & e);
    void remove();

    AudioUnitController *au;
    ofxControlWidget *gui;
    Selection *selection;
    ofxControlMenu *menu;
    string audioUnitName, sectionName;
};


class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void addAudioUnit(string name);
    void removeAudioUnit(string name);
		
    void selectAudioUnit(ofxControlMenuEventArgs & evt);
    void selectMidiDevice(ofxControlMenuEventArgs & evt){
        currentMidiDevice = &units[evt.toggle->getName()];
    }
    
    map<string, vector<string> > unitLookup;
    map<string, AudioUnitController> units;
    map<string, ofxControlMenu*> unitMenus;
    
    ofxAudioUnitMixer mixer;
    ofxAudioUnitOutput output;
    int idxMixer;

    ofxControlWidget gui;
    ofxControlWidget synthGui;
    ofxControlMenu *midiGui;

    AudioUnitController *currentMidiDevice;
    
    vector<Section*> sections;
    Selection selection;
    
    ofxOscReceiver osc;
    string addressRoot;
    int portIn;
};

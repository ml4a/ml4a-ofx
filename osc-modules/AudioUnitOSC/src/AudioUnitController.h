#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"


class AudioUnitControllerParameter
{
public:
    AudioUnitControllerParameter(ofxAudioUnitSampler *synth, AudioUnitParameterInfo parameter, int index);
    ofParameter<float> & getParameter() {return value;}
    void setValue(float v);
    
private:
    void parameterChanged(float & v);
    
    int index;
    ofParameter<float> value;
    ofxAudioUnitSampler *synth;
};


class AudioUnitController
{
public:
    void setup(string name, OSType type, OSType subType, OSType manufacturer);
    void connectTo(ofxAudioUnitMixer & mixer, int bus);
    void draw(int x_, int y_);
    void showUI() {synth.showUI();}
    
    string getName() {return name;}
    ofxAudioUnitSampler & getSynth() {return synth;}
    ofParameter<float> & getParameter(string name);
    
    void setVolume(float volume);
    
    void savePreset(string name);
    void loadPreset(string name);
    
    map<string, vector<AudioUnitParameterInfo> > & getParameterGroups() {return parameterGroups;}
    
    void setColor(ofColor color) {this->color = color;}
    ofColor getColor() {return color;}
    
protected:
    
    static void audioUnitParameterChanged(void * context, void * object, const AudioUnitEvent * event, UInt64 hostTime, AudioUnitParameterValue value);
    void loadParameterGroups();
    
    string name;
    map<int, AudioUnitControllerParameter*> parameters;
    map<string, vector<AudioUnitParameterInfo> > parameterGroups;
    ofxAudioUnitSampler synth;
    ofxAudioUnitMixer *mixer;
    int bus;
    AUEventListenerRef auEventListener;
    
    OSType type;
    OSType subType;
    OSType manufacturer;
    ofColor color;
};



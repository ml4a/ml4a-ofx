#pragma once

#include "ofMain.h"
#include "ofxControl.h"
#include "ofxAbletonLive.h"


class AbletonGui
{
public:
    struct TrackParameter {
        ofParameter<float> * parameter;
        string name;
        TrackParameter(ofParameter<float> * parameter, string name);
    };
    
    class AbletonGuiTrack {
    public:
        AbletonGuiTrack(ofxAbletonLiveTrack *track, AbletonGui *main);
        void setup();
        void eventSelectParameter(ofxControlMenuEventArgs & e);
        
        AbletonGui *main;
        ofxAbletonLiveTrack *track;
        ofxControlWidget panel;
        bool tVisible;
    };
    
    void setup(ofxAbletonLive * live, string addressRoot, int portIn);
    void update();
    void draw();
    
    void parseMessage(ofxOscMessage & msg);
    void setSlider(int idx, float value) {sliders[idx]->setValue(value);}
    
    void addOutput(TrackParameter * tp);
    void removeOutput(TrackParameter * tp);
    
    void clear();
    
    vector<AbletonGuiTrack*> & getTracks() {return tracks;}
    
    map<ofxControlToggle*, TrackParameter*> parameterLU;
    map<ofxControlSlider<float>*, TrackParameter*> activeParameters;
    map<string, ofxControlSlider<float>*> activeAddresses;
    vector<ofxControlSlider<float>*> sliders;

private:
    
    void refreshAddresses();
    void eventSetPort(ofxControlTextBoxEventArgs & e);
    void eventSetAddress(ofxControlTextBoxEventArgs & e);
    void eventParameterChanged(ofxControlSliderEventArgs<float> & e);
    void eventSelectPlayback(ofxControlMenuEventArgs & e);
    void eventSelectTrack(ofxControlMenuEventArgs & e);
    
    ofxControlWidget panel;
    ofxControlWidget main;
    ofxControlWidget *active;
    
    bool abletonActive;
    int portIn;
    ofxOscReceiver osc;
    string addressRoot;
    ofxAbletonLive *live;
    vector<AbletonGuiTrack*> tracks;
};
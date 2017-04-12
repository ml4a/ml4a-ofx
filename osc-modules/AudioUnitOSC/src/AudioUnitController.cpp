#include "AudioUnitController.h"


AudioUnitControllerParameter::AudioUnitControllerParameter(ofxAudioUnitSampler *synth, AudioUnitParameterInfo parameter, int index)
{
    this->synth = synth;
    this->index = index;
    value.set(parameter.name, parameter.defaultValue, parameter.minValue, parameter.maxValue);
    value.addListener(this, &AudioUnitControllerParameter::parameterChanged);
}

void AudioUnitControllerParameter::parameterChanged(float & v)
{
    //synth->setParameter(index, 0, value);
    AudioUnitSetParameter(*synth, index, kAudioUnitScope_Global, 0, value, 0);
}

void AudioUnitControllerParameter::setValue(float v)
{
    value.set(v);
}

void AudioUnitController::setup(string name, OSType type, OSType subType, OSType manufacturer)
{
    this->name = name;
    this->type = type;
    this->subType = subType;
    this->manufacturer = manufacturer;
    color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
    synth = ofxAudioUnitSampler(type, subType, manufacturer);
    loadParameterGroups();
    AUEventListenerCreate(&AudioUnitController::audioUnitParameterChanged,
                          this, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode,
                          0.005, // minimum callback interval (seconds)
                          0.005, // callback granularity (for rate-limiting)
                          &auEventListener);
}

void AudioUnitController::connectTo(ofxAudioUnitMixer & mixer, int bus)
{
    synth.connectTo(mixer, bus);
    this->mixer = &mixer;
    this->bus = bus;
}

void AudioUnitController::audioUnitParameterChanged(void *context, void *object, const AudioUnitEvent *event, UInt64 hostTime, AudioUnitParameterValue parameterValue)
{
    ((AudioUnitController *) context)->parameters[event->mArgument.mParameter.mParameterID]->setValue(parameterValue);
}

void AudioUnitController::loadParameterGroups()
{
    int clumpId = -1;
    string groupName;
    vector<AudioUnitParameterInfo> params = synth.getParameterList();
    for (int p = 0; p < params.size(); p++) {
        if (params[p].clumpID != clumpId) {
            clumpId = params[p].clumpID;
            groupName = params[p].name;
            vector<AudioUnitParameterInfo> parameters;
            parameterGroups[groupName] = parameters;
        }
        parameterGroups[groupName].push_back(params[p]);
    }
}

void AudioUnitController::draw(int x_, int y_)
{
    int x = x_;
    int y = y_;
    int idxGroup = 0;
    map<string, vector<AudioUnitParameterInfo> >::iterator it = parameterGroups.begin();
    for (; it != parameterGroups.end(); ++it) {
        string s = ofToString("Group "+ofToString(idxGroup++));
        ofDrawBitmapString(s, x, y);
        y += 15;
        vector<AudioUnitParameterInfo>::iterator itp = it->second.begin();
        for (; itp != it->second.end(); ++itp) {
            string s = ofToString((*itp).name) + " (" + ofToString((*itp).minValue) + "," + ofToString((*itp).maxValue) + ")";
            ofDrawBitmapString(s, x + 8, y);
            x = y > 560 ? x + 240 : x;
            y = y > 560 ? y_ : y + 15;
        }
        y = y > 560 ? y_ : y + 8;
    }
}

ofParameter<float> & AudioUnitController::getParameter(string name)
{
    vector<AudioUnitParameterInfo> params = synth.getParameterList();
    for (int p = 0; p < params.size(); p++)
    {
        if (name == params[p].name)
        {
            AudioUnitParameter param = {synth, static_cast<AudioUnitParameterID>(p), kAudioUnitScope_Global, 0};
            AUListenerAddParameter(auEventListener, this, &param);
            parameters[p] = new AudioUnitControllerParameter(&synth, params[p], p);
            return parameters[p]->getParameter();
        }
    }
    // if we got here, no such parameter exists
    ofLog(OF_LOG_ERROR, "No parameter called "+name+" found");
    return;
}

void AudioUnitController::setVolume(float volume)
{
    mixer->setInputVolume(volume, bus);
}

void AudioUnitController::savePreset(string name)
{
    synth.saveCustomPresetAtPath(ofToDataPath(name+".aupreset"));
}

void AudioUnitController::loadPreset(string name)
{
    synth.loadCustomPresetAtPath(ofToDataPath(name+".aupreset"));
}

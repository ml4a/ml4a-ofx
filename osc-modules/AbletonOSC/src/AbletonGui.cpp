#include "AbletonGui.h"


AbletonGui::TrackParameter::TrackParameter(ofParameter<float> * parameter, string name) {
    this->parameter = parameter;
    this->name = name;
}

void AbletonGui::AbletonGuiTrack::eventSelectParameter(ofxControlMenuEventArgs & e) {
    if (e.value) {
        main->addOutput(main->parameterLU[e.toggle]);
    }
    else {
        main->removeOutput(main->parameterLU[e.toggle]);
    }
}

AbletonGui::AbletonGuiTrack::AbletonGuiTrack(ofxAbletonLiveTrack *track, AbletonGui *main)
{
    this->track = track;
    this->main = main;
    tVisible = false;
    setup();
}

void AbletonGui::AbletonGuiTrack::setup()
{
    // setup device guis
    panel.setName(track->getName());
    panel.setAutoDraw(false);
    panel.setAutoUpdate(false);
    
    // setup playback gui
    ofxControlMenu *playback = panel.addMenu("Playback", this, &AbletonGuiTrack::eventSelectParameter, true);
    main->parameterLU[playback->addToggle("Volume", new bool(false))] = new TrackParameter(&track->getVolume(), ofToString(track->getName())+": Volume");
    main->parameterLU[playback->addToggle("Pan", new bool(false))] = new TrackParameter(&track->getPan(), ofToString(track->getName())+": Pan");
    
    map<int, ofxAbletonLiveDevice*>::iterator itd = track->getDevices().begin();
    for (; itd != track->getDevices().end(); ++itd) {
        map<int, ofxAbletonLiveParameter*>::iterator itp = itd->second->getParameters().begin();
        ofxControlMenu *menu = panel.addMenu(itd->second->getName(), this, &AbletonGuiTrack::eventSelectParameter, true);
        for (; itp != itd->second->getParameters().end(); ++itp) {
            main->parameterLU[menu->addToggle(itp->second->getName(), new bool(false))] = new TrackParameter(itp->second->getParameter(), ofToString(track->getName())+": "+itp->second->getParameter()->getName());
        }
    }
    // sends, clips
}

void AbletonGui::setup(ofxAbletonLive * live, string addressRoot, int portIn)
{
    this->live = live;
    this->addressRoot = addressRoot;
    this->portIn = portIn;
    this->abletonActive = true;

    panel.setName("Ableton Live");
    ofxControlMenu *playback = panel.addMenu("Playback", this, &AbletonGui::eventSelectPlayback, true);
    parameterLU[playback->addToggle("Tempo", new bool(false))] = new TrackParameter(&live->getTempo(), "main: Tempo");
    parameterLU[playback->addToggle("Pan", new bool(false))] = new TrackParameter(&live->getPan(), "main: Pan");
    parameterLU[playback->addToggle("Volume", new bool(false))] = new TrackParameter(&live->getVolume(), "main: Volume");
    parameterLU[playback->addToggle("CrossFade", new bool(false))] = new TrackParameter(&live->getCrossFader(), "main: CrossFader");
    
    ofxControlMenu *view = panel.addMenu("View Tracks", this, &AbletonGui::eventSelectTrack, true);
    
    map<int, ofxAbletonLiveTrack*>::iterator it = live->getTracks().begin();
    map<int, ofxAbletonLiveReturnTrack*>::iterator itr = live->getReturnTracks().begin();
    for (; it != live->getTracks().end(); ++it) {
        AbletonGuiTrack *newTrack = new AbletonGuiTrack(it->second, this);
        tracks.push_back(newTrack);
        view->addToggle(it->second->getName(), &newTrack->tVisible);
    }
    for (; itr != live->getReturnTracks().end(); ++itr) {
        AbletonGuiTrack *newTrack = new AbletonGuiTrack(itr->second, this);
        tracks.push_back(newTrack);
        view->addToggle(itr->second->getName(), &newTrack->tVisible);
    }
    AbletonGuiTrack *newMasterTrack = new AbletonGuiTrack(live->getMasterTrack(), this);
    tracks.push_back(newMasterTrack);
    view->addToggle(live->getMasterTrack()->getName(), &newMasterTrack->tVisible);
    
    main.setName("AbletonOSC");
    main.setWidth(320);
    
    ofxControlWidget *oscGui = main.addWidget("Osc Input Port");
    oscGui->setName("Osc In");
    oscGui->addTextBox("Port", this, &AbletonGui::eventSetPort)->setValue(ofToString(portIn));
    oscGui->addTextBox("Address", this, &AbletonGui::eventSetAddress)->setValue(addressRoot);
    oscGui->addToggle("Active", &abletonActive);

    main.setHeaderColor(ofColor::darkorange);
    active = main.addWidget("Output parameters");
    active->setCollapsible(true);
    panel.setPosition(main.getRectangle().x + main.getRectangle().width + 10, main.getRectangle().y);
    
    osc.setup(portIn);
}

void AbletonGui::update() {
    if (!abletonActive) return;
    
    while (osc.hasWaitingMessages()) {
        ofxOscMessage msg;
        osc.getNextMessage(msg);
        parseMessage(msg);
    }
}

void AbletonGui::draw() {
    if (live == NULL) return;
    
    panel.draw();
    vector<AbletonGuiTrack*>::iterator it = tracks.begin();
    for (; it != tracks.end(); ++it) {
        if ((*it)->tVisible) {
            (*it)->panel.draw();
        }
    }
}

void AbletonGui::addOutput(TrackParameter * tp) {
    ofxControlSlider<float> *newSlider = active->addSlider(tp->name, new float(*tp->parameter), tp->parameter->getMin(), tp->parameter->getMax(), this, &AbletonGui::eventParameterChanged);
    activeParameters[newSlider] = tp;
    sliders.push_back(newSlider);
    refreshAddresses();
}

void AbletonGui::refreshAddresses() {
    activeAddresses.clear();
    for (int s = 0; s < sliders.size(); s++) {
        string sliderAddress = addressRoot + "/" + ofToString(s);
        activeAddresses[sliderAddress] = sliders[s];
    }
}

void AbletonGui::removeOutput(TrackParameter * tp) {
    map<ofxControlSlider<float>*, TrackParameter*>::iterator it = activeParameters.begin();
    while (it != activeParameters.end()) {
        if (it->second == tp) {
            // remove slider reference
            vector<ofxControlSlider<float>*>::iterator its = sliders.begin();
            while (its != sliders.end()) {
                if ((*its) == it->first) {
                    sliders.erase(its);
                }
                else {
                    ++its;
                }
            }
            // remove from active parameters
            string name = it->first->getName();
            it = activeParameters.erase(it);
            active->removeElement(name);            
        }
        else {
            ++it;
        }
    }
    refreshAddresses();
}

void AbletonGui::parseMessage(ofxOscMessage & msg)
{
    string address = msg.getAddress();
    if (address == addressRoot) {
        for (int idx = 0; idx < min((int)sliders.size(), msg.getNumArgs()); idx++) {
            sliders[idx]->setValue(msg.getArgAsFloat(idx));
        }
    }
    else if (activeAddresses.count(address) >0) {
        activeAddresses[address]->setValue(msg.getArgAsFloat(0));
    }
}

void AbletonGui::eventSetPort(ofxControlTextBoxEventArgs & e) {
    portIn = ofToInt(e.value);
    osc.setup(portIn);
}

void AbletonGui::eventSetAddress(ofxControlTextBoxEventArgs & e) {
    addressRoot = e.value;
    refreshAddresses();
}

void AbletonGui::eventSelectTrack(ofxControlMenuEventArgs & e)
{
    ofPoint position = ofPoint(panel.getRectangle().x + panel.getRectangle().width + 4, panel.getRectangle().y);
    vector<AbletonGuiTrack*>::iterator it = tracks.begin();
    for (; it != tracks.end(); ++it) {
        if ((*it)->tVisible) {
            (*it)->panel.setAutoUpdate(true);
            (*it)->panel.setPosition(position);
            position = ofPoint((*it)->panel.getRectangle().x + (*it)->panel.getRectangle().width + 4, (*it)->panel.getRectangle().y);
        }
        else {
            (*it)->panel.setAutoUpdate(false);
        }
    }
}

void AbletonGui::clear() {
    active->clearElements();
    vector<AbletonGuiTrack*>::iterator it = tracks.begin();
    while (it != tracks.end()) {
        (*it)->panel.clearElements();
    }
    panel.clearElements();
    main.clearElements();
}

void AbletonGui::eventParameterChanged(ofxControlSliderEventArgs<float> & e) {
    if (abletonActive) {
        activeParameters[e.slider]->parameter->set(e.value);
    }
}

void AbletonGui::eventSelectPlayback(ofxControlMenuEventArgs & e) {
    if (e.value) {
        addOutput(parameterLU[e.toggle]);
    }
    else {
        removeOutput(parameterLU[e.toggle]);
    }
}

#include "ofApp.h"

//--------------------------------------------------------------
ParamterAudioUnitPair::ParamterAudioUnitPair(AudioUnitController *au, string name){
    this->au = au;
    this->name = name;
}

//--------------------------------------------------------------
void ParamterAudioUnitPair::setValue(float value) {
    float ival = ofLerp(au->getParameter(name).getMin(),
                        au->getParameter(name).getMax(),
                        value);
    au->getParameter(name).set(ival);
}

//--------------------------------------------------------------
void Selection::add(AudioUnitController *au, string name) {
    ParamterAudioUnitPair pa(au, name);
    parameters.push_back(pa);
}

//--------------------------------------------------------------
void Selection::remove(AudioUnitController *au, string name) {
    vector<ParamterAudioUnitPair>::iterator it = parameters.begin();
    while (it != parameters.end())
    {
        if (it->name == name && it->au == au) {
            parameters.erase(it);
        }
        else {
            ++it;
        }
    }
}

//--------------------------------------------------------------
void Selection::removeAudioUnit(string audioUnitName) {
    vector<ParamterAudioUnitPair>::iterator it = parameters.begin();
    while (it != parameters.end())
    {
        if (it->au->getName() == audioUnitName) {
            parameters.erase(it);
        }
        else {
            ++it;
        }
    }
}

//--------------------------------------------------------------
void Selection::setValue(int idx, float value) {
    if (idx < parameters.size()) {
        parameters[idx].setValue(value);
    }
}

//--------------------------------------------------------------
void Selection::draw(int x, int y) {
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255);
    ofTranslate(x, y);
    for (int i=0; i<parameters.size(); i++) {
        ofDrawBitmapString(ofToString(i) + " :: " + parameters[i].name, 0, (i + 1) * 16);
    }
    ofPopStyle();
    ofPopMatrix();
}

//--------------------------------------------------------------
Section::Section(AudioUnitController *au, string audioUnitName, string sectionName, vector<AudioUnitParameterInfo> & parameters, ofxControlWidget *gui, Selection *selection) {
    this->au = au;
    this->gui = gui;
    this->audioUnitName = audioUnitName;
    this->sectionName = sectionName;
    this->selection = selection;
    vector<string> choices;
    for (auto p : parameters) {
        //if (p.flags == 3360686096) { // exponential
        choices.push_back(p.name);
    }
    menu = gui->addMenu(audioUnitName+" ("+sectionName+")", choices, this, &Section::eventSelectParameter, true);
    menu->setCollapsed(true);
}

//--------------------------------------------------------------
void Section::remove() {
    gui->removeElement(audioUnitName+" ("+sectionName+")");
}

//--------------------------------------------------------------
void Section::eventSelectParameter(ofxControlMenuEventArgs & e) {
    string name = e.toggle->getName();
    if (e.value) {
        selection->add(au, name);
    }
    else {
        selection->remove(au, name);
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    idxMixer = 0;
    currentMidiDevice = NULL;
    
    synthGui.setName("AudioUnits");
    synthGui.setPosition(0, 0);
    synthGui.setWidth(300);

    string synthCmdResult = ofSystem("auval -a");
    vector<string> unitNames;
    vector<string> synthList = ofSplitString(synthCmdResult, "\n");
    for (int i=6; i<synthList.size(); i++) {
        vector<string> line = ofSplitString(synthList[i], "  -  ");
        if (line.size() > 1) {
            string name = line[1];
            string auLine = ofTrim(line[0]);
            for (int s=0; s<3; s++) ofStringReplace(auLine, "  ", " ");
            vector<string> code = ofSplitString(auLine, " ");
            string makerType = ofToString(code[2])+"/"+ofToString(code[0]);
            if (unitMenus.count(makerType) == 0) {
                ofxControlMenu *auMenu = synthGui.addMenu("AudioUnits: "+makerType, this, &ofApp::selectAudioUnit, true, false);
                unitMenus[makerType] = auMenu;
            }
            unitMenus[makerType]->addToggle(name);
            unitNames.push_back(name);
            std::reverse(code[0].begin(),code[0].end());
            std::reverse(code[1].begin(),code[1].end());
            std::reverse(code[2].begin(),code[2].end());
            unitLookup[name] = code;
        }
    }
    
    mixer.connectTo(output);
    output.start();
    
    gui.setName("Available AudioUnit Parameters");
    gui.setPosition(310, 0);
    gui.setWidth(400);
    midiGui = gui.addMenu("Select MIDI Device", this, &ofApp::selectMidiDevice, false, false);

    addressRoot = DEFAULT_INPUT_ADDRESS;
    portIn = DEFAULT_PORT_IN;
    
    ofXml xml;
    xml.load("settings_audiounit.xml");
    xml.setTo("AudioUnitOSC");
    portIn = ofToInt(xml.getValue("port"));
    addressRoot = xml.getValue("address");
    
    osc.setup(portIn);
}

//--------------------------------------------------------------
void ofApp::addAudioUnit(string name) {
    vector<string> lut = unitLookup[name];
    AudioUnitController au;
    units[name] = au;
    units[name].setup(name, *(OSType*) lut[0].c_str(), *(OSType*) lut[1].c_str(), *(OSType*) lut[2].c_str());
    units[name].connectTo(mixer, idxMixer++);
    units[name].showUI();
    midiGui->addToggle(name);
    map<string, vector<AudioUnitParameterInfo> >::iterator it = units[name].getParameterGroups().begin();
    for (; it != units[name].getParameterGroups().end(); ++it) {
        Section *section = new Section(&units[name], name, it->first, units[name].getParameterGroups()[it->first], &gui, &selection);
        sections.push_back(section);
    }
}

//--------------------------------------------------------------
void ofApp::removeAudioUnit(string name) {
    vector<Section*>::iterator it = sections.begin();
    while (it != sections.end())
    {
        if ((*it)->audioUnitName == name) {
            selection.removeAudioUnit((*it)->audioUnitName);
            (*it)->remove();
            sections.erase(it);
        }
        else {
            ++it;
        }
    }
    midiGui->removeToggle(name);
    units.erase(name);
}

//--------------------------------------------------------------
void ofApp::selectAudioUnit(ofxControlMenuEventArgs & evt) {
    string name = evt.toggle->getName();
    bool toAdd = evt.value;
    if (units.count(name) > 0 && !toAdd) {
        removeAudioUnit(name);
    }
    else if (units.count(name) == 0 && toAdd) {
        addAudioUnit(name);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    while (osc.hasWaitingMessages()) {
        ofxOscMessage msg;
        osc.getNextMessage(msg);
        if (msg.getAddress() == addressRoot) {
            for (int i=0; i<msg.getNumArgs(); i++) {
                selection.setValue(i, msg.getArgAsFloat(i));
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(60);
    
    ofPushStyle();
    ofSetColor(255);
    ofDrawBitmapString("Parameters", 740, 20);
    ofDrawLine(730, 24, 1020, 24);
    ofPopStyle();

    selection.draw(730, 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    int note = 70;
    if (key=='q')         note = 70;
    else if (key=='w')    note = 72;
    else if (key=='e')    note = 74;
    else if (key=='r')    note = 75;
    else if (key=='t')    note = 77;
    else if (key=='y')    note = 79;
    else if (key=='u')    note = 81;
    else if (key=='i')    note = 82;
    if (currentMidiDevice != NULL) {
        currentMidiDevice->getSynth().midiNoteOn(note, 127);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    int note = 70;
    if (key=='q')         note = 70;
    else if (key=='w')    note = 72;
    else if (key=='e')    note = 74;
    else if (key=='r')    note = 75;
    else if (key=='t')    note = 77;
    else if (key=='y')    note = 79;
    else if (key=='u')    note = 81;
    else if (key=='i')    note = 82;
    if (currentMidiDevice != NULL) {
        currentMidiDevice->getSynth().midiNoteOff(note, 100);
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowShape(540, 360);
    ofSetWindowTitle("DarknetOSC");
    
    // initialize darknet
    string cfgfile = ofToDataPath("../../../../data/darknet/darknet.cfg");
    string weightfile = ofToDataPath("../../../../data/darknet/darknet.weights");
//    string cfgfile = ofToDataPath("darknet/darknet.cfg");
//    string weightfile = ofToDataPath("darknet/darknet.weights");
    darknet.init( cfgfile, weightfile );
    
    // setup camera
    cam.initGrabber(320, 240);

    oscDestination = OSC_DESTINATION_DEFAULT;
    oscAddressRoot = OSC_ADDRESS_ROOT_DEFAULT;
    oscPort = OSC_PORT_DEFAULT;
    
    // load settings from file
    ofXml xml;
    xml.load(ofToDataPath("settings_darknet.xml"));
    xml.setTo("DarknetOSC");
    oscDestination = xml.getValue("ip");
    oscPort = ofToInt(xml.getValue("port"));
    oscAddressRoot = xml.getValue("address");
//    bool sendClassificationsByDefault = (xml.getValue("sendClassificationsByDefault") == "1");
    
    // setup osc
    osc.setup(oscDestination, oscPort);
    sending = false;
    
    // setup gui
    gui.setup();
    gui.setName("DarknetOSC");
    gui.add(sending.set("sending", false));
//    gui.add(sendClassifications.setup("send classifications", sendClassificationsByDefault));
}

void ofApp::sendOsc() {
//    int idxLayer = sendClassifications ? darknet.getLayerNames().size() - 3 : darknet.getLayerNames().size() - 4;
//    float * activations = get_network_output_layer_gpu( darknet.getNetwork(), idxLayer);
//    auto layer = darknet.getNetwork().layers[idxLayer];
//    int numFeatures = layer.out_c * layer.out_h * layer.out_w;
//    msg.clear();
//    msg.setAddress(oscAddressRoot);
//    for (int i=0; i<numFeatures; i++) {
//        msg.addFloatArg(activations[i]);
//    }
//    osc.sendMessage(msg);

    classifications = darknet.classify(cam.getPixels(), 1000);

    msg.clear();
    msg.setAddress(oscAddressRoot);
    for (int i=0; i<classifications.size(); i++) {
        msg.addFloatArg(classifications[i].probability);
    }
    osc.sendMessage(msg);
}

void ofApp::update()
{
    cam.update();
    if (cam.isFrameNew() && sending) {
        sendOsc();
    }
}

void ofApp::draw()
{
    if (!darknet.isLoaded()) {
        ofDrawBitmapString("Network file not found!\nCheck your data folder to make sure it exists.", 20, 20);
        return;
    }
    
    if (sending) {
        ofBackground(0, 255, 0);
        
        ofSetColor(0, 100);
        ofDrawRectangle(10, 280, 520, 66);
        ofSetColor(255);
        string txt = "sending "+ofToString(sendClassifications ? "classification probabilities":"fc2 layer activations")+" ("+ofToString(msg.getNumArgs())+" values)\n";
        txt += "to "+oscDestination+", port "+ofToString(oscPort)+",\n";
        txt += "osc address \""+oscAddressRoot+"\"\n";
        txt += "press spacebar or click 'sending' to turn off sending.";
        ofDrawBitmapString(txt, 20, 296);
    }
    else {
        ofBackground(255, 0, 0);
        
        ofSetColor(0, 100);
        ofDrawRectangle(10, 280, 520, 66);
        ofSetColor(255);
        string txt = "press spacebar or click 'sending' to turn on sending.";
        ofDrawBitmapString(txt, 20, 296);
    }
    
    ofSetColor(255);
    cam.draw(210, 10);
    gui.draw();
}

void ofApp::keyPressed(int key) {
    if (key==' ') {
        sending = !sending;
    }
}

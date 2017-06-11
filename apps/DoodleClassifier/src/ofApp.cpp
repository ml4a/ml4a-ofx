#include "ofApp.h"

//--------------------------------------------------------------
void FoundSquare::draw() {
    img.draw(0, 0);
    string labelStr = "no class";
    labelStr = (isPrediction?"predicted: ":"assigned: ")+label;
    ofDrawBitmapStringHighlight(labelStr, 4, img.getHeight()-22);
    ofDrawBitmapStringHighlight("{"+ofToString(rect.x)+","+ofToString(rect.y)+","+ofToString(rect.width)+","+ofToString(rect.height)+"}, area="+ofToString(area), 4, img.getHeight()-5);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1600, 900);
    
    width = 640;
    height = 480;
    
    cam.setDeviceID(0);
    cam.setup(width, height);
    ccv.setup(ofToDataPath("../../../../data/image-net-2012.sqlite3"));
//    ccv.setup(ofToDataPath("image-net-2012.sqlite3"));
    
    bAdd.addListener(this, &ofApp::addSamplesToTrainingSetNext);
    bTrain.addListener(this, &ofApp::trainClassifier);
    bClassify.addListener(this, &ofApp::classifyNext);
    bSave.addListener(this, &ofApp::save);
    bLoad.addListener(this, &ofApp::load);
    trainingLabel.addListener(this, &ofApp::setTrainingLabel);
    
    // default settings
    oscDestination = DEFAULT_OSC_DESTINATION;
    oscAddress = DEFAULT_OSC_ADDRESS;
    oscPort = DEFAULT_OSC_PORT;
    
    // load settings from file
    ofXml xml;
    xml.load(ofToDataPath("settings_doodleclassifier.xml"));
    xml.setTo("DoodleOSC");
    oscDestination = xml.getValue("ip");
    oscPort = ofToInt(xml.getValue("port"));
    oscAddress = xml.getValue("address");
    if (xml.exists("classes") && xml.setTo("classes") && xml.exists("class[0]")) {
        xml.setTo("class[0]");
        classNames.clear();
        do {
            string newClass = xml.getValue();
            classNames.push_back(newClass);
        }
        while(xml.setToSibling());
    }

    sender.setup(oscDestination, oscPort);
    
    gui.setup();
    gui.setName("DoodleClassifier");
    ofParameterGroup gCv;
    gCv.setName("CV initial");
    gCv.add(minArea.set("Min area", 10, 1, 100));
    gCv.add(maxArea.set("Max area", 200, 1, 500));
    gCv.add(threshold.set("Threshold", 128, 0, 255));
    gCv.add(nDilate.set("Dilations", 1, 0, 8));
    gui.add(trainingLabel.set("Training Label", 0, 0, classNames.size()-1));
    gui.add(bAdd.setup("Add samples"));
    gui.add(bTrain.setup("Train"));
    gui.add(bRunning.setup("Run", false));
    gui.add(bClassify.setup("Classify"));
    gui.add(bSave.setup("Save"));
    gui.add(bLoad.setup("Load"));
    gui.add(gCv);
    gui.setPosition(0, 400);
    gui.loadFromFile("settings_doodleclassifier_cv.xml");
    
    fbo.allocate(width, height);
    colorImage.allocate(width, height);
    grayImage.allocate(width, height);
    isTrained = false;
    toAddSamples = false;
    toClassify = false;
    
    trainingData.setNumDimensions(4096);
    AdaBoost adaboost;
    adaboost.enableNullRejection(false);
    adaboost.setNullRejectionCoeff(3);
    pipeline.setClassifier(adaboost);
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if(cam.isFrameNew())
    {
        // get grayscale image and threshold
        colorImage.setFromPixels(cam.getPixels());
        grayImage.setFromColorImage(colorImage);
        for (int i=0; i<nDilate; i++) {
            grayImage.erode_3x3();
        }
        grayImage.threshold(threshold);
        //grayImage.invert();
        
        // find initial contours
        contourFinder.setMinAreaRadius(minArea);
        contourFinder.setMaxAreaRadius(maxArea);
        contourFinder.setThreshold(127);
        contourFinder.findContours(grayImage);
        contourFinder.setFindHoles(true);
        
        // draw all contour bounding boxes to FBO
        fbo.begin();
        ofClear(0, 255);
        ofFill();
        ofSetColor(255);
        for (int i=0; i<contourFinder.size(); i++) {
            //cv::Rect rect = contourFinder.getBoundingRect(i);
            //ofDrawRectangle(rect.x, rect.y, rect.width, rect.height);
            ofBeginShape();
            for (auto p : contourFinder.getContour(i)) {
                ofVertex(p.x, p.y);
            }
            ofEndShape();
        }
        fbo.end();
        ofPixels pixels;
        fbo.readToPixels(pixels);
        
        // find merged contours
        contourFinder2.setMinAreaRadius(minArea);
        contourFinder2.setMaxAreaRadius(maxArea);
        contourFinder2.setThreshold(127);
        contourFinder2.findContours(pixels);
        contourFinder2.setFindHoles(true);
        
        if (toAddSamples) {
            addSamplesToTrainingSet();
            toAddSamples = false;
        }
        else if (isTrained && (bRunning || toClassify)) {
            classifyCurrentSamples();
            toClassify = false;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(70);
    
    ofPushMatrix();
    ofScale(0.75, 0.75);
    
    // original
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(0, 20);
    cam.draw(0, 0);
    ofDrawBitmapStringHighlight("original", 0, 0);
    ofPopMatrix();
    ofPopStyle();
    
    // thresholded
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(width, 20);
    grayImage.draw(0, 0);
    ofSetColor(0, 255, 0);
    contourFinder.draw();
    ofDrawBitmapStringHighlight("thresholded", 0, 0);
    ofPopMatrix();
    ofPopStyle();
    
    // merged
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(2*width, 20);
    fbo.draw(0, 0);
    ofSetColor(0, 255, 0);
    contourFinder2.draw();
    ofDrawBitmapStringHighlight("merged", 0, 0);
    ofPopMatrix();
    ofPopStyle();
    
    ofPopMatrix();
    
    // draw tiles
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(210, 0.75*height+25);
    int nPerRow = max(5, (int) ceil(foundSquares.size()/2.0));
    ofTranslate(-ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, max(0,nPerRow-5)*226), 0);
    for (int i=0; i<foundSquares.size(); i++) {
        ofPushMatrix();
        ofTranslate(226*(i%nPerRow), 240*floor(i/nPerRow));
        foundSquares[i].draw();
        ofPopMatrix();
    }
    ofPopMatrix();
    ofPopStyle();
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit() {
    gui.saveToFile(ofToDataPath("settings_doodleclassifier_cv.xml"));
}

//--------------------------------------------------------------
void ofApp::gatherFoundSquares() {
    foundSquares.clear();
    for (int i=0; i<contourFinder2.size(); i++) {
        FoundSquare fs;
        fs.rect = contourFinder2.getBoundingRect(i);
        fs.area = contourFinder2.getContourArea(i);
        fs.img.setFromPixels(cam.getPixels());
        fs.img.crop(fs.rect.x, fs.rect.y, fs.rect.width, fs.rect.height);
        fs.img.resize(224, 224);
        foundSquares.push_back(fs);
    }
}

//--------------------------------------------------------------
void ofApp::addSamplesToTrainingSet() {
    ofLog(OF_LOG_NOTICE, "Adding samples...");
    gatherFoundSquares();
    for (int i=0; i<foundSquares.size(); i++) {
        foundSquares[i].label = classNames[trainingLabel];
        vector<float> encoding = ccv.encode(foundSquares[i].img, ccv.numLayers()-1);
        VectorFloat inputVector(encoding.size());
        for (int i=0; i<encoding.size(); i++) inputVector[i] = encoding[i];
        trainingData.addSample(trainingLabel, inputVector);
        ofLog(OF_LOG_NOTICE, " Added sample #"+ofToString(i)+" label="+ofToString(trainingLabel));
    }
}

//--------------------------------------------------------------
void ofApp::trainClassifier() {
    ofLog(OF_LOG_NOTICE, "Training...");
    if (pipeline.train(trainingData)){
        ofLog(OF_LOG_NOTICE, "getNumClasses: "+ofToString(pipeline.getNumClasses()));
    }
    isTrained = true;
    ofLog(OF_LOG_NOTICE, "Done training...");
}

//--------------------------------------------------------------
void ofApp::classifyCurrentSamples() {
    ofLog(OF_LOG_NOTICE, "Classifiying on frame "+ofToString(ofGetFrameNum()));
    gatherFoundSquares();
    for (int i=0; i<foundSquares.size(); i++) {
        vector<float> encoding = ccv.encode(foundSquares[i].img, ccv.numLayers()-1);
        VectorFloat inputVector(encoding.size());
        for (int i=0; i<encoding.size(); i++) inputVector[i] = encoding[i];
        if (pipeline.predict(inputVector)) {
            // gt classification
            int label = pipeline.getPredictedClassLabel();
            foundSquares[i].isPrediction = true;
            foundSquares[i].label = classNames[label];

            // send over OSC
            ofxOscMessage m;
            m.setAddress(oscAddress);
            m.addStringArg(foundSquares[i].label);
            sender.sendMessage(m, false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::setTrainingLabel(int & label_) {
    trainingLabel.setName(classNames[label_]);
}

//--------------------------------------------------------------
void ofApp::save() {
    trainingData.save(ofToDataPath("doodleclassifier_model.grt"));
}

//--------------------------------------------------------------
void ofApp::load() {
    trainingData.load(ofToDataPath("doodleclassifier_model.grt"));
    trainClassifier();
}

//--------------------------------------------------------------
void ofApp::classifyNext() {
    toClassify = true;
}

//--------------------------------------------------------------
void ofApp::addSamplesToTrainingSetNext() {
    toAddSamples = true;
}

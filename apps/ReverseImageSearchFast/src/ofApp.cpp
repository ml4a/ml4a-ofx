#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1440, 800);
    
    cam.initGrabber(640, 480);
    ccv.setup("/Users/gene/Code/of_v0.9.6_osx_release/apps/ofxaddons-bingo/libccv_instagram/bin/data/image-net-2012.sqlite3");
    
    bExtractDir.addListener(this, &ofApp::extractMSCOCO);
    bAnalyze.addListener(this, &ofApp::runPCAonImageSet);
    bKDTree.addListener(this, &ofApp::runKDTree);
    bSave.addListener(this, &ofApp::saveDefault);
    bLoad.addListener(this, &ofApp::loadDefault);
    

    gui.setup("Reverse Image Search");
    gui.add(headerHeight.set("header height", 320, 100, 480));
    gui.add(thumbHeight.set("thumb height", 240, 90, 420));
    
    gui.add(numPCAcomponents.set("num PCA components", 1024, 64, 1500));
    gui.add(maxPCASamples.set("max PCA rows", 75000, 10, 100000));
    gui.add(numResults.set("num results", 60, 5, 100));
    gui.add(tRecord.set("recording", false));
    
    
    gui.add(bExtractDir.setup("extract dir"));
    gui.add(bAnalyze.setup("run pca"));
    gui.add(bKDTree.setup("build kd-tree"));
    gui.add(bSave.setup("save"));
    gui.add(bLoad.setup("load"));
    
    gui.setPosition(ofGetWidth()-200, 0);
    
    loadDefault();
    runKDTree();
}

//--------------------------------------------------------------
void ofApp::update(){
    if (tRecord) {
        analyzeWebcam();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    if (activeImage.isAllocated()) {
        drawResults();
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::drawResults(){
    float margin = 3;
    int numRows = floor((ofGetHeight()-headerHeight)/(thumbHeight+margin));
    float y = ofGetHeight() - numRows * (thumbHeight + margin);
    float x = margin;
    activeImage.draw(200, 0);
    ofDrawBitmapStringHighlight("Nearest neighbor images:", x + margin, y - margin);
    for (int i=1; i<resultImages.size(); i++) {
        resultImages[i-1].draw(x, y);
        x += (margin + resultImages[i-1].getWidth());
        if (x > (ofGetWidth() - resultImages[i].getWidth()*0.33)) {
            y += thumbHeight + margin;
            x = margin;
        }
    }
}

//--------------------------------------------------------------
void ofApp::analyzeWebcam() {
    cam.update();
    if (cam.isFrameNew()) {
        activeImage.setFromPixels(cam.getPixels());
        activeEncoding = ccv.encode(activeImage, ccv.numLayers()-1);
        queryResults();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeImage() {
    int idx = floor(ofRandom(images.size()));
    activeImage.load(images[idx].filename);
    if (images[idx].encoding.size() == 0) {
        images[idx].encoding = ccv.encode(activeImage, ccv.numLayers()-1);
    }
    activeEncoding = images[idx].encoding;
    queryResults();
}

//--------------------------------------------------------------
void ofApp::queryResults() {
    vector<double> encoding2;
    for (int i=0; i<activeEncoding.size(); i++) {
        encoding2.push_back(activeEncoding[i]);
    }
    vector<double> projectedEncoding2 = pca.project(encoding2);
    kdTree.getKNN(projectedEncoding2, numResults, indexes, distances);
    resultImages.resize(numResults-1);
    for (int i=1; i<numResults; i++) {
        resultImages[i-1].load(images[indexes[i]].filename);
        resultImages[i-1].resize(thumbHeight * resultImages[i-1].getWidth() / resultImages[i-1].getHeight(), thumbHeight);
    }
    if (activeImage.getHeight() > headerHeight) {
        activeImage.resize(headerHeight * activeImage.getWidth() / activeImage.getHeight(), headerHeight);
    }
}

//--------------------------------------------------------------
void ofApp::getImagePathsRecursive(ofDirectory dir){
    const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};
    ofDirectory new_dir;
    int size = dir.listDir();
    for (int i = 0; i < size; i++){
        if (dir.getFile(i).isDirectory()){
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            getImagePathsRecursive(new_dir);
        }
        else if (find(begin(allowed_ext), end(allowed_ext), ofToLower(dir.getFile(i).getExtension())) != end(allowed_ext)) {
            candidateFiles.push_back(dir.getFile(i));
        }
    }
}

//--------------------------------------------------------------
void ofApp::extractFeaturesForDirectory(string directory) {
    ofLog() << "Gathering images recursively from "+directory;
    candidateFiles.clear();
    ofDirectory dir = ofDirectory(directory);
    getImagePathsRecursive(dir);
    int numImages = 650;//candidateFiles.size();
    for(int i=0; i<numImages; i++) {
        if (i % 200 == 0) ofLog() << "extracting features for image "<<i<<"/"<<numImages;
        bool success = activeImage.load(candidateFiles[i]);
        if (success) {
            Image image;
            image.filename = candidateFiles[i].getAbsolutePath();
            image.encoding = ccv.encode(activeImage, ccv.numLayers()-1);
            images.push_back(image);
        }
        else {
            ofLog(OF_LOG_ERROR, "Failed to load image: "+candidateFiles[i].getAbsolutePath());
        }
    }
    ofLog() << "finished extracting features for "<<images.size()<<" images.";
}

//--------------------------------------------------------------
void ofApp::runPCAonImageSet(){
    vector<int> pcaIndexes;
    for (int i=0; i<images.size(); i++) {pcaIndexes.push_back(i);}
    if (maxPCASamples < images.size()) {
        random_shuffle(pcaIndexes.begin(), pcaIndexes.end());
        pcaIndexes.resize(maxPCASamples);
    }
    
    for (int i=0; i<pcaIndexes.size(); i++) {
        if (i%200==0) ofLog() << "copying encoding for image "<<i<<"/"<<pcaIndexes.size();
        int idx = pcaIndexes[i];
        vector<double> sample(images[idx].encoding.begin(), images[idx].encoding.end());
        pca.addSample(sample);
    }
    
    ofLog() << "Run PCA";
    int startTime = ofGetElapsedTimef();
    pca.pca(numPCAcomponents);
    ofLog() << "Finished PCA in "<<(ofGetElapsedTimef() - startTime)<<" sec";
    
    ofLog() << "Project original samples into reduced space";
    for (int i=0; i<images.size(); i++) {
        if (i%200==0) ofLog() << "getting PCA-projected encoding for image "<<i<<"/"<<images.size();
        vector<double> sample(images[i].encoding.begin(), images[i].encoding.end());
        images[i].projectedEncoding = pca.project(sample);
    }
    ofLog() << "finished extracting projected encodings for "<<images.size()<<" images.";
}

//--------------------------------------------------------------
void ofApp::runKDTree() {
    kdTree.clear();
    for (int i=0; i<images.size(); i++) {
        if (i%2000==0) ofLog() << "kd-tree: adding image "<<i<<"/"<<images.size();
        kdTree.addPoint(images[i].projectedEncoding);
    }
    ofLog() << "build kd-tree" << endl;
    int startTime = ofGetElapsedTimef();
    kdTree.constructKDTree();
    ofLog() << "finished constructiong kd-tree for "<<images.size()<<" images in "<<(ofGetElapsedTimef() - startTime)<<" sec";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        analyzeImage();
    }
}

//--------------------------------------------------------------
void ofApp::save(string path) {
    const char *filepath = path.c_str();
    ofstream fout(filepath, ios::binary);
    vector<vector<double> > projectedEncodings;
    vector<string> filenames;
    for (auto image : images) {
        projectedEncodings.push_back(image.projectedEncoding);
        filenames.push_back(image.filename);
    }
    dlib::serialize(projectedEncodings, fout);
    dlib::serialize(filenames, fout);
    dlib::serialize(pca.getE(), fout);
    dlib::serialize(pca.getV(), fout);
    dlib::serialize(pca.getColumnMeans(), fout);
}

//--------------------------------------------------------------
void ofApp::load(string path) {
    const char *filepath = path.c_str();
    ifstream fin(filepath, ios::binary);
    vector<vector<double> > projectedEncodings;
    vector<string> filenames;
    vector<double> column_means;
    dlib::matrix<double, 0, 0> E, V;
    dlib::deserialize(projectedEncodings, fin);
    dlib::deserialize(filenames, fin);
    dlib::deserialize(E, fin);
    dlib::deserialize(V, fin);
    dlib::deserialize(column_means, fin);
    pca.setE(E);
    pca.setV(V);
    pca.setColumnMeans(column_means);
    images.clear();
    for (int i=0; i<filenames.size(); i++) {
        Image image;
        image.filename = filenames[i];
        image.projectedEncoding = projectedEncodings[i];
        images.push_back(image);
    }
}

//--------------------------------------------------------------
void ofApp::saveKDTree(string path) {
    kdTree.save(path);
}

//--------------------------------------------------------------
void ofApp::loadKDTree(string path) {
    kdTree.clear();
    for (auto image : images) {
        kdTree.addPoint(image.projectedEncoding);
    }
    kdTree.load(path);
}

//--------------------------------------------------------------
void ofApp::saveDefault() {
    save(ofToDataPath("data_vecs50k.dat"));
}

//--------------------------------------------------------------
void ofApp::loadDefault() {
    load(ofToDataPath("data_vecs50k.dat"));
}

//--------------------------------------------------------------
void ofApp::extractMSCOCO() {
    string folder = "/Users/gene/Teaching/ML4A/ml4a-ofx/apps/ReverseImageSearchFast/bin/data/mscoco";
    extractFeaturesForDirectory(folder);
}

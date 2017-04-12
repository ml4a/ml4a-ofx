#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1280, 720);

    ris.setup("/Users/gene/Code/of_v0.9.6_osx_release/apps/ofxaddons-bingo/libccv_instagram/bin/data/image-net-2012.sqlite3");
    ris.load("/Users/gene/bin/ml4a/ReverseImageSearch/data_vecs145k.dat");
//    ris.load("/Users/gene/bin/ml4a/ReverseImageSearch/data_small.dat");
    ris.runKDTree();
    
    tScreenDebug.addListener(this, &ofApp::toggleScreenGrabDebug);
    tWebcam.addListener(this, &ofApp::enableWebcam);
    tVideo.addListener(this, &ofApp::enableVideo);
    tScreen.addListener(this, &ofApp::enableScreenGrab);
    bSampleImage.addListener(this, &ofApp::analyzeImage);
    tRunning.addListener(this, &ofApp::toggleRunning);
    
    ris.maxPCASamples.set("max PCA samples", 50000, 10, 100000);
    ris.numPCAcomponents.set("num PCA components", 1024, 64, 1500);
    ris.bExtractDir.setup("analyze directory");
    ris.bSave.setup("save");
    ris.bLoad.setup("load");
    
    guiOptions.setup();
    guiOptions.setName("Options");
    guiOptions.setPosition(ofGetWidth()-200, 0);
    guiOptions.add(ris.numPCAcomponents.set("num PCA components", 1024, 64, 1500));
    guiOptions.add(ris.bExtractDir.setup("analyze directory"));
    guiOptions.add(ris.bSave.setup("save"));
    guiOptions.add(ris.bLoad.setup("load"));
    
    guiView.setup();
    guiView.setName("View");
    guiView.setPosition(0, 0);
    guiView.add(tRunning.setup("running?", false));
    guiView.add(headerHeight.set("header height", 320, 100, 480));
    guiView.add(thumbHeight.set("thumb height", 240, 90, 420));
    guiView.add(ris.numResults.set("num results", 32, 5, 100));
    guiView.add(bSampleImage.setup("query random image"));
    guiView.add(tWebcam.set("query webcam", false));
    guiView.add(tVideo.set("query video", false));
    guiView.add(tScreen.set("query screengrab", false));
    guiView.add(tScreenDebug.set(" > set screengrab window", false));
    
    screen.setup(ofGetWidth()-17, ofGetHeight()-10, true);
}

void ofApp::toggleRunning(bool & running) {
    if (running) {
        ris.startThread();
    } else {
        ris.stopThread();
    }
}

//--------------------------------------------------------------
ReverseImageSearchThreaded::ReverseImageSearchThreaded() {
    ready = true;
    isNew = false;
    hasImage = false;
    
    bExtractDir.addListener(this, &ReverseImageSearchThreaded::extractDirectory);
    bSave.addListener(this, &ReverseImageSearchThreaded::saveDialog);
    bLoad.addListener(this, &ReverseImageSearchThreaded::loadDialog);
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::update(ofBaseHasPixels &pix, int layer) {
    this->layer = layer;
    ready = false;
    hasImage = true;
    img.setFromPixels(pix.getPixels());
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::threadedFunction() {
    while(isThreadRunning()) {
        if(lock()) {
            if (!ready && hasImage) {
                encoding = encode(img, layer);
                
                vector<double> encodingD;
                for (int i=0; i<encoding.size(); i++) {encodingD.push_back((double) encoding[i]);}
                vector<double> projectedEncoding = pca.project(encodingD);
                kdTree.getKNN(projectedEncoding, numResults, indexes, distances);
                
                results.clear();
                results.resize(numResults);
                for (int i=0; i<numResults; i++) {
                    results[i] = images[indexes[i]].filename;
                }
                
                ready = true;
                isNew = true;
            }
            unlock();
        } else {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

//--------------------------------------------------------------
bool ReverseImageSearchThreaded::hasNewResults() {
    if (isNew) {
        isNew = false;
        return true;
    }
    else {
        return false;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (tWebcam) {
        analyzeWebcam();
    } else if (tScreen) {
        analyzeScreen();
    } else if (tVideo) {
        analyzeVideo();
    }
    
    if (ris.isReady()) {
        ris.update(activeImage, ris.numLayers()-1);
    }
    
    if (ris.hasNewResults()) {
        vector<string> & results = ris.getResults();
        resultImages.resize(results.size());
        for (int i=0; i<results.size(); i++) {
            resultImages[i].load(results[i]);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    if (activeImage.isAllocated()) {
        if (screen.isDebug()) {
            screen.drawDebug();
        } else {
            drawResults();
        }
    }
    guiOptions.draw();
    guiView.draw();
}

//--------------------------------------------------------------
void ofApp::drawResults(){
    float margin = 3;
    int numRows = floor((ofGetHeight()-headerHeight)/(thumbHeight+margin));
    float y = ofGetHeight() - numRows * (thumbHeight + margin);
    float x = margin;
    activeImage.draw(205, 0, headerHeight * activeImage.getWidth() / activeImage.getHeight(), headerHeight);
    for (int i=0; i<resultImages.size(); i++) {
        float thumbWidth = thumbHeight * resultImages[i].getWidth() / resultImages[i].getHeight();
        resultImages[i].draw(x, y, thumbWidth, thumbHeight);
        x += (margin + thumbWidth);
        if (x > (ofGetWidth() - thumbWidth * 0.33)) {
            y += thumbHeight + margin;
            x = margin;
        }
    }
    ofDrawBitmapStringHighlight("Query image", 210, 13);
    ofDrawBitmapStringHighlight("Nearest neighbor images:", margin + 5, ofGetHeight() - numRows * (thumbHeight + margin) + 13);
}

//--------------------------------------------------------------
void ofApp::analyzeImage() {
    tWebcam = false;
    tScreen = false;
    tVideo = false;
    int idx = floor(ofRandom(ris.getImages().size()));
    activeImage.load(ris.getImages()[idx].filename);
    ris.update(activeImage, ris.numLayers()-1);
    if (!tRunning) {
        tRunning = true;
        ris.startThread();
    }
}

//--------------------------------------------------------------
void ofApp::enableWebcam(bool & enable) {
    if (!enable) return;
    tScreen = false;
    tVideo = false;
    cam.initGrabber(640, 480);
    movie.close();
    if (!tRunning) {
        tRunning = true;
        ris.startThread();
    }
}

//--------------------------------------------------------------
void ofApp::enableScreenGrab(bool & enable) {
    if (!enable) return;
    tWebcam = false;
    tVideo = false;
    cam.close();
    movie.close();
    if (!tRunning) {
        tRunning = true;
        ris.startThread();
    }
}

//--------------------------------------------------------------
void ofApp::enableVideo(bool & enable) {
    if (!enable) return;
    tScreen = false;
    tWebcam = false;
    cam.close();
    ofFileDialogResult result = ofSystemLoadDialog("Load a movie");
    movie.load(result.getPath());
    movie.play();
    if (!tRunning) {
        tRunning = true;
        ris.startThread();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeWebcam() {
    cam.update();
    if (cam.isFrameNew()) {
        activeImage.setFromPixels(cam.getPixels());
    }
}

//--------------------------------------------------------------
void ofApp::analyzeVideo() {
    movie.update();
    if (movie.isFrameNew()) {
        activeImage.setFromPixels(movie.getPixels());
    }
}

//--------------------------------------------------------------
void ofApp::analyzeScreen() {
    screen.update();
    if (!screen.isDebug()) {
        screen.getGrabber().getTextureReference().readToPixels(screenPixels);
        screenPixels.setImageType(OF_IMAGE_COLOR);
        activeImage.setFromPixels(screenPixels);
    }
}

//--------------------------------------------------------------
void ofApp::toggleScreenGrabDebug(bool & debug) {
    screen.setDebug(debug);
    if (debug && !tScreen) {
        tScreen = true;
        enableScreenGrab((bool&)tScreen);
    }
    if (!screen.isDebug()) {
        screenPixels.allocate(screen.getGrabber().getTextureReference().getWidth(),
                              screen.getGrabber().getTextureReference().getHeight(), 4);
    }
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::getImagePathsRecursive(ofDirectory dir){
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
void ReverseImageSearchThreaded::extractFeaturesForDirectory(string directory) {
    ofLog() << "Gathering images recursively from "+directory;
    candidateFiles.clear();
    ofDirectory dir = ofDirectory(directory);
    getImagePathsRecursive(dir);
    int numImages = candidateFiles.size();
    for(int i=0; i<numImages; i++) {
        if (i % 200 == 0) ofLog() << "extracting features for image "<<i<<"/"<<numImages;
        bool success = img.load(candidateFiles[i]);
        if (success) {
            Image image;
            image.filename = candidateFiles[i].getAbsolutePath();
            image.encoding = encode(img, numLayers()-1);
            images.push_back(image);
        }
        else {
            ofLog(OF_LOG_ERROR, "Failed to load image: "+candidateFiles[i].getAbsolutePath());
        }
    }
    ofLog() << "finished extracting features for "<<images.size()<<" images.";
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::runPCAonImageSet(){
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
void ReverseImageSearchThreaded::runKDTree() {
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
void ReverseImageSearchThreaded::save(string path) {
    ofLog()<<"Saving to "<<path;
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
    ofLog()<<"Saved "<<images.size()<<" image vectors to "<<path<<endl;
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::load(string path) {
    ofLog()<<"Loading from "<<path;
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
    ofLog()<<"Loaded "<<images.size()<<" image vectors "<<path<<endl;
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::saveKDTree(string path) {
    kdTree.save(path);
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::loadKDTree(string path) {
    kdTree.clear();
    for (auto image : images) {
        kdTree.addPoint(image.projectedEncoding);
    }
    kdTree.load(path);
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::saveDialog() {
    if (ofGetFrameNum() == 0) return;
    ofFileDialogResult result = ofSystemSaveDialog("data.dat", "Where to save saved features");
    if (result.bSuccess) {
        string path = result.getPath();
        save(path);
    }
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::loadDialog() {
    if (ofGetFrameNum() == 0) return;
    ofFileDialogResult result = ofSystemLoadDialog("Load saved feature vectors");
    if (result.bSuccess) {
        string path = result.getPath();
        load(path);
        runKDTree();
    }
}

//--------------------------------------------------------------
void ReverseImageSearchThreaded::extractDirectory() {
    if (ofGetFrameNum() == 0) return;
    ofFileDialogResult result = ofSystemLoadDialog("Which directory to scan?", true);
    if (result.bSuccess) {
        string folder = result.getPath();
        extractFeaturesForDirectory(folder);
        runPCAonImageSet();
        runKDTree();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    screen.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    screen.mouseDragged(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    screen.mousePressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    screen.mouseReleased(x, y);
}


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1440, 800);
    
    maxSamples.set("max samples", 50000, 10, 100000);
    bExtractDir.addListener(this, &ofApp::extractDirectory);
    bSave.addListener(this, &ofApp::saveDialog);
    bLoad.addListener(this, &ofApp::loadDialog);
    tScreenDebug.addListener(this, &ofApp::toggleScreenGrabDebug);
    tWebcam.addListener(this, &ofApp::enableWebcam);
    tVideo.addListener(this, &ofApp::enableVideo);
    tScreen.addListener(this, &ofApp::enableScreenGrab);
    tRetina.addListener(this, &ofApp::toggleMBPRetina);
    bSampleImage.addListener(this, &ofApp::analyzeImage);
    bSampleImage.addListener(this, &ofApp::analyzeImage);
    
    guiOptions.setup();
    guiOptions.setName("Options");
    guiOptions.setPosition(ofGetWidth()-200, 0);
    guiOptions.add(numComponents.set("dimensionality", 100, 2, 1500));
    guiOptions.add(bExtractDir.setup("analyze directory"));
    guiOptions.add(bSave.setup("save"));
    guiOptions.add(bLoad.setup("load"));

    guiView.setup();
    guiView.setName("View");
    guiView.setPosition(0, 0);
    guiView.add(headerHeight.set("header height", 320, 100, 480));
    guiView.add(thumbHeight.set("thumb height", 232, 90, 420));
    guiView.add(numResults.set("num results", 20, 5, 100));
    guiView.add(bSampleImage.setup("query random image"));
    guiView.add(tWebcam.set("query webcam", false));
    guiView.add(tVideo.set("query video", false));
    guiView.add(tScreen.set("query screengrab", false));
    guiView.add(tScreenDebug.set(" > set screengrab window", false));
    guiView.add(tRetina.set(" > MacBook Retina?", true));

    screen.setup(ofGetWidth()-17, ofGetHeight()-10, tRetina);
    ccv.setup(ofToDataPath("../../../../data/image-net-2012.sqlite3"));
//    ccv.setup(ofToDataPath("image-net-2012.sqlite3"));
    
//    baseDir = ofToDataPath("../../../../data/mscoco");
//    load(ofToDataPath("../../../../data/feature_vectors/mscoco_images_145k_rp100.dat"), baseDir);
//    runKDTree();
}

//--------------------------------------------------------------
void ofApp::toggleMBPRetina(bool & retina) {
    screen.setup(ofGetWidth()-17, ofGetHeight()-10, tRetina);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (toExtract) {
        extractFeaturesForDirectory(toExtractDir);
        runDimReduction();
        runKDTree();
        toExtract = false;
    }
    if (toLoad) {
        ofDrawBitmapStringHighlight("Loading saved vectors", 210, 13, ofColor(255,0,0));
        load(toLoadModel, baseDir);
        runKDTree();
        toLoad = false;
    }

    if (tWebcam) {
        analyzeWebcam();
    } else if (tScreen) {
        analyzeScreen();
    } else if (tVideo) {
        analyzeVideo();
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
    for (int i=1; i<resultImages.size(); i++) {
        float thumbWidth = thumbHeight * resultImages[i-1].getWidth() / resultImages[i-1].getHeight();
        resultImages[i-1].draw(x, y, thumbWidth, thumbHeight);
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
void ofApp::enableWebcam(bool & enable) {
    if (!enable) return;
    tScreen = false;
    tVideo = false;
    cam.initGrabber(640, 480);
    movie.close();
}

//--------------------------------------------------------------
void ofApp::enableScreenGrab(bool & enable) {
    if (!enable) return;
    tWebcam = false;
    tVideo = false;
    cam.close();
    movie.close();
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
void ofApp::analyzeVideo() {
    movie.update();
    if (movie.isFrameNew()) {
        activeImage.setFromPixels(movie.getPixels());
        activeEncoding = ccv.encode(activeImage, ccv.numLayers()-1);
        queryResults();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeScreen() {
    screen.update();
    if (!screen.isDebug()) {
        screen.getGrabber().getTextureReference().readToPixels(screenPixels);
        screenPixels.setImageType(OF_IMAGE_COLOR);
        activeImage.setFromPixels(screenPixels);
        activeEncoding = ccv.encode(activeImage, ccv.numLayers()-1);
        queryResults();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeImage() {
    tWebcam = false;
    tScreen = false;
    tVideo = false;
    cam.close();
    int idx = floor(ofRandom(images.size()));
    activeImage.load(baseDir+"/"+images[idx].filename);
    if (images[idx].encoding.size() == 0) {
        images[idx].encoding = ccv.encode(activeImage, ccv.numLayers()-1);
    }
    activeEncoding = images[idx].encoding;
    queryResults();
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
void ofApp::queryResults() {
    vector<double> encoding;
    for (int i=0; i<activeEncoding.size(); i++) {encoding.push_back(activeEncoding[i]);}
    vector<double> projectedEncoding = rp.project(encoding);
    kdTree.getKNN(projectedEncoding, numResults, indexes, distances);
    resultImages.resize(numResults-1);
    for (int i=1; i<numResults; i++) {
        resultImages[i-1].load(baseDir+"/"+images[indexes[i]].filename);
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
    baseDir = directory;
    ofLog() << "Gathering images recursively from "+directory;
    candidateFiles.clear();
    ofDirectory dir = ofDirectory(directory);
    getImagePathsRecursive(dir);
    int numImages = candidateFiles.size();
    for(int i=0; i<numImages; i++) {
        if (i % 200 == 0) ofLog() << "extracting features for image "<<i<<"/"<<numImages;
        if (i % 10000 == 0) {
            //optional: save feature vectors as you go, in case extraction gets interrupted so you can retrieve later
            //save(ofToDataPath("mscoco_image_vectors_current.dat"), true);
        }
        string imagePath = candidateFiles[i].getAbsolutePath();
        ofStringReplace(imagePath, dir.getAbsolutePath(), "");
        if (imageMap.count(imagePath)) {
            continue;
        }
        bool success = activeImage.load(candidateFiles[i]);
        if (success) {
            Image image;
            image.filename = imagePath;
            image.encoding = ccv.encode(activeImage, ccv.numLayers()-1);
            images.push_back(image);
            imageMap[image.filename] = true;
        }
        else {
            ofLog(OF_LOG_ERROR, "Failed to load image: "+candidateFiles[i].getAbsolutePath());
        }
    }
    ofLog() << "finished extracting features for "<<images.size()<<" images.";
}

//--------------------------------------------------------------
void ofApp::runDimReduction(){
    vector<int> indexes;
    for (int i=0; i<images.size(); i++) {indexes.push_back(i);}
//    if (maxSamples < images.size()) {
//        random_shuffle(pcaIndexes.begin(), pcaIndexes.end());
//        pcaIndexes.resize(maxSamples);
//    }
//
    rp.clearTrainingInstances();
    for (int i=0; i<indexes.size(); i++) {
        if (i%200==0) ofLog() << "copying encoding for image "<<i<<"/"<<indexes.size();
        int idx = indexes[i];
        vector<double> sample(images[idx].encoding.begin(), images[idx].encoding.end());
        rp.addSample(sample);
    }
    
    ofLog() << "Run dimensionality reduction";
    int startTime = ofGetElapsedTimef();
    rp.randomProjection(numComponents);
    //rp.pca(numComponents);
    ofLog() << "Finished dimensionality reduction in "<<(ofGetElapsedTimef() - startTime)<<" sec";
    
    
    ofLog() << "Project original samples into reduced space";
    for (int i=0; i<images.size(); i++) {
        if (i%200==0) ofLog() << "getting projected encoding for image "<<i<<"/"<<images.size();
        vector<double> sample(images[i].encoding.begin(), images[i].encoding.end());
        images[i].projectedEncoding = rp.project(sample);
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
void ofApp::save(string path, bool featuresOnly) {
    ofLog()<<"Saving to "<<path;
    const char *filepath = path.c_str();
    ofstream fout(filepath, ios::binary);
    vector<vector<double> > projectedEncodings;
    vector<vector<float> > encodings;
    vector<string> filenames;
    for (auto image : images) {
        if (featuresOnly) {
            encodings.push_back(image.encoding);
        }
        projectedEncodings.push_back(image.projectedEncoding);
        filenames.push_back(image.filename);
    }
    if (featuresOnly) {
        dlib::serialize(encodings, fout);
    }
    dlib::serialize(projectedEncodings, fout);
    dlib::serialize(filenames, fout);
    if (!featuresOnly) {
        //dlib::serialize(rp.getE(), fout);
        dlib::serialize(rp.getV(), fout);
        dlib::serialize(rp.getColumnMeans(), fout);
    }

    ofLog()<<"Saved "<<images.size()<<" image vectors to "<<path<<endl;
}

//--------------------------------------------------------------
void ofApp::load(string path, string baseDir, bool featuresOnly) {
    ofLog()<<"Loading from "<<path;
    const char *filepath = path.c_str();
    ifstream fin(filepath, ios::binary);
    vector<vector<double> > projectedEncodings;
    vector<vector<float> > encodings;
    vector<string> filenames;
    vector<double> column_means;
    dlib::matrix<double, 0, 0> E, V;
    if (featuresOnly) {
        dlib::deserialize(encodings, fin);
    }
    dlib::deserialize(projectedEncodings, fin);
    dlib::deserialize(filenames, fin);
    if (!featuresOnly) {
        //dlib::deserialize(E, fin);
        dlib::deserialize(V, fin);
        dlib::deserialize(column_means, fin);
        rp.setV(V);
        rp.setColumnMeans(column_means);
    }
    imageMap.clear();
    images.clear();
    for (int i=0; i<filenames.size(); i++) {
        Image image;
        image.filename = filenames[i];
        if (featuresOnly) {
            image.encoding = encodings[i];
        }
        image.projectedEncoding = projectedEncodings[i];
        images.push_back(image);
        imageMap[image.filename] = true;
    }
    ofLog()<<"Loaded "<<images.size()<<" image vectors "<<path<<endl;
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
void ofApp::saveDialog() {
    //save(ofToDataPath("data.dat"));
    ofFileDialogResult result = ofSystemSaveDialog("data.dat", "Where to save saved features");
    if (result.bSuccess) {
        string path = result.getPath();
        save(path);
    }
}

//--------------------------------------------------------------
void ofApp::loadDialog() {
    ofFileDialogResult result = ofSystemLoadDialog("Load saved feature vectors");
    ofFileDialogResult result2 = ofSystemLoadDialog("Where is the dataset located?", true);
    if (result.bSuccess) {
        toLoadModel = result.getPath();
        baseDir = result2.getPath();
        toLoad = true;
    }
}

//--------------------------------------------------------------
void ofApp::extractDirectory() {
    ofFileDialogResult result = ofSystemLoadDialog("Which directory to scan?", true);
    if (result.bSuccess) {
        toExtractDir = result.getPath();
        toExtract = true;
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


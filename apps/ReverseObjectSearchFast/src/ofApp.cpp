#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetWindowShape(1280, 720);
    ofSetFullscreen(true);
    ofSetBackgroundAuto(false);

    string cfgfile = ofToDataPath("../../../../data/darknet/yolo9000.cfg");
    string weightfile = ofToDataPath("../../../../data/darknet/yolo9000.weights");
    string nameslist = ofToDataPath("../../../../data/darknet/9k.names");
//    string cfgfile = ofToDataPath("darknet/yolo9000.cfg");
//    string weightfile = ofToDataPath("darknet/yolo9000.weights");
//    string nameslist = ofToDataPath("darknet/9k.names");
    darknet.init( cfgfile, weightfile, nameslist );
    
    maxSamples.set("max samples", 50000, 10, 100000);
    bExtractDir.addListener(this, &ofApp::extractDirectory);
    bSave.addListener(this, &ofApp::saveDialog);
    bLoad.addListener(this, &ofApp::loadDialog);
    tWebcam.addListener(this, &ofApp::enableWebcam);
    tVideo.addListener(this, &ofApp::enableVideo);
    bSampleImage.addListener(this, &ofApp::analyzeImage);
    
    guiOptions.setup();
    guiOptions.setName("Options");
    guiOptions.setPosition(ofGetWidth()-200, 0);
    guiOptions.add(numComponents.set("dimensionality", 100, 2, 1000));
    guiOptions.add(bExtractDir.setup("analyze directory"));
    guiOptions.add(bSave.setup("save vectors"));
    guiOptions.add(bLoad.setup("load vectors"));
    
    guiView.setup();
    guiView.setName("View");
    guiView.setPosition(0, 0);
    guiView.add(yoloThreshold.set("yolo threshold", 0.3, 0.0, 1.0));
    guiView.add(yoloMaxOverlap.set("max overlap", 0.5, 0.0, 1.0));
    guiView.add(headerHeight.set("header height", 200, 100, 480));
    guiView.add(thumbHeight.set("thumb height", 76, 60, 420));
    guiView.add(numResults.set("num results", 16, 5, 100));
    guiView.add(bSampleImage.setup("query random image"));
    guiView.add(tWebcam.set("query webcam", false));
    guiView.add(tVideo.set("query video", false));
    
}

//--------------------------------------------------------------
void ofApp::update(){
    // check if analyzing/loading
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

    // abalyze, display
    if (tWebcam) {
        analyzeWebcam();
    } else if (tVideo) {
        analyzeVideo();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    if (activeImage.isAllocated()) {
        drawResults();
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
    
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(210, 0);
    float z = headerHeight / activeImage.getHeight();
    activeImage.draw(0, 0, z * activeImage.getWidth(), headerHeight);
    for(detected_object d : detections ){
        ofSetColor( d.color );
        ofNoFill();
        ofRectangle rect = ofRectangle(z * d.rect.x, z * d.rect.y, z * d.rect.width, z * d.rect.height);
        ofDrawRectangle( rect );
        ofDrawBitmapStringHighlight( d.label + ": " + ofToString(d.probability), rect.x, rect.y+20 );
    }

    ofPopStyle();
    ofPopMatrix();
    
    for (int i=0; i<resultImages.size(); i++) {
        for (int j=0; j<resultImages[i].size(); j++) {
            float thumbWidth = thumbHeight * resultImages[i][j].getWidth() / resultImages[i][j].getHeight();
            resultImages[i][j].draw(x, y, thumbWidth, thumbHeight);
            x += (margin + thumbWidth);
        }
        y += thumbHeight + margin;
        x = margin;
    }
    
    ofDrawBitmapStringHighlight("Query image", 210, 13);
    ofDrawBitmapStringHighlight("Nearest neighbor images:", margin + 5, ofGetHeight() - numRows * (thumbHeight + margin) + 13);
}

//--------------------------------------------------------------
void ofApp::enableWebcam(bool & enable) {
    if (!enable) return;
    tVideo = false;
    cam.initGrabber(640, 480);
    movie.close();
}

//--------------------------------------------------------------
void ofApp::enableVideo(bool & enable) {
    if (!enable) return;
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
        detections = darknet.yolo(activeImage, yoloThreshold, yoloMaxOverlap);
        queryResults();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeVideo() {
    movie.update();
    if (movie.isFrameNew()) {
        activeImage.setFromPixels(movie.getPixels());
        detections = darknet.yolo(activeImage, yoloThreshold, yoloMaxOverlap);
        queryResults();
    }
}

//--------------------------------------------------------------
void ofApp::analyzeImage() {
    tWebcam = false;
    tVideo = false;
    int idx = floor(ofRandom(images.size()));
    activeImage.load(baseDir+"/"+images[idx].filename);
    detections = darknet.yolo(activeImage, yoloThreshold, yoloMaxOverlap);
    queryResults();
}

//--------------------------------------------------------------
void ofApp::queryResults() {
    resultImages.resize(detections.size());
    for (int i=0; i<detections.size(); i++) {
        vector<float> encoding = detections[i].features;
        vector<double> encodingD(encoding.begin(), encoding.end());
        vector<double> projectedEncoding = rp.project(encodingD);
        kdTree.getKNN(projectedEncoding, numResults, indexes, distances);
        resultImages[i].resize(numResults);
        for (int j=0; j<numResults; j++) {
            resultImages[i][j].load(baseDir+"/"+images[indexes[j]].filename);
            float x = images[indexes[j]].crop.getX();
            float y = images[indexes[j]].crop.getY();
            float w = images[indexes[j]].crop.getWidth();
            float h = images[indexes[j]].crop.getHeight();
            resultImages[i][j].crop(x, y, w, h);
        }
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
    ofLog() << "Gathering images recursively from "+baseDir;
    candidateFiles.clear();
    ofDirectory dir = ofDirectory(baseDir);
    getImagePathsRecursive(dir);
    int numImages = candidateFiles.size();
    for(int i=0; i<numImages; i++) {
        if (i % 200 == 0) ofLog() << "extracting features for image "<<i<<"/"<<numImages;
        if (i % 5000 == 0) {
            //optional: save feature vectors as you go, in case extraction gets interrupted so you can retrieve later
            //save(ofToDataPath("mscoco_object_vectors_"+ofToString(images.size())+".dat"), true);
        }
        string imagePath = candidateFiles[i].getAbsolutePath();
        ofStringReplace(imagePath, dir.getAbsolutePath(), "");
        if (imageMap.count(imagePath)) {
            continue;
        }
        bool success = activeImage.load(candidateFiles[i]);
        if (success) {
            detections = darknet.yolo(activeImage, yoloThreshold, yoloMaxOverlap);
            for (int j=0; j<detections.size(); j++) {
                Image image;
                image.filename = imagePath;
                image.encoding = detections[j].features;
                image.crop = detections[j].rect;
                image.probability = detections[j].probability;
                images.push_back(image);
                imageMap[image.filename] = true;
            }
        }
        else {
            ofLog(OF_LOG_ERROR, "Failed to load image: "+candidateFiles[i].getAbsolutePath());
        }
    }
    //save(ofToDataPath("feature_vectors_final.dat", true));
    ofLog() << "finished extracting features for "<<images.size()<<" objects from "<<imageMap.size()<<" images.";
}

//--------------------------------------------------------------
void ofApp::runDimReduction(){
    vector<int> indexes;
    for (int i=0; i<images.size(); i++) {indexes.push_back(i);}
//    if (maxSamples < images.size()) {
//        random_shuffle(indexes.begin(), indexes.end());
//        indexes.resize(maxSamples);
//    }
    
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
//    rp.pca(numComponents);
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
    vector<vector<float> > crops;
    vector<float> probabilities;
    vector<string> filenames;
    for (auto image : images) {
        if (featuresOnly) {
            encodings.push_back(image.encoding);
        }
        projectedEncodings.push_back(image.projectedEncoding);
        crops.push_back(vector<float>{image.crop.x, image.crop.y, image.crop.width, image.crop.height});
        probabilities.push_back(image.probability);
        filenames.push_back(image.filename);
    }
    if (featuresOnly) {
        dlib::serialize(encodings, fout);
    }
    dlib::serialize(projectedEncodings, fout);
    dlib::serialize(crops, fout);
    dlib::serialize(probabilities, fout);
    dlib::serialize(filenames, fout);
    if (!featuresOnly) {
        //dlib::serialize(rp.getE(), fout);
        dlib::serialize(rp.getV(), fout);
        dlib::serialize(rp.getColumnMeans(), fout);
    }
    ofLog()<<"Saved "<<images.size()<<" image vectors from "<<imageMap.size()<<" images to "<<path<<endl;
}

//--------------------------------------------------------------
void ofApp::load(string path, string baseDir, bool featuresOnly) {
    ofLog()<<"Loading from "<<path;
    const char *filepath = path.c_str();
    ifstream fin(filepath, ios::binary);
    vector<vector<double> > projectedEncodings;
    vector<vector<float> > encodings;
    vector<vector<float> > crops;
    vector<float> probabilities;
    vector<string> filenames;
    vector<double> column_means;
    dlib::matrix<double, 0, 0> E, V;
    if (featuresOnly) {
        dlib::deserialize(encodings, fin);
    }
    dlib::deserialize(projectedEncodings, fin);
    dlib::deserialize(crops, fin);
    dlib::deserialize(probabilities, fin);
    dlib::deserialize(filenames, fin);
    if (!featuresOnly) {
        //dlib::deserialize(E, fin);
        dlib::deserialize(V, fin);
        dlib::deserialize(column_means, fin);
        //rp.setE(E);
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
        image.crop = ofRectangle(crops[i][0], crops[i][1], crops[i][2], crops[i][3]);
        image.probability = probabilities[i];
        images.push_back(image);
        imageMap[image.filename] = true;
    }
    ofLog()<<"Loaded "<<images.size()<<" image vectors from "<<imageMap.size()<<" images in "<<path<<endl;
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


#include "ofApp.h"

const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};

void ofApp::scanDirectoryRecursive(ofDirectory dir) {
    ofDirectory new_dir;
    int size = dir.listDir();
    for (int i = 0; i < size; i++){
        if (dir.getFile(i).isDirectory()){
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            scanDirectoryRecursive(new_dir);
        }
        else if (find(begin(allowed_ext),
                      end(allowed_ext),
                      ofToLower(dir.getFile(i).getExtension())) != end(allowed_ext)) {
            imageFiles.push_back(dir.getFile(i));
        }
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    ccvPath = ofToDataPath("../../../../data/image-net-2012.sqlite3");
//    ccvPath = ofToDataPath("image-net-2012.sqlite3");
    
    // listen for scroll events, and save screenshot button press
    ofAddListener(ofEvents().mouseScrolled, this, &ofApp::mouseScrolled);
    bSaveScreenshot.addListener(this, &ofApp::eSaveScreenshot);
    bAnalyzeNew.addListener(this, &ofApp::eAnalyzeDirectory);
    bLoad.addListener(this, &ofApp::eLoadJSON);
    bSave.addListener(this, &ofApp::eSaveJSON);

    // setup gui
    ofParameterGroup gView, gAnalyze;
    gView.setName("view");
    gView.add(tViewGrid.set("view as grid", false));
    gView.add(scale.set("scale", 4.0, 1.0, 10.0));
    gView.add(imageSize.set("image size", 1.0, 0.0, 2.0));
    gAnalyze.setName("analyze");
    gAnalyze.add(numImages.set("max num images", 500, 1, 8000));
    gAnalyze.add(perplexity.set("perplexity", 50, 5, 80));
    gAnalyze.add(theta.set("theta", 0.3, 0.1, 0.7));
    gui.setup();
    gui.setName("Image t-SNE");
    gui.add(gView);
    gui.add(gAnalyze);
    gui.add(bAnalyzeNew.setup("analyze new directory"));
    gui.add(bLoad.setup("load result from json"));
    gui.add(bSave.setup("save result to json"));
    gui.add(bSaveScreenshot.setup("save screenshot"));
    
    position.set(0, 0);
    isAnalyzing = false;
}

//--------------------------------------------------------------
void ofApp::analyzeDirectory(string imagesPath){
    if (!ccv.isLoaded()){
        ccv.setup(ccvPath);
        if (!ccv.isLoaded()) {
            ofSystemAlertDialog("Can't find model file "+ccvPath+"!");
            return;
        }
    }
    
    // get list of all the images in the directory
    ofLog() << "Gathering images...";
    ofDirectory dir = ofDirectory(imagesPath);
    imageFiles.clear();
    scanDirectoryRecursive(dir);
    if (imageFiles.size() < numImages) {
        numImages = imageFiles.size();
        ofLog(OF_LOG_NOTICE, "There are less images in the directory than the number of images requested. Adjusting to "+ofToString(numImages));
    }

    // start analyzing
    thumbs.clear();
    encodings.clear();
    tsneVecs.clear();
    solvedGrid.clear();
    isAnalyzing = true;
    ofLog() << "Encoding images...";
}

//--------------------------------------------------------------
void ofApp::eAnalyzeDirectory(){
    ofFileDialogResult result = ofSystemLoadDialog("Analyze a directory of image", true);
    if (result.bSuccess) {
        analyzeDirectory(result.getPath());
    }
}

//--------------------------------------------------------------
void ofApp::updateAnalysis(){
    if (thumbs.size() < numImages) {
        int currIdx = thumbs.size();
        ImageThumb thumb;
        thumb.idx = currIdx;
        thumb.path = imageFiles[currIdx].getAbsolutePath();
        thumb.image.load(thumb.path);
        // resize thumb
        if (thumb.image.getWidth() > thumb.image.getHeight()) {
            thumb.image.crop((thumb.image.getWidth()-thumb.image.getHeight()) * 0.5, 0, thumb.image.getHeight(), thumb.image.getHeight());
        } else if (thumb.image.getHeight() > thumb.image.getWidth()) {
            thumb.image.crop(0, (thumb.image.getHeight()-thumb.image.getWidth()) * 0.5, thumb.image.getWidth(), thumb.image.getWidth());
        }
        thumb.image.resize(THUMB_SIZE, THUMB_SIZE);
        thumbs.push_back(thumb);
        progressMsg = "loaded "+ofToString(thumbs.size())+"/"+ofToString(numImages)+" images.";
    }
    else if (encodings.size() < thumbs.size()){
        int currIdx = encodings.size();
        vector<float> encoding = ccv.encode(thumbs[currIdx].image, ccv.numLayers()-1);
        encodings.push_back(encoding);
        progressMsg = "loaded "+ofToString(thumbs.size())+" images.";
        progressMsg += "\nencoded "+ofToString(encodings.size())+"/"+ofToString(thumbs.size())+" images.";
        if (encodings.size() == thumbs.size()) {
            progressMsg += "\nrunning t-SNE...";
        }
    }
    else if (tsneVecs.size() == 0) {
        runTsne();
        progressMsg = "loaded "+ofToString(thumbs.size())+" images.";
        progressMsg += "\nencoded "+ofToString(encodings.size())+" images.";
        progressMsg += "\nfinished running t-SNE!";
        progressMsg += "\nsolving grid...";
    }
    else if (solvedGrid.size() == 0) {
        solveToGrid();
        isAnalyzing = false;
        ofLog() << "Finished analyzing directory!";
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (isAnalyzing) {
        updateAnalysis();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(0), ofColor(100));
    ofPushMatrix();
    if (isAnalyzing) {
        ofDrawBitmapString(progressMsg, 250, 20);
    } else {
        ofTranslate(position.x * (scale - 1.0), position.y * (scale - 1.0));
        drawThumbs();
    }
    ofPopMatrix();
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::drawThumbs(){
    if (thumbs.size() == 0) {
        return;
    }
    
    if (tViewGrid) {
        imageSize = (scale * ofGetWidth()) / (thumbs[0].image.getWidth() * numGridCols);
        for (int i=0; i<solvedGrid.size(); i++) {
            float x = ofMap(solvedGrid[i].x, 0, 1, 0, imageSize * thumbs[i].image.getWidth() * (numGridCols-1));
            float y = ofMap(solvedGrid[i].y, 0, 1, 0, imageSize * thumbs[i].image.getHeight() * (numGridRows-1));
            thumbs[i].image.draw(x, y, imageSize * thumbs[i].image.getWidth(), imageSize * thumbs[i].image.getHeight());
            //ofDrawBitmapStringHighlight(ofToString(thumbs[i].idx), x+2, y+13);
        }
    }
    else {
        float maxDim = max(scale * ofGetWidth(), scale * ofGetHeight());
        for (int i=0; i<thumbs.size(); i++) {
            float x = ofMap(thumbs[i].point.x, 0, 1, 0, maxDim);
            float y = ofMap(thumbs[i].point.y, 0, 1, 0, maxDim);
            thumbs[i].image.draw(x, y, imageSize * thumbs[i].image.getWidth(), imageSize * thumbs[i].image.getHeight());
        }
    }
}

//--------------------------------------------------------------
void ofApp::runTsne() {
    ofLog() << "Run t-SNE on images";
    tsneVecs = tsne.run(encodings, 2, perplexity, theta, true);
    
    // normalize t-SNE vectors
    ofPoint tMin(1e8, 1e8), tMax(-1e8, -1e8);
    for (auto & t : tsneVecs) {
        tMin.set(min(tMin.x, (float) t[0]), min(tMin.y, (float) t[1]));
        tMax.set(max(tMax.x, (float) t[0]), max(tMax.y, (float) t[1]));
    }
    
    // save them to thumbs
    for (int t=0; t<tsneVecs.size(); t++) {
        thumbs[t].point.set((tsneVecs[t][0] - tMin.x) / (tMax.x - tMin.x), (tsneVecs[t][1] - tMin.y) / (tMax.y - tMin.y));
    }
}

//--------------------------------------------------------------
void ofApp::solveToGrid() {
    // this is a naive, brute-force way to figure out a grid configuration
    // (num rows x num columns) which fits as many of the original images as
    // possible. try all combinations where rows and cols are between
    // 3/ * sqrt(numImages) and 4/3 * sqrt(numImages)
    int numImages = thumbs.size();
    int minRows = floor((0.75) * sqrt(thumbs.size()));
    int maxRows = ceil(1.3333 * sqrt(thumbs.size()));
    int sizeGrid = 0;
    for (int i=minRows; i<maxRows; i++) {
        for (int j=i; j<=maxRows; j++) {
            int n = i * j;
            if (n > sizeGrid && n <= numImages) {
                numGridRows = i;
                numGridCols = j;
                sizeGrid = n;
            }
        }
    }
    
    // shuffle the thumbnails so you can take a random numImages-sized subset
    random_shuffle(thumbs.begin(), thumbs.end());
    
    // assign to grid
    ofLog() << "Solve t-SNE to "<<numGridCols<<"x"<<numGridRows<<" grid, "<<sizeGrid<<" of "<<numImages<<" fit";
    vector<ofVec2f> tsnePoints; // convert vector<double> to ofVec2f
    for (int t=0; t<sizeGrid; t++) {tsnePoints.push_back(thumbs[t].point);}
    vector<ofVec2f> gridPoints = makeGrid(numGridCols, numGridRows);
    solvedGrid = solver.match(tsnePoints, gridPoints, false);
}

//--------------------------------------------------------------
void ofApp::loadJSON(string jsonPath) {
    ofxJSONElement result;
    bool parsingSuccessful = result.open(jsonPath);
    for (int i=0; i<result.size(); i++) {
        string path = result[i]["path"].asString();
        float x = result[i]["point"][0].asFloat();
        float y = result[i]["point"][1].asFloat();
        ImageThumb thumb;
        thumb.point.set(x, y);
        thumb.idx = i;
        thumb.path = path;
        thumb.image.load(path);
        // resize thumb
        if (thumb.image.getWidth() > thumb.image.getHeight()) {
            thumb.image.crop((thumb.image.getWidth()-thumb.image.getHeight()) * 0.5, 0, thumb.image.getHeight(), thumb.image.getHeight());
        } else if (thumb.image.getHeight() > thumb.image.getWidth()) {
            thumb.image.crop(0, (thumb.image.getHeight()-thumb.image.getWidth()) * 0.5, thumb.image.getWidth(), thumb.image.getWidth());
        }
        thumb.image.resize(THUMB_SIZE, THUMB_SIZE);
        thumbs.push_back(thumb);
    }
    //resizeThumbs(THUMB_SIZE, THUMB_SIZE);
}

//--------------------------------------------------------------
void ofApp::saveJSON(string jsonPath) {
    ofxJSONElement result;
    for (int i=0; i<thumbs.size(); i++) {
        string path = thumbs[i].path;
        float x = thumbs[i].point.x;
        float y = thumbs[i].point.y;
        ofxJSONElement entry;
        entry["path"] = path;
        entry["point"][0] = x;
        entry["point"][1] = y;
        result.append(entry);
    }
    bool saveSuccessful = result.save(jsonPath);
}

//--------------------------------------------------------------
void ofApp::eLoadJSON() {
    ofFileDialogResult result = ofSystemLoadDialog("Load t-SNE from json file");
    if (result.bSuccess) {
        loadJSON(result.getPath());
        solveToGrid();
        ofLog() << "Finished loading " << result.getPath() << "!";
    }
}

//--------------------------------------------------------------
void ofApp::eSaveJSON() {
    ofFileDialogResult result = ofSystemSaveDialog("data.json", "Export t-SNE to json file");
    if (result.bSuccess) {
        saveJSON(result.getPath());
    }
}

//--------------------------------------------------------------
void ofApp::eSaveScreenshot(){
    ofFileDialogResult result = ofSystemSaveDialog("out.png", "Save current screenshot of t-SNE to PNG file");
    if (result.bSuccess) {
        saveScreenshot(result.getPath());
    }
}

//--------------------------------------------------------------
void ofApp::saveScreenshot(string imgPath){
    if (thumbs.size() == 0) {
        ofLog() << "there are no images to save...";
        return;
    }
    
    ofFbo fbo;
    if (tViewGrid) {
        fbo.allocate(imageSize * thumbs[0].image.getWidth() * numGridCols, imageSize * thumbs[0].image.getHeight() * numGridRows);
    } else {
        float maxDim = max(scale * ofGetWidth(), scale * ofGetHeight());
        fbo.allocate(maxDim + imageSize * thumbs[0].image.getWidth(), maxDim + imageSize * thumbs[0].image.getHeight());
    }
    
    fbo.begin();
    ofClear(0, 0);
    ofBackground(0);
    drawThumbs();
    fbo.end();
    
    ofPixels pix;
    ofImage img;
    fbo.readToPixels(pix);
    img.setFromPixels(pix);
    img.save(imgPath);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    position.x = ofClamp(position.x - (ofGetPreviousMouseX()-ofGetMouseX()), -scale * ofGetWidth() - 500, 500);
    position.y = ofClamp(position.y - (ofGetPreviousMouseY()-ofGetMouseY()), -scale * ofGetWidth() - 500, 500);
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(ofMouseEventArgs &evt) {
    scale.set(ofClamp(scale + 0.01 * (evt.scrollY), 1.0, 10.0));
}

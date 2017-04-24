#include "ofApp.h"

const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};

void ofApp::scan_dir_imgs(ofDirectory dir){
    ofDirectory new_dir;
    int size = dir.listDir();
    for (int i = 0; i < size; i++){
        if (dir.getFile(i).isDirectory()){
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            scan_dir_imgs(new_dir);
        } else if (std::find(std::begin(allowed_ext),
                             std::end(allowed_ext),
                             dir.getFile(i).getExtension()) != std::end(allowed_ext)) {
            imageFiles.push_back(dir.getFile(i));
        }
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    // setup ofxCcv
    ccv.setup(ofToDataPath("../../../../data/image-net-2012.sqlite3"));
    
    // imageDir, imageSavePath = location of images, path to save the final grid image
    // nx, ny = size of the grid (make sure there are at least nx*ny images in imageDir!)
    // w, h = downsample (or scale up) for source images prior to encoding!
    // displayW, displayH = resolution of the individual thumbnails for your output image - be careful about going over your maximum texture size on graphics card - 5000x5000 may work, but 10000x10000 may not
    // perplexity, theta (for t-SNE, see 'example' for explanation of these)
    
    imageDir = ofToDataPath("../../../../data/animals");
    
    ofAddListener(ofEvents().mouseScrolled, this, &ofApp::mouseScrolled);
    bAnalyzeNew.addListener(this, &ofApp::analyzeNew);
    
    // setup gui
    gui.setup();
    gui.add(zoom.set("view scale", 1.0, 0.0, 1.0));
    gui.add(scale.set("image scale", 1.0, 0.0, 2.0));
    gui.add(numGridRows.set("num rows", 10, 1, 100));
    gui.add(numGridCols.set("num cols", 10, 1, 100));
    gui.add(bAnalyzeNew.setup("analyze new directory"));
    gui.add(tViewGrid.set("view as grid", true));
    
    
    
    displayW = 100;
    displayH = 100;
    perplexity = 50; // corresponds to "number of neighbors", somewhere in the range 10-100 is good
    theta = 0.5; // lower is more "accurate" but takes longer
    
    
    
    analyzeImages();
    
    
    runTSNEAndGridding();
    
    
    
    
    
    position.set(-0.5 * ofGetWidth(), -0.5 * ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::analyzeImages(){
    resizeWidth = 256;
    resizeHeight = 256;
    
    // get images recursively from directory
    ofLog() << "Gathering images...";
    ofDirectory dir = ofDirectory(imageDir);
    scan_dir_imgs(dir);
    int numImages = numGridRows * numGridCols;
    if (imageFiles.size() < numImages) {
        ofLog(OF_LOG_ERROR, "There are less images in the directory than the grid size requested (nx*ny="+ofToString(numImages)+"). Exiting to save you trouble...");
        ofExit(); // not enough images to fill the grid, so quitting
    }
    
    // load all the images
    for(int i=0; i<numImages; i++) {
        if (i % 20 == 0)    ofLog() << " - loading image "<<i<<" / "<<numImages<<" ("<<dir.size()<<" in dir)";
        images.push_back(ofImage());
        images.back().load(imageFiles[i].getAbsolutePath());
    }
    
    // resize images to w x h
    for (int i=0; i<images.size(); i++) {
        if (images[i].getWidth() > images[i].getHeight()) {
            images[i].crop((images[i].getWidth()-images[i].getHeight()) * 0.5, 0, images[i].getHeight(), images[i].getHeight());
        }
        else if (images[i].getHeight() > images[i].getWidth()) {
            images[i].crop(0, (images[i].getHeight()-images[i].getWidth()) * 0.5, images[i].getWidth(), images[i].getWidth());
        }
        images[i].resize(resizeWidth, resizeHeight);
    }

}

//--------------------------------------------------------------
void ofApp::runTSNEAndGridding(){
    // encode all of the images with ofxCcv
    ofLog() << "Encoding images...";
    for (int i=0; i<images.size(); i++) {
        if (i % 20 == 0) ofLog() << " - encoding image "<<i<<" / "<<images.size();
        vector<float> encoding = ccv.encode(images[i], ccv.numLayers()-1);
        encodings.push_back(encoding);
    }
    
    // run t-SNE and load image points to imagePoints
    ofLog() << "Run t-SNE on images";
    tsneVecs = tsne.run(encodings, 2, perplexity, theta, true);
    for (auto t : tsneVecs) {
        cout << ofToString(t) << endl;
    }
    // solve assignment grid
    vector<ofVec2f> tsnePoints; // convert vector<double> to ofVec2f
    for (auto t : tsneVecs) tsnePoints.push_back(ofVec2f(t[0], t[1]));
    vector<ofVec2f> gridPoints = makeGrid(numGridCols, numGridRows);
    solvedGrid = solver.match(tsnePoints, gridPoints, false);

}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::saveToImage() {
    string imageSavePath = ofToDataPath("tsne_grid_animals.png");
    ofFbo fbo;
    fbo.allocate(numGridCols * displayW, numGridRows * displayH);
    fbo.begin();
    ofClear(0, 0);
    ofBackground(0);
    for (int i=0; i<solvedGrid.size(); i++) {
        float x = (fbo.getWidth() - displayW) * solvedGrid[i].x;
        float y = (fbo.getHeight() - displayH) * solvedGrid[i].y;
        images[i].draw(x, y, displayW, displayH);
    }
    fbo.end();
    ofImage img;
    fbo.readToPixels(img);
    img.save(imageSavePath);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(0), ofColor(100));
    
    ofPushMatrix();
    
    ofTranslate(position.x * (zoom - 0.5), position.y * (zoom - 0.5));

    if (tViewGrid) {
        for (int i=0; i<solvedGrid.size(); i++) {
            float x = zoom * (numGridCols - 1) * resizeWidth * solvedGrid[i].x;
            float y = zoom * (numGridRows - 1) * resizeHeight * solvedGrid[i].y;
            images[i].draw(x, y, scale * images[i].getWidth(), scale * images[i].getHeight());
        }
    }
    else {
        for (int i=0; i<images.size(); i++) {
            float x = ofMap(tsneVecs[i][0], 0, 1, 0, scale * ofGetWidth());
            float y = ofMap(tsneVecs[i][1], 0, 1, 0, scale * ofGetHeight());
//            float x = scale * (numGridCols - 1) * resizeWidth * tsneVecs[i][0];
//            float y = scale * (numGridRows - 1) * resizeHeight * tsneVecs[i][1];
//            
            //thumbs[i].image.draw(x, y, imageSize * thumbs[i].image.getWidth(), imageSize * thumbs[i].image.getHeight());
            
            images[i].draw(x, y, scale * images[i].getWidth(), scale * images[i].getHeight());
        }
        ofPopMatrix();
    }
    
    ofPopMatrix();
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    position.x = position.x - (ofGetMouseX()-ofGetPreviousMouseX());
    position.y = position.y - (ofGetMouseY()-ofGetPreviousMouseY());
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(ofMouseEventArgs &evt) {
    zoom.set(ofClamp(zoom + 0.01 * (evt.scrollY), 0.0, 10.0));
}

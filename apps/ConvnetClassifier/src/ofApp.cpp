#include "ofApp.h"

const ofColor backgroundPlotColor = ofColor(50,50,50,255);

void ofApp::setup() {
    ofSetWindowShape(640, 575);
    cam.initGrabber(320, 240);
    
    
    largeFont.load("verdana.ttf", 12, true, true);
    largeFont.setLineHeight(14.0f);
    smallFont.load("verdana.ttf", 10, true, true);
    smallFont.setLineHeight(12.0f);
    hugeFont.load("verdana.ttf", 20, true, true);
    hugeFont.setLineHeight(38.0f);
    
    //Initialize the training and info variables
    infoText = "";
    trainingClassLabel = 1;
    predictedClassLabel = 0;
    trainingModeActive = false;
    recordTrainingData = false;
    predictionModeActive = false;
    drawInfo = true;
    
    //Set the inputs
    trainingData.setNumDimensions( 4096 );
    
    //set the default classifier
    setClassifier( SVM_LINEAR );
    
    
    ccv.setup("../../../../data/image-net-2012.sqlite3");
    if (!ccv.isLoaded()) return;
    
}

void ofApp::update() {
    if (!ccv.isLoaded()) {
        ofDrawBitmapString("Network file not found! Check your data folder to make sure it exists.", 20, 20);
        return;
    }
    
    cam.update();
    

    VectorFloat inputVector(4096);
    

        featureEncoding = ccv.encode(cam, ccv.numLayers()-1);
        for (int i=0; i<featureEncoding.size(); i++) {
            inputVector[i] =  featureEncoding[i];
        }
    
    
    
    
    //Update the training mode if needed
    if( trainingModeActive ){
        
        //Check to see if the countdown timer has elapsed, if so then start the recording
        if( !recordTrainingData ){
            if( trainingTimer.timerReached() ){
                recordTrainingData = true;
                trainingTimer.start( RECORDING_TIME );
            }
        }else{
            //We should be recording the training data - check to see if we should stop the recording
            if( trainingTimer.timerReached() ){
                trainingModeActive = false;
                recordTrainingData = false;
            }
        }
        
        if( recordTrainingData ){
            
            if( !trainingData.addSample(trainingClassLabel, inputVector) ){
                infoText = "WARNING: Failed to add training sample to training data!";
            }
        }
    }
    
    //Update the prediction mode if active
    if( predictionModeActive ){
        
        
        if( pipeline.predict( inputVector ) ){
            predictedClassLabel = pipeline.getPredictedClassLabel();
            predictionPlot.update( pipeline.getClassLikelihoods() );
            
        }else{
            infoText = "ERROR: Failed to run prediction!";
        }
    }
}


void ofApp::draw() {

    ofBackground(200);

    
    ofSetColor(255);
    cam.draw(285, 225);
    
    
    
    int marginX = 5;
    int marginY = 5;
    int graphX = marginX;
    int graphY = marginY;
    int graphW = ofGetWidth() - graphX*2;
    int graphH = 150;
    
    //Draw the info text
    if( drawInfo ){
        float infoX = marginX;
        float infoW = 250;
        float textX = 10;
        float textY = marginY;
        float textSpacer = smallFont.getLineHeight() * 1.5;
        
        ofFill();
        ofSetColor(100,100,100);
        ofDrawRectangle( infoX, 5, infoW, 565 );
        ofSetColor( 255, 255, 255 );
        
        smallFont.drawString( "Convnet CLASSIFIER EXAMPLE", textX, textY +20); textY += textSpacer*2;
        
        smallFont.drawString( "[i]: Toogle Info", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[r]: Toggle Recording", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[l]: Load Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[s]: Save Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[c]: Clear Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[t]: Train Model", textX, textY ); textY += textSpacer;
        smallFont.drawString( "[1,2,3...]: Set Class Label", textX, textY ); textY += textSpacer;
        smallFont.drawString( "Classifier: " + classifierTypeToString( classifierType ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "[n] null rejection: " + ofToString(nullRejection), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        smallFont.drawString( "Class Label: " + ofToString( trainingClassLabel ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Recording: " + ofToString( recordTrainingData ), textX, textY ); textY += textSpacer;
        smallFont.drawString( "Num Samples: " + ofToString( trainingData.getNumSamples() ), textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        ofSetColor (255,204,0);
        smallFont.drawString( infoText, textX, textY ); textY += textSpacer;
        textY += textSpacer;
        
        textY += textSpacer;
        
        ofSetColor (255,255,255);
    
        smallFont.drawString("INSTRUCTIONS", textX, textY ); textY += textSpacer;
        smallFont.drawString("1. Press [r] to record some ", textX, textY ); textY += textSpacer;
        smallFont.drawString("training examples", textX, textY ); textY += textSpacer;
        smallFont.drawString("2. Press [1,2,3...] to switch class", textX, textY ); textY += textSpacer;
        smallFont.drawString("3. Press [r] to record new samples", textX, textY ); textY += textSpacer;
        smallFont.drawString("4. Press [t] to train the model", textX, textY ); textY += textSpacer;
        
        textY += textSpacer;
        textY += textSpacer;
        smallFont.drawString( "Framerate : "+ofToString(ofGetFrameRate()), textX, textY ); textY += textSpacer;
        
        //Update the graph position
        graphX = infoX + infoW + 15;
        graphW = ofGetWidth() - graphX - 15;
    }
    
    
    if( trainingModeActive ){
        char strBuffer[1024];
        if( !recordTrainingData ){
            ofSetColor(255,150,0);
            sprintf(strBuffer, "Training Mode Active \nGet Ready! Timer: %0.1f",trainingTimer.getSeconds());
        }else{
            ofSetColor(255,0,0);
            sprintf(strBuffer, "Training Mode Active \nRecording! Timer: %0.1f",trainingTimer.getSeconds());
        }
        std::string txt = strBuffer;
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        hugeFont.drawString(strBuffer, graphX, 510 );
    }
    
    
    //If the model has been trained, then draw this
    if( pipeline.getTrained()){
        predictionPlot.draw( graphX, graphY, graphW, graphH ); graphY += graphH * 1.1;
        
        std::string txt = "Predicted Class: " + ofToString( predictedClassLabel );
        ofRectangle bounds = hugeFont.getStringBoundingBox( txt, 0, 0 );
        ofSetColor(0,0,255);
        hugeFont.drawString( txt, graphX, 195 );
    }

}

void ofApp::keyPressed(int key) {
    
    infoText = "";
    bool buildTexture = false;
    
    switch ( key) {
        case 'r':
            predictionModeActive = false;
            trainingModeActive = true;
            recordTrainingData = false;
            trainingTimer.start( PRE_RECORDING_COUNTDOWN_TIME );
            break;
        case '1':
            trainingClassLabel = 1;
            break;
        case '2':
            trainingClassLabel = 2;
            break;
        case '3':
            trainingClassLabel = 3;
            break;
        case '4':
            trainingClassLabel = 4;
            break;
        case '5':
            trainingClassLabel = 5;
            break;
        case '6':
            trainingClassLabel = 6;
            break;
        case '7':
            trainingClassLabel = 7;
            break;
        case '8':
            trainingClassLabel = 8;
            break;
        case '9':
            trainingClassLabel = 9;
            break;
            
        case 'n':
            nullRejection =! nullRejection;
            setClassifier(this->classifierType % NUM_CLASSIFIERS ); //Is this solid?
            break;
            
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";
                std::cout << "getNumClasses: " << pipeline.getNumClasses() << std::endl;
                predictionPlot.setup( 500, pipeline.getNumClasses(), "prediction likelihoods" );
                predictionPlot.setDrawGrid( true );
                predictionPlot.setDrawInfoText( true );
                predictionPlot.setFont( smallFont );
                predictionPlot.setBackgroundColor( backgroundPlotColor );
                predictionModeActive = true;
            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.save( ofToDataPath("TrainingData.grt") ) ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.load( ofToDataPath("TrainingData.grt") ) ){
                infoText = "Training data loaded from file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
        case 'i':
            drawInfo = !drawInfo;
            break;
            
        case OF_KEY_TAB:
            setClassifier( ++this->classifierType % NUM_CLASSIFIERS );
            break;
            
        default:
            break;
            
            
    }
}


bool ofApp::setClassifier( const int type ){
    
    AdaBoost adaboost;
    DecisionTree dtree;
    KNN knn;
    GMM gmm;
    ANBC naiveBayes;
    MinDist minDist;
    RandomForests randomForest;
    Softmax softmax;
    SVM svm;
    
    this->classifierType = type;
    
    switch( classifierType ){
        case ADABOOST:
            adaboost.enableNullRejection( nullRejection );
            adaboost.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( adaboost );
            break;
        case DECISION_TREE:
            dtree.enableNullRejection( nullRejection );
            dtree.setNullRejectionCoeff( 3 );
            dtree.setMaxDepth( 10 );
            dtree.setMinNumSamplesPerNode( 3 );
            dtree.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( dtree );
            break;
        case KKN:
            knn.enableNullRejection( nullRejection );
            knn.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( knn );
            break;
        case GAUSSIAN_MIXTURE_MODEL:
            gmm.enableNullRejection( nullRejection );
            gmm.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( gmm );
            break;
        case NAIVE_BAYES:
            naiveBayes.enableNullRejection( nullRejection );
            naiveBayes.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( naiveBayes );
            break;
        case MINDIST:
            minDist.enableNullRejection( nullRejection );
            minDist.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( minDist );
            break;
        case RANDOM_FOREST_10:
            randomForest.enableNullRejection( nullRejection );
            randomForest.setNullRejectionCoeff( 3 );
            randomForest.setForestSize( 10 );
            randomForest.setNumRandomSplits( 2 );
            randomForest.setMaxDepth( 10 );
            randomForest.setMinNumSamplesPerNode( 3 );
            randomForest.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( randomForest );
            break;
        case RANDOM_FOREST_100:
            randomForest.enableNullRejection( nullRejection );
            randomForest.setNullRejectionCoeff( 3 );
            randomForest.setForestSize( 100 );
            randomForest.setNumRandomSplits( 2 );
            randomForest.setMaxDepth( 10 );
            randomForest.setMinNumSamplesPerNode( 3 );
            randomForest.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( randomForest );
            break;
        case RANDOM_FOREST_200:
            randomForest.enableNullRejection( nullRejection );
            randomForest.setNullRejectionCoeff( 3 );
            randomForest.setForestSize( 200 );
            randomForest.setNumRandomSplits( 2 );
            randomForest.setMaxDepth( 10 );
            randomForest.setMinNumSamplesPerNode( 3 );
            randomForest.setRemoveFeaturesAtEachSpilt( false );
            pipeline.setClassifier( randomForest );
            break;
        case SOFTMAX:
            softmax.enableNullRejection( false );
            softmax.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( softmax );
            break;
        case SVM_LINEAR:
            svm.enableNullRejection( nullRejection );
            svm.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( SVM(SVM::LINEAR_KERNEL) );
            break;
        case SVM_RBF:
            svm.enableNullRejection( nullRejection );
            svm.setNullRejectionCoeff( 3 );
            pipeline.setClassifier( SVM(SVM::RBF_KERNEL) );
            break;
        default:
            return false;
            break;
    }
    
    return true;
}

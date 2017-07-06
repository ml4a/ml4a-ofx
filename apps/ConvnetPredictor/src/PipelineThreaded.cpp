#include "PipelineThreaded.h"


PipelineThreaded::PipelineThreaded() {
    training = false;
    success = false;
}

void PipelineThreaded::startTraining() {
    startThread();
    training = true;
}

void CategoricalThreaded::setup(string name, ofxPanel *panelRef, int inputSize) {
    trainingData.setNumDimensions( inputSize );
    panelRef->add(slider.set(name, 1, 1, 2));
}

void RegressionThreaded::setup(string name, ofxPanel *panelRef, int inputSize) {
    trainingData.setInputAndTargetDimensions( inputSize, 1 );
    panelRef->add(slider.set(name, 0.5, 0.0, 1.0));
    //sliderF.addListener(this, &ofApp::eSlider);
}

void CategoricalThreaded::setupModel() {
    KNN knn; /*Other classifiers: AdaBoost adaboost; DecisionTree dtree; KNN knn; GMM gmm; ANBC naiveBayes; MinDist minDist; RandomForests randomForest; Softmax softmax; SVM svm; */
    setClassifier( knn );
}

void RegressionThreaded::setupModel() {
    unsigned int numInputNeurons = 4096; //trainingData.getNumInputDimensions();
    unsigned int numOutputNeurons = 1; //1 as we are using multidimensional regression
    
    //Initialize the MLP
    int maxEpochs = 10;
    int numHiddenNeurons = 10;
    
    MLP mlp;
    mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::SIGMOID, Neuron::SIGMOID );
    
    //Set the training settings
    mlp.setMaxNumEpochs( maxEpochs ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
    mlp.setMinChange( 1.0e-10 ); //This sets the minimum change allowed in training error between any two epochs
    mlp.setLearningRate( 0.01 ); //This sets the rate at which the learning algorithm updates the weights of the neural network
    mlp.setNumRandomTrainingIterations( 1 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
    mlp.setUseValidationSet( true ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
    mlp.setValidationSetSize( 15 ); //Use 20% of the training data for validation during the training phase
    mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
    
    //The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e. [0.0 1.0])
    mlp.enableScaling( true );
    
    *this << MultidimensionalRegression(mlp,true);
}

void CategoricalThreaded::clear() {
    trainingData.clear();
    PipelineThreaded::clear();
}

void RegressionThreaded::clear() {
    trainingData.clear();
    PipelineThreaded::clear();
}

bool CategoricalThreaded::addSample(VectorFloat *inputVector) {
    return trainingData.addSample(slider, *inputVector);
}

bool RegressionThreaded::addSample(VectorFloat *inputVector) {
    VectorFloat targetVector(1);
    targetVector[0] = slider;
    return trainingData.addSample(*inputVector, targetVector);
}

bool CategoricalThreaded::predict(VectorFloat *inputVector) {
    bool success = GestureRecognitionPipeline::predict(*inputVector);
    if (success) {
        slider = getPredictedClassLabel();
    }
    return success;
}

bool RegressionThreaded::predict(VectorFloat *inputVector) {
    bool success = success = GestureRecognitionPipeline::predict(*inputVector);
    if (success) {
        target = getRegressionData()[0];
    }
    return success;
}

void CategoricalThreaded::update() { }

void RegressionThreaded::update() {
    float lerpAmt = 0.5;
    slider = ofLerp(slider, target, lerpAmt);
}

void CategoricalThreaded::threadedFunction() {
    while(isThreadRunning()) {
        if(lock()) {
            success = train(trainingData);
            training = false;
            unlock();
            stopThread();
        } else {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

void RegressionThreaded::threadedFunction() {
    while(isThreadRunning()) {
        if(lock()) {
            success = train(trainingData);
            training = false;
            unlock();
            stopThread();
        } else {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

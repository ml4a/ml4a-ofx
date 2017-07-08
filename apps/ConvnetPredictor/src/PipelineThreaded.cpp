#include "PipelineThreaded.h"


PipelineThreaded::PipelineThreaded() {
    training = false;
    success = false;
}

void PipelineThreaded::startTraining() {
    startThread();
    training = true;
}

void CategoricalThreaded::setup(string name, ofxPanel *panelRef, int inputSize, int numClasses) {
    trainingData.setNumDimensions( inputSize );
    panelRef->add(slider.set(name, 1, 1, numClasses));
}

void RegressionThreaded::setup(string name, ofxPanel *panelRef, int inputSize) {
    trainingData.setInputAndTargetDimensions( inputSize, 1 );
    panelRef->add(slider.set(name, 0.5, 0.0, 1.0));
}

void CategoricalThreaded::setupModel(const int type) {
    switch(type) {
        case SUPPORT_VECTOR_MACHINE:
            {
                SVM svm;
                svm.setMinNumEpochs(500);
                svm.setMaxNumEpochs(1000);
                setClassifier(svm);
            }
            break;
        case KNearestNeighbors:
            {
                KNN knn;
                setClassifier(knn);
            }
            break;
        default:
            return false;
            break;
    }
}

void RegressionThreaded::setupModel(const int type) {
    switch(type) {
        case LOGISTIC_REGRESSION:
            {
                LogisticRegression logisticRegression;
                logisticRegression.setMinNumEpochs(500);
                logisticRegression.setMaxNumEpochs(1000);
                logisticRegression.setMinChange(1.0e-8);
                logisticRegression.setLearningRate(0.01);
                logisticRegression.setUseValidationSet(true);
                logisticRegression.setValidationSetSize(15);
                logisticRegression.setRandomiseTrainingOrder(true);
                logisticRegression.enableScaling(true);
                *this << MultidimensionalRegression(logisticRegression, true);
            }
            break;
        case NEURAL_NET:
            {
                MLP mlp;
                mlp.init(4096, 20, 1, Neuron::LINEAR, Neuron::SIGMOID, Neuron::SIGMOID );
                mlp.setMinNumEpochs(500);
                mlp.setMaxNumEpochs(1000);
                mlp.setMinChange(1.0e-8);
                mlp.setLearningRate(0.01);
                mlp.setNumRandomTrainingIterations(1);
                mlp.setUseValidationSet(true);
                mlp.setValidationSetSize(15);
                mlp.setRandomiseTrainingOrder(true);
                mlp.enableScaling( true );
                *this << MultidimensionalRegression(mlp,true);
            }
            break;
        default:
            return false;
            break;
    }
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
    targetVector[0] = (float) slider;
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

void CategoricalThreaded::update(float lerpAmt) { }

void RegressionThreaded::update(float lerpAmt) {
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

## FaceClassifier

[A complete guide to this application can be found here](http://ml4a.github.io/guides/FaceClassifier/)

Example showing how to train a model and do real time classification with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).
 Inputs are your selected facial features (gestures, orientation and/or raw points). Outputs are width and height of a rectangle. 
Supports a long range of classification algoritms, and allows users to switch between iputs and combine them on the fly. 
Also allows users to save and load models.
 
This application requires the following addons:
- [ofxCv](https://github.com/kylemcdonald/ofxCv)
- [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2)
- [ofxGrt](https://github.com/nickgillian/ofxGrt)

 
### Instructions:
1. To record a training example, make a distinct facial gesture, then press the r key to start recording some training examples for that class (the class should default to 1 when you start the example). 
2. Press the 2 number key to switch the class label to 2 and make a different facial gesture and press r to record again
3. Repeat this again for class 3 or as many classes as you like
4. Press [t] to train the model
5. Do facial gestures and see what class the model predicts you are doing 


To do:
- Make the UI a bit more friendly
- Flexible graph area
- Enable users to delete the latest recording
## FaceDTW

Example showing how to train DTW model and do real time classification with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).
Inputs are your selected facial features (gestures and/or orientation). 
Also allows users to save and load models.
 
This application requires the following addons:
- [ofxCv](https://github.com/kylemcdonald/ofxCv)
- [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2)
- [ofxGrt](https://github.com/nickgillian/ofxGrt)

Tips:
 Accurately training gestures can be tricky, but it should be possible to make a somewhat stable "nod" vs "head shake" vs "no movement" classifier using the orientation inputs or a gesture . Try to save class [0] for your neutral position (no expressions) and make sure to record something in the class.
 See a training demo video [here](http://andreasrefsgaard.dk/wp-content/uploads/2016/09/FaceDTW.mp4).

To do:
 - The app is currently a bit slow or lagging... Find out what is causing this.
 - Using raw points as inputs is currently disabled, since it makes the framerate drop a lot
 - Clean up the way the recordings are drawn to the screen - does not work well with the big nuber of inputs that is currently used
 - Remove or improve the distance matrix
 - Make a more intitive example?

 
### Instructions:
1. Press [r] to start recording a neutral gesture on class 0. Press [r] to stop recording again. 
2. Repeat this a number of times.
3. Press [1] to change class and [r] to record a distinct gesture. Press [r] to stop recording again.
4. Repeat this a number of times.
5. Press [t] to train the model.


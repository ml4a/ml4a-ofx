## FaceRegressor

[A complete guide to this application can be found here](http://ml4a.github.io/guides/FaceRegressor/)

[And a tutorial video](https://player.vimeo.com/video/212934622)

Example showing how to train a model and do real time regression with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).
 Inputs are your selected facial features (gestures, orientation and/or raw points). Outputs are width and height of a rectangle. 

This application requires the following addons:
- [ofxCv](https://github.com/kylemcdonald/ofxCv)
- [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2)
- [ofxGrt](https://github.com/nickgillian/ofxGrt)
 
### Instructions:

1. Set the height and width of the rectangle using the sliders

2. Click record and prepare a distinct facial expression. Once recording, your training examples will contain containing your selected facial features (gestures, orientation and/or raw points) and the current width and height of the rectangle.

3. Repeat step 1-2 with different width and height values for the rectangle and different expressions

4. Click train to train the model

5. Move your face and see the changes in rectangle width and height





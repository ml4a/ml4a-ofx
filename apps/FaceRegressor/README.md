##FaceRegressor

 Example showing how to train a model and do real time regression with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).
 Inputs are your selected facial features (gestures, orientation and/or raw points). Outputs are width and height of a rectangle. 
 
###Instructions:
1. Set the height and width of the rectangle using [+], [-], [1] and [2] keys
2. Press [r] to record some training samples containing your selected facial features (gestures, orientation and/or raw points) and the width+height of the rectangle.
3. Repeat step 1) and 2) with different rectangle width and height and different facial expressions / head orientations
4. Press [t] to train the model
5. Move your face and see the changes in rectangle width and height based on your facial orientation and expression


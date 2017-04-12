## SimpleRegression

[A complete guide to this application can be found here](http://ml4a.github.io/guides/SimpleRegression/)

[Also, see this short tutorial video](https://player.vimeo.com/video/212888761 )

Simple example showing how to train a model and do real time regression with [ofxGrt](https://github.com/nickgillian/ofxGrt).
 Inputs are mouseX and mouseY. Outputs are width and height of a rectangle.

This addon requires [ofxGrt](https://github.com/nickgillian/ofxGrt).
 
### Instructions:
1. Set the height and width of the rectangle using the sliders

2. Press record and use the countdown time to position your mouse. Once recording, your training examples will contain the x and y position of your mouse and the current width and height of the rectangle.

3. Repeat step 1-2 with different width and height values for the rectangle and different mouse positions

4. Press train to train the model

5. Move your mouse around the canvas and see the changes in the width and height of the rectangle



## DoodleClassifier

[A complete guide to this application can be found here](http://ml4a.github.io/guides/DoodleClassifier/)

[A short tutorial video can be found here](https://vimeo.com/196944929/). 

This application requires the following addons:  
- [ofxCv](https://github.com/kylemcdonald/ofxCv) 
- [ofxCcv](https://github.com/kylemcdonald/ofxCcv) 
- [ofxGrt](https://github.com/nickgillian/ofxGrt)

An interface for classifying doodles/drawings. The app lets you define a series of doodle categories, examples drawn under a webcam, and then learns to classify new doodles.

The settings file `settings_doodleclassifier.xml` contains settings which you may wish to modify.  You may edit or add to the defined classes inside the `<classes>` tag. The settings file also contains the OSC info. By default, OSC is set to send to localhost, through port 5000, to address "/classification".

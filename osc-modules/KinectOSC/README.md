## KinectOSC

This is a simple utility for use with the Kinect. It will track a single user's skeleton and continuously send the coordinates over open sound control (OSC). 

A gui gives you the option to toggle sending real-world (3d) coordinates or screen-projected (2d) coordinates or both.

By default, it will stream to port 6448, to the osc address `/wek/inputs`.

These settings can be edited in the file `settings_kinect.xml` inside the data folder.

## building from source

If you want to build this from source, it requires the following addons:

 - [ofxOpenNI](https://github.com/gameoverhack/ofxOpenNI/)
 - [ofxControl](https://github.com/genekogan/ofxControl)

Make sure you follow the instructions in [ofxOpenNI](https://github.com/gameoverhack/ofxOpenNI/) to include the openni config folder in the data folder of the application, which has been omitted from this repository because of space considerations. If you can't find it, you can download it in the [releases page of KinectOSC](https://github.com/genekogan/KinectOSC/releases) or in the ofxOpenNI page.

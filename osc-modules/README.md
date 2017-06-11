# osc-modules

Standalone openFrameworks apps which send or receive data over open sound control (OSC) between various peripherals and applications. 

Includes:
 - [AbletonOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/AbletonOSC) : Receives data to control Ableton Live music parameters.
 - [AudioUnitOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/AudioUnitOSC) : Receives data to control Audio Unit modules.
 - [ConvnetOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/ConvnetOSC) : Sends convolutional neural network-based feature vectors of a live webcam stream (using ofxCcv).
 - [DarknetOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/DarknetOSC) : Sends convolutional neural network-based feature vectors of a live webcam stream (using ofxDarknet).
 - [FaceTracker2OSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/FaceTracker2OSC) : Sends coordinates of face found in a webcam stream.
 - [KeyboardOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/KeyboardOSC) : Execute keystrokes via OSC commands.
 - [KinectOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/KinectOSC) : Sends skeleton data of a single tracked skeleton from a Kinect.
 - [LeapMotionOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/LeapMotionOSC) : Sends finger point data detected by Leap Motion hand tracker.

KinectOSC*, FaceTracker2OSC, ConvnetOSC, and DarknetOSC require additional data files in order to work properly. These files can be all be downloaded at once by navigating to the data folder and running `sh setup.sh`.  If you are having any trouble with those, refer to the documentation in each of those pages linked above for more specific usage instructions and links for the individual data files needed.

Additionally, the OSC configuration for each of the applications are also located in the `data` folder. Editing those files will let you adjust the destination to send OSC to (or in the case of `AbletonOSC` and `AudioUnitOSC` where to receive from).

*Note, KinectOSC requires some additional files from ofxOpenNI not found in `setup.sh` (refer to the [readme](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/KinectOSC)) in KinectOSC for info on how to get that).

Check the releases page for the latest downloadable release.
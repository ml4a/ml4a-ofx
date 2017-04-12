# osc-modules

Standalone openFrameworks apps which send or receive data over open sound control (OSC) between various peripherals and applications. 

Includes:
 - [AbletonOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/AbletonOSC) : Receives data to control Ableton Live music parameters.
 - [AudioUnitOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/AudioUnitOSC) : Receives data to control Audio Unit modules.
 - [ConvnetOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/ConvnetOSC) : Sends convolutional neural network-based feature vectors of a live webcam stream.
 - [FaceTracker2OSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/FaceTracker2OSC) : Sends coordinates of face found in a webcam stream.
 - [KinectOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/KinectOSC) : Sends skeleton data of a single tracked skeleton from a Kinect.
 - [LeapMotionOSC](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules/LeapMotionOSC) : Sends finger point data detected by Leap Motion hand tracker.

KinectOSC, FaceTracker2OSC, and ConvnetOSC require additional data files in order to work properly. Refer to the documentation in each of those pages linked above for more specific usage instructions and links for the data files needed.

Check the releases page for the latest downloadable release.
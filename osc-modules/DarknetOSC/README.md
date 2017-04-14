## DarknetOSC

Application which sends Darknet classification probabilities over OSC.

To compile it from source, you need the addon [ofxDarknet](https://github.com/mrzl/ofxDarknet). Either way, you need to make sure `darknet.weights` are downloaded to the `data/darknet` folder of `ml4a-ofx`. 

To change the OSC host, port, and address, edit the `settings_darknet.xml` file in the data folder. By default, it is set to localhost, port 6448, address `/wek/inputs`, which works out of the box with Wekinator. 

#### Note about compiling with ofxOsc

At the moment, you will get compilation errors if you try to compile this from source because of some conflicts between ofxOsc and ofxDarknet (documented [here](https://forum.openframeworks.cc/t/compiling-ofxosc-and-ofxdarknet-together/26446)). To fix this error, simply comment out the ofxOsc lines with the errors and it will work then. A permanent solution to this problem is needed -- please send a PR to [ofxDarknet](https://github.com/mrzl/ofxDarknet) if you know a solution.
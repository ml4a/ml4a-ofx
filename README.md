## ml4a-ofx

A collection of real-time interactive applications and associated scripts for working with machine learning. 

All apps contained here require [openFrameworks](http://www.openframeworks.cc) to run, as well as a number of addons, listed below.

### Organization

The openFrameworks apps are provided as source code, and can be built and compiled by using the project generator that comes with openFrameworks.

Several of these applications are coupled with python scripts which do some analysis of media (feature extraction, t-SNE, etc), whose results are then imported into your ofApp via JSON or some other means for further processing. Some of them can be replicated entirely within openFrameworks, and wherever possible (for example, t-SNE) such applications are also provided. The advantage of the scripts is that they can be used in other environments as well, and are much easier to port or implement from the preeminent machine learning frameworks. 

### Applications

The [apps](https://github.com/ml4a/ml4a-ofx/tree/master/apps) folder contains all of the individual openFrameworks apps, and their descriptions and usage instructions.

### Osc Modules

The [osc-modules](https://github.com/ml4a/ml4a-ofx/tree/master/osc-modules) folder contains a set of applications which send or receive OSC in order to facilitate communication with other applications, and are very easily used alongside [Wekinator](http://www.wekinator.org). 

### Datasets

Some of the applications work on image/audio/text datasets. Example datasets are provided and can be downloaded with the provided scripts (see `download_images.py` for image example). Additionally, some require pre-trained models which can also be downloaded. For the ones which require VGG weights, you can download that [here](https://drive.google.com/file/d/0Bz7KyqmuGsilT0J5dmRCM0ROVHc/view?usp=sharing).

### Addons

All of the included applications (both in the apps and osc-modules folders) require some combination of the following addons. The readme for each app lists the addons required individually. The following is a complete list: 
- [ofxAbletonLive](https://github.com/genekogan/ofxAbletonLive)
- [ofxAssignment](https://github.com/kylemcdonald/ofxAssignment)
- [ofxAudioUnit](https://github.com/admsyn/ofxAudioUnit)
- [ofxCv](https://github.com/kylemcdonald/ofxCv)
- [ofxCcv](https://github.com/kylemcdonald/ofxCcv)
- [ofxDarknet](https://github.com/mrzl/ofxDarknet)
- [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2)
- [ofxGrt](https://github.com/nickgillian/ofxGrt)
- [ofxJSON](https://github.com/jeffcrouse/ofxJSON)
- [ofxLeapMotion2](https://github.com/genekogan/ofxLeapMotion2)
- [ofxLearn](https://github.com/genekogan/ofxLearn)
- [ofxMaxim](https://github.com/falcon4ever/ofxMaxim)
- [ofxOpenNI](https://github.com/gameoverhack/ofxOpenNI)
- [ofxScreenGrab](https://github.com/genekogan/ofxScreenGrab)
- [ofxTemplateMatch](https://github.com/genekogan/ofxTemplateMatching)
- [ofxTSNE](https://github.com/genekogan/ofxTSNE)

The addons also make use of ofxGui, ofxOsc, and ofxOpenCv, which are included in openFrameworks by default.

These addons are not currently used in ml4a-ofx, but are also relevant and may be wrapped into examples in the future: 
- [ofxMSATensorflow](https://github.com/memo/ofxMSATensorFlow)
- [ofxDlib](https://github.com/bakercp/ofxDlib)
- [ofxCaffe](https://github.com/Geekrick88/ofxCaffe)
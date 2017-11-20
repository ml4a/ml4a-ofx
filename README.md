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

## Getting started

These are some steps to get you up and running with ml4a-ofx:

### Prepare your project

0. Make sure you've read and followed the [openFrameworks setup guides](http://openframeworks.cc/download/) relevant for your system. Going through them makes sure the foundation for all ml4a-ofx apps is working correctly.
1. This guide assumes that you have:
   - One folder with the `openFrameworks` setup in it. This is the folder in which you would test your `openFrameworks` setup as described above.
   - One different folder in which you have the `ml4a-ofx` content. This folder does *not* have to be within the `openFrameworks` folder.
1. For your project, you most certainly want to use an existing `ml4a-ofx` app as a starting point. So simply duplicate any of the folders within `ml4a-ofx/apps` and give the folder a custom name. Or work in the folder of the example project if you do not plan to re-use it anyways.
1. Check the `readme.md` file of your project. It always contains the required addons you need to get your app running. Follow the links of each addon, check the installation guide and follow each one.
   - Some of the apps need example data to work correctly. As you probably have enough space on your computer anyways, open a terminal window, go into the `ml4a-ofx` folder and run `sh setup.sh`. This will download the necessary data and place it in the `data` folder.
   - All addons currently used across the examples are mentioned in the [Addons list](#addons).
   - Be careful: Put the required addons into the `addons` subfolder of your `openFrameworks` folder to work correctly.

### Setup your project

#### On a Mac

You can use the `projectGenerator` app that is included in `openFrameworks`.

- Set the base path to your `openFrameworks` folder as described in the app. This has to be done on the first time only.
- Click on the `create/update` tab
- Import your project by clicking the `import` button, selecting your project's folder. This should be located in your `ml4a-ofx` folder. Your project name, project path and the addons list should update accordingly. If any of the addons is missing, the interface will notify you of that. Since you prepared your project in the above described steps, all should be good here. Otherwise, check the "Prepare your project" part again.
- Click "Update" to get the project setup running. The interface doesn't show it explicitly, but this gets the project generator running. It should™ result in a success message. You can then click "Open in IDE" to directly open the project in XCode. Also, feel free to close the `projectGenerator` again since you don't need it anymore for now.
- Within XCode, wait a few moments to index and process the files of the project. You can then compile the project by pressing the "Play" button or `CMD + R`.
- Cross your fingers :fingers_crossed: and hope your project compiles :wink:

If you run into any errors: The `openFrameworks setup guides` might have some explanations or possible steps. Otherwise, it always helps to google the error (seriously!) and try to figure out what's going on. In case you get completely stuck, you might find help here by [opening an issue](https://github.com/ml4a/ml4a-ofx/issues/new). Include any error message you find and try to explain the problem as good as possible. This will help resolve any issue a lot.

#### On Windows

[Feel free to add documentation here]

#### On Linux

[Feel free to add documentation here]
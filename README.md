## ml4a-ofx

Collection of openFrameworks apps and associated scripts for working with machine learning. This is a work-in-progress.

### Organization

The openFrameworks apps are provided as source code only. Use the project generator to create the actual project files. 

Some of these applications are coupled with python scripts which do some analysis of media (feature extraction, t-SNE, etc), whose results are then imported into your ofApp via JSON or some other means for further processing. Some of them can be replicated entirely within openFrameworks, and wherever possible (for example, t-SNE) such applications are also provided. The advantage of the scripts is that they can be used in other environments as well, and are much easier to port or implement from the preeminent machine learning frameworks. 

### Datasets

Some of the applications work on image/audio/text datasets. Example datasets are provided and can be downloaded with the provided scripts (see `download_images.py` for image example). Additionally, some require pre-trained models which can also be downloaded. For the ones which require VGG weights, you can download that [here](https://drive.google.com/file/d/0Bz7KyqmuGsilT0J5dmRCM0ROVHc/view?usp=sharing).

### Applications

The [apps](https://github.com/ml4a/ml4a-ofx/tree/master/apps) folder contains a description of each of the applications in this repository.



tSNE live?

___

addons

ofxGui
ofxOsc

- [ofxCv](https://github.com/kylemcdonald/ofxCv)
- [ofxCcv](https://github.com/kylemcdonald/ofxCcv)
- [ofxDarknet](https://github.com/mrzl/ofxDarknet)
- [ofxFaceTracker2]https://github.com/HalfdanJ/ofxFaceTracker2)
- [ofxGrt](https://github.com/nickgillian/ofxGrt)
- [ofxJSON](https://github.com/jeffcrouse/ofxJSON)
- [ofxLearn](https://github.com/genekogan/ofxLearn)
- [ofxMaxim](https://github.com/falcon4ever/ofxMaxim)

ofxMSATensorflow

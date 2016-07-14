## ml4a-ofx

Collection of openFrameworks apps and associated scripts for working with machine learning. This is a work-in-progress. There will be better documentation soon.


### Organization

The openFrameworks apps are provided as source code only. Use the project generator to create the actual project files. 

Some of these applications are coupled with python scripts which do some analysis of media (feature extraction, t-SNE, etc), whose results are then imported into your ofApp via JSON or some other means for further processing. Some of them can be replicated entirely within openFrameworks, and wherever possible (for example, t-SNE) such applications are also provided. The advantage of the scripts is that they can be used in other environments as well, and are much easier to port or implement from the preeminent machine learning frameworks. 


### Datasets

Some of the applications work on image/audio/text datasets. Example datasets are provided and can be downloaded with the provided scripts. Additionally, some require pre-trained models which can also be downloaded... instructions + download scripts tbd.


### Applications

Specific instructions for running each application are included in a README inside each app folder. A description of each application follows:

#### tSNE-images

View a collection of audio samples organized via t-SNE on extracted audio features. Works both on a collection (folder) of individual files, or a single audio file which can is segmented by onsets or beats.

#### tSNE-audio

View a collection of audio samples organized via t-SNE on extracted audio features. Works both on a collection (folder) of individual files, or a single audio file which can is segmented by onsets or beats.

#### tSNE-images-manual (TBD)

(to-do) The same as tSNE-images, but uses ofxCcv and ofxTSNE to do the analysis entirely within openFrameworks. 

#### ConvnetOSC

Send the fc7 activations of a trained convnet over OSC.

#### ConvnetViewer (TBD)

View the activations of a trained convnet. In the future, this may be merged with ConvnetOSC as a single application which enables viewing, encoding, sending OSC, and importing image sets.
## Applications

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
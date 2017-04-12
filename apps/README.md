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

#### ConvnetViewer

View the activations of a trained convnet. In the future, this may be merged with ConvnetOSC as a single application which enables viewing, encoding, sending OSC, and importing image sets.

#### ReverseImageSearch

Search for most similar images in an image collection, given a query image. Analyze it first, then save the results for real-time browsing.

#### ConvnetClassifier

Use the fc7 activations of a trained convnet to make your own classifier using [ofxGrt](https://github.com/nickgillian/ofxGrt).

#### FaceClassifier

Train a model and do real time classification with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).

#### FaceDTW

Perform facial gestures over time and classify them using Dynamic Time Warping with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).

#### FaceRegressor

Train a model and do real time regression with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2).

###SimpleRegression

Boiled down example showing how to train a model and do real time regression with [ofxGrt](https://github.com/nickgillian/ofxGrt) with mouse position as the input.





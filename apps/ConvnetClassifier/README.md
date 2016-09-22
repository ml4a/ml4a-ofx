##ConvnetClassifier

Example showing how to train a model and do real time classification with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ConvNet](https://github.com/ml4a/ml4a-ofx/tree/master/apps).

Inputs are ConvNet activations as a 4096-bit input vector from the webcam feed.
Supports a long range of classification algoritms and allows users to save and load models.
 

For this example to work, you need to place the image-net-2012.sqlite3 file in the apps data folder.
The image-net-2012.sqlite3 file can be found in [ofxCcv](https://github.com/kylemcdonald/ofxCcv) or downloaded directly from [here](https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3).

To do:
- Run the convnet part in a seperate thread
 
###Instructions:
1. Press [r] to record some training examples
2. Press [1,2,3...] to switch class
3. Press [r] to record new samples
4. Press [t] to train the model



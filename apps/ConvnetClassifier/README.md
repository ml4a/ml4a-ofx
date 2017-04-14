## ConvnetClassifier

[A complete guide to this application can be found here](http://ml4a.github.io/guides/ConvnetClassifier/)

Example showing how to train a model and do real time classification with [ofxGrt](https://github.com/nickgillian/ofxGrt) and [ConvNet](https://github.com/ml4a/ml4a-ofx/tree/master/apps).

This application requires the addons [ofxCcv](https://github.com/kylemcdonald/ofxCcv) and [ofxGrt](https://github.com/nickgillian/ofxGrt).

Inputs are ConvNet activations as a 4096-bit input vector from the webcam feed.

Supports a long range of classification algoritms and allows users to save and load models.

Make sure the file `image-net-2012.sqlite3` is located inside the folder `ml4a-ofx/data`, or change the line `ccv.setup("../../../../data/image-net-2012.sqlite3");` to reflect the location of this file. The `image-net-2012.sqlite3` file can be found in [ofxCcv](https://github.com/kylemcdonald/ofxCcv) or downloaded directly from [here](https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3).

To do:
- Run the convnet part in a separate thread
 
### Instructions:
1. Click [record] or press [r] to record some training examples
2. Set the slider or press [1,2,3...] to switch class
3. Click [record] or press [r] to record new samples
4. Click [train] or press [t] to train the model



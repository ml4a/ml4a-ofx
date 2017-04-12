## Fast Reverse Image Search

[A complete guide to this application can be found here](http://ml4a.github.io/guides/ReverseImageSearchFast/)

This app requires:
 - [ofxCcv](https://github.com/kylemcdonald/ofxCcv)
 - [ofxLearn](https://github.com/genekogan/ofxLearn)
 - [ofxKDTree](https://github.com/genekogan/ofxKDTree)

Fast reverse image search on a live image, including webcam, video, or saved image. For same thing, but able to use a screengrab as an input, see [ReverseImageSearchLive](https://github.com/ml4a/ml4a-ofx/tree/master/apps/ReverseImageSearchLive).

The app collects images from a directory (recursively) and extracts a feature vector from each one, using a convolutional neural net. It then computes a principal component analysis on the entire feature vector set (to reduce the dimensionality) and saves all of it to file. At run-time, the PCA-projected feature vectors are embedded into a [k-d tree](https://en.wikipedia.org/wiki/K-d_tree) for fast nearest neighbor search. A live input image is analyzed in the same fashion (extracted with a convnet and projected to the same PCA-reduced space), and `k` nearest neighbors are fetched from the k-d tree and loaded on disk. 

Better instructions how to use this soon. Also, a model which has been trained on [MS-Coco](http://mscoco.org/) (~145,000 images) will be uploaded and made available shortly. 

For a screengrab of this being done live, [see here](https://twitter.com/ml4a_/status/835874470606798848).


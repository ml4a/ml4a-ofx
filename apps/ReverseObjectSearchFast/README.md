# ReverseObjectSearchFast

[These instructions can also be found here](http://ml4a.github.io/guides/ReverseObjectSearchFast/).

This app demonstrates how to quickly do object detection on an image and retrieve `k` nearest neighbors to each of the found objects. It is mostly identical to [ReverseImageSearchFast](https://github.com/ml4a/ml4a-ofx/tree/master/apps/ReverseImageSearchFast), except that it operates over multiple subsets of the image (detected objects) simultaneously rather than over the whole image itself.

The below screenshot shows the basic functionality. A query image (top-center) is analyzed, and a number of objects are detected ("soup bowl", "club sandwich", etc). Below it is a row of similar images for each object. Similar images are computed from possibly hundreds of thousands of candidates, loaded from disk, and displayed on the app in less than 1 second, creating opportunities to be used in real-time.

![ReverseObjectSearchFast setup](http://ml4a.github.io/images/guides/reverseobjectsearch_food.jpg "ReverseObjectSearchFast setup")

## How it works
---
A large dataset of images is pre-analyzed by a convolutional neural network trained to detect and label salient objects found in each image. The detected objects are recorded, along with their original file paths, bounding boxes, probabilities/confidences, and feature vectors (a 1024-length vector extracted from the second to last layer of a ["YOLO" convnet](http://pjreddie.com/darknet/yolo/)). 

After all feature vectors are extracted, they are transformed into a lower dimensional subspace (for dimensionality reduction) using a [random matrix projection](http://stats.stackexchange.com/questions/235632/pca-vs-random-projection). It can optionally be done with principal component analysis instead, which is probably a bit more accurate, but runtime can be very high for a large corpus.

After feature extraction and dimensionality reduction, the results are serialized into a `.dat` file so you can load them back later.  

After the analysis has been done or a previous analysis has been loaded, the reduced feature vectors are embedded into a [k-d tree](https://en.wikipedia.org/wiki/K-d_tree) for fast lookup.  

At runtime, a new image is queried by being analyzed in exactly the same procedure as above -- its objects are detected, feature vectors are extracted and projected into the same lower-dimensional subspace using the random matrix generated during the analysis phase. The projected vectors are each used to query the KD-Tree which retrieves the approximate nearest `k` neighbors to each object respectively (`k` is set using the `num results` slider). Those images are then loaded from disk and displayed.


## Installation
---
The following addons are needed:

- [ofxDarknet](https://github.com/mrzl/ofxDarknet)
- [ofxKDTree](https://github.com/genekogan/ofxKDTree)
- [ofxLearn](https://github.com/genekogan/ofxLearn)

After generating the project in project generator, make sure the `cfg` folder which comes with ofxDarknet is inside the data folder of the application, so darknet can find the config files.

Also make sure XCode/Visual Studio can find the `libdarknetOSX.dylib` dynamic library by making sure the `other linker flags` has the correct path to it (By default it uses a relative path which won't work if you place this app in a non-standard location).

## Usage instructions
---

### Analyzing directory of images

In order to be able to retrieve nearest neighbor images, a dataset of images needs to first be analyzed. Through the interface, this can be done by clicking `analyze directory`, which will compile the image paths (recursively) from a base directory. Equivalently, run the following code:

    baseDir = "YOUR_IMAGES_DIRECTORY_HERE";
    extractFeaturesForDirectory(baseDir);
    runDimReduction();	
    runKDTree();

The analysis may take hours, depending on how many images are in the directory. Each image can take around 0.25-0.75 sec depending on your machine.  

Once analysis is done, it can be saved by clicking `save vectors` or running:

    save("YOUR_VECTORS_FILE.dat");

If you want to save just the original vectors without the projections (useful to save them while doing the analysis, in case it crashes in the middle...) you can set the `featuresOnly` bool to true and run:

	save("YOUR_VECTORS_FILE.dat", true);
	
To load the saved vectors back into memory next time you launch the application, click `load vectors` and select the saved `dat` file, then select the base directory where the images are located. Equivalently, this can be done in code:

	baseDir = "/PATH_TO_IMAGE_DIRECTORY";
    load("/PATH_TO_SAVED_VECTORS", baseDir);
    

### Download pre-analyzed image set

An example image set was analyzed and prepared, which you can download and use. The image set is [MC-Coco](http://mscoco.org/), and the saved analysis contains 376,459 detected objects from 136,199 unique images.  

First you need to [download](http://mscoco.org/dataset/#download) the images. Download the files (around 25GB altogether):

- 2014 Training images [80K/13GB]
- 2014 Val. images [40K/6.2GB]
- 2014 Testing images [40K/6.2GB]

Unzip them and place the images into side-by-side folders 'train', 'test', and 'val' respectively. Make sure the images are unzipped this way because the trained vectors are saved for these relative paths. For example, the first image in the the `test` directory should be called something like `/PATH_TO_COCO/test/COCO_test2014_000000000014.jpg` where `/PATH_TO_COCO/` is the location of the folder containing the images.

Next, download the saved vectors. There are 4 options, which vary according to their dimensionality: 16, 32, 64, and 128. The tradeoff is that the bigger ones are more accurate, but are larger, take longer to load into memory, and query times are longer as well. I find 32 to be a good compromise but your results and the demands of your use case may vary.

All can be downloaded from Dropbox via the following links: 

- [mscoco_145k_rp16 (265 MB)](https://www.dropbox.com/s/k1pnanmyt62jn1q/mscoco_145k_rp16.dat?dl=0)
- [mscoco_145k_rp32 (497 MB)](https://www.dropbox.com/s/y7uzxqfskes1n8t/mscoco_145k_rp32.dat?dl=0)
- [mscoco_145k_rp64 (964 MB)](https://www.dropbox.com/s/x9df05b1xt35mbo/mscoco_145k_rp64.dat?dl=0)
- [mscoco_145k_rp128 (1.9 GB)](https://www.dropbox.com/s/udf9qpu47r5rfpt/mscoco_145k_rp128.dat?dl=0)

Afterwards, you may load them as usual following the method in the previous section. Make sure `baseDir` is pointing towards the parent folder of `train`, `test`, and `val`.

### Runtime

Once vectors are loaded, the application can analyze a new image and find nearest neighbors for each detected object. There are three input options, which are selected via toggle.

1) `query random image`: this will randomly load an image from the analyzed images (in `baseDir`) and do the analysis on it.

2) `query webcam`: this will turn on your webcam and begin to analyze it continuously.

3) `query video`: this will let you load a video file from disk and begin playing it and analyzing it continuously.

There are several parameters to be aware of:

- `yolo threshold` sets the minimum confidence over which to include detected objects. Higher threshold gives you fewer but more confident items back.
- `max overlap` is a heuristic to filter out objects which are overlapping and thus often redundantly capturing duplicates of objects at slightly different bounding box. A `max overlap` of 0.5 means that it will filter out objects whose area is more than 50% of some other found object, prioritizing objects by confidence.
- `header height` and `thumb height` simply control the display height of the query image and the resulting image rows. 
- `num results` controls the number of nearest neighbors to load for each detected object.

Another thing to be aware of is that it's often a good idea to save smaller resolution/lower qualities images of your dataset, because if the image is large, then loading several dozen from disk for each detection may take much more runtime than the actual similarity calculation.


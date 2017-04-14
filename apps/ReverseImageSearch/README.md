## Reverse Image Search

[A complete guide to this application can be found here](http://ml4a.github.io/guides/ReverseImageSearch/)

This app displays the `k` nearest neighbors to any image within a pre-analyzed dataset. The nearest neighbor analysis is done previously in a python script (more below) and the results are saved to a JSON file. The openFrameworks application simple loads the JSON file and displays the result. A much more dynamic version of this which allows you to get the `k` nearest neighbors to new, previously unseen images, can be found at [ReverseImageSearchFast](https://github.com/ml4a/ml4a-ofx/tree/master/apps/ReverseImageSearchFast) and [ReverseImageSearchLive](https://github.com/ml4a/ml4a-ofx/tree/master/apps/ReverseImageSearchLive).

As such, this app only requires [ofxJSON](https://github.com/jeffcrouse/ofxJSON)

First run `analyze-images.py` script to extract features for your images and record a list of nearest neighbors for each image. Specify a directory of images and some parameters:

 	python analyze-images.py --images_path path/to/input/directory --output_path path/to/output/jsonw

for example:

	python analyze-images.py --images_path ../datasets/animals --output_path ../apps/ReverseImageSearch/bin/data/lookup.json

Optionally you can specify the number of PCA components to base the distance lookup with the `--num_components` flag (set to 300 by default) and you can also set how many nearest neighbors to record for each image with `--num_closest` (100 by default).
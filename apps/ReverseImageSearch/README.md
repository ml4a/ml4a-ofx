## Reverse Image Search

This app requires [ofxJSON](https://github.com/jeffcrouse/ofxJSON)

First run `analyze-images.py` script to extract features for your images and record a list of nearest neighbors for each image. Specify a directory of images and some parameters:

 	python analyze-images.py --vgg_path path/to/vgg/weights --images_path path/to/input/directory --output_path path/to/output/json

for example:

	python analyze-images.py --vgg_path ../datasets/vgg16_weights.h5 --images_path ../datasets/animals --output_path ../apps/ReverseImageSearch/bin/data/lookup.json


Optionally you can specify the number of PCA components to keep with the `--num_components` flag (is set to 300 by default) and you can also set how many nearest neighbors to record for each image with `--num_closest` (100 by default).
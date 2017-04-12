## Image t-SNE viewer

[A complete guide to this application can be found here](http://ml4a.github.io/guides/ImageTSNEViewer/)

This app requires [ofxJSON](https://github.com/jeffcrouse/ofxJSON)

The above instructions, more succinctly:

First run `tSNE-images.py` script to generate the t-SNE. Specify a directory of images and some parameters:

	python tSNE-images.py --images_path path/to/input/directory --output_path path/to/output/json

for example:

	python tSNE-images.py --images_path ../datasets/animals/ --output_path ../apps/ImageTSNEViewer/bin/data/points.json

Then run this application, where `path` has been set to the location of your JSON file generated above. Alternatively, name it `points.json` and place it in the data folder, which is the default path.
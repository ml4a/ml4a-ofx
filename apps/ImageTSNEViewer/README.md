## image t-SNE viewer

First run tSNE-images.py script to generate the t-SNE. Specify a directory of images and some parameters:

	python tSNE-images.py --vgg_path path/to/vgg/weights --images_path path/to/input/directory --output_path path/to/output/json

for example:

	python tSNE-images.py --vgg_path ../datasets/vgg16_weights.h5 --images_path ../datasets/animals/ --output_path ../apps/ImageTSNEViewer/bin/data/points.json

You can change the number of dimensions for the t-SNE with the parameter "--num_dimensions" but this ofApp is setup to just read 2 columns at the moment.


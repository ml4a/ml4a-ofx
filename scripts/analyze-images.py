import argparse
import sys
import numpy as np
import os
from os.path import isfile, join
import h5py
from PIL import Image
from keras.models import Sequential
from keras.layers.core import Flatten, Dense, Dropout
from keras.layers import Convolution2D, ZeroPadding2D, MaxPooling2D
from keras.optimizers import SGD
from sklearn.decomposition import PCA
from scipy.spatial import distance
import json


def process_arguments(args):
    parser = argparse.ArgumentParser(description='tSNE on audio')
    parser.add_argument('--vgg_path', action='store', help='path to location of VGG-16 weights')
    parser.add_argument('--images_path', action='store', help='path to directory of images')
    parser.add_argument('--output_path', action='store', help='path to where to put output json file')
    parser.add_argument('--num_components', action='store', default=300, help='number of principal components to keep [300]')
    parser.add_argument('--num_closest', action='store', default=100, help='number of nearest neighbors for each image to record [100]')
    params = vars(parser.parse_args(args))
    return params

def get_image(path):
    try:
        img = Image.open(path)
    except:
        print("Error loading %s: skipping"%path)
        return None
    if img.mode != 'RGB':
        img = img.convert('RGB')
    img = img.resize((224, 224), Image.ANTIALIAS)
    im = np.array(img.getdata(), np.uint8)
    im = im.reshape(img.size[1], img.size[0], 3).astype(np.float32)
    im[:,:,0] -= 123.68   # mean-centering and transposition is probably unecessary
    im[:,:,1] -= 116.779
    im[:,:,2] -= 103.939
    im = im.transpose((2,0,1))
    im = np.expand_dims(im, axis=0)
    return im

def VGG_16(weights_path):
    model = Sequential()
    model.add(ZeroPadding2D((1,1),input_shape=(3,224,224)))
    model.add(Convolution2D(64, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(64, 3, 3, activation='relu'))
    model.add(MaxPooling2D((2,2), strides=(2,2)))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(128, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(128, 3, 3, activation='relu'))
    model.add(MaxPooling2D((2,2), strides=(2,2)))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(256, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(256, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(256, 3, 3, activation='relu'))
    model.add(MaxPooling2D((2,2), strides=(2,2)))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(512, 3, 3, activation='relu'))
    model.add(MaxPooling2D((2,2), strides=(2,2)))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(512, 3, 3, activation='relu'))
    model.add(ZeroPadding2D((1,1)))
    model.add(Convolution2D(512, 3, 3, activation='relu'))
    model.add(MaxPooling2D((2,2), strides=(2,2)))
    model.add(Flatten())
    model.add(Dense(4096, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(4096, activation='relu'))
    f = h5py.File(weights_path)
    for k in range(f.attrs['nb_layers']):
        if k >= len(model.layers):
            break
        g = f['layer_{}'.format(k)]
        weights = [g['param_{}'.format(p)] for p in range(g.attrs['nb_params'])]
        model.layers[k].set_weights(weights)
    print("finished loading VGGNet")
    return model

def main(vgg_path, images_path, output_path, num_components, num_closest):
	model = VGG_16(vgg_path) # load model
	sgd = SGD(lr=0.1, decay=1e-6, momentum=0.9, nesterov=True)
	model.compile(optimizer=sgd, loss='categorical_crossentropy')
	# get images
	images = [f for f in os.listdir(images_path) if isfile(join(images_path, f))]
	# analyze images and grab activations
	data = {"paths":[], "activations":[]}
	for idx,image_path in enumerate(images):
	    file_path = join(images_path,image_path)
	    image = get_image(file_path);
	    if image is not None:
	        print "getting activations for %s %d/%d" % (image_path,idx,len(images))
	        acts = model.predict(image)[0]
	        data["paths"].append(os.path.abspath(join(images_path,images[idx])))
	        data["activations"].append(acts)
	activations = np.array(data["activations"])
	pca = PCA(n_components=num_components)
	pca.fit(activations)
	data["pca_acts"] = pca.transform(activations)
	lookups = []
	for i, p1 in enumerate(data["pca_acts"]):
	    distances = []
	    for j, p2 in enumerate(data["pca_acts"]):
	        dst = distance.euclidean(p1, p2)
	        distances.append(dst)
	    idx_closest = sorted(range(len(distances)), key=lambda k: distances[k])[1:num_closest]
	    lookups.append(idx_closest)
	json_data = []
	for i,lookup in enumerate(lookups):
	    json_data.append({"path":data["paths"][i], "lookup":lookup})
	with open(output_path, 'w') as outfile:
	    json.dump(json_data, outfile)

if __name__ == '__main__':
	params = process_arguments(sys.argv[1:])
	vgg_path = params['vgg_path']
	images_path = params['images_path']
	num_components = int(params['num_components'])
	num_closest = int(params['num_closest'])
	output_path = params['output_path']
	main(vgg_path, images_path, output_path, num_components, num_closest)
	print("finished saving %s"%output_path)


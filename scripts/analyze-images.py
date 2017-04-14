import argparse
import sys
import numpy as np
import json
import os
from os.path import isfile, join
import keras
from keras.preprocessing import image
from keras.applications.imagenet_utils import decode_predictions, preprocess_input
from keras.models import Model
from sklearn.decomposition import PCA
from sklearn.manifold import TSNE
from scipy.spatial import distance

def process_arguments(args):
    parser = argparse.ArgumentParser(description='tSNE on audio')
    parser.add_argument('--images_path', action='store', help='path to directory of images')
    parser.add_argument('--output_path', action='store', help='path to where to put output json file')
    parser.add_argument('--num_closest', action='store', default=10, help='number of nearest neighbors to store')
    params = vars(parser.parse_args(args))
    return params

def get_image(path, input_shape):
    img = image.load_img(path, target_size=input_shape)
    x = image.img_to_array(img)
    x = np.expand_dims(x, axis=0)
    x = preprocess_input(x)
    return x

def analyze_images(images_path):
    # make feature_extractor
    model = keras.applications.VGG16(weights='imagenet', include_top=True)
    feat_extractor = Model(inputs=model.input, outputs=model.get_layer("fc2").output)
    input_shape = model.input_shape[1:3]
    # get images
    candidate_images = [f for f in os.listdir(images_path) if os.path.splitext(f)[1].lower() in ['.jpg','.png','.jpeg']]
    # analyze images and grab activations
    activations = []
    images = []
    for idx,image_path in enumerate(candidate_images):
        file_path = join(images_path,image_path)
        img = get_image(file_path, input_shape);
        if img is not None:
            print("getting activations for %s %d/%d" % (image_path,idx,len(candidate_images)))
            acts = feat_extractor.predict(img)[0]
            activations.append(acts)
            images.append(image_path)
    # run PCA firt
    print("Running PCA on %d images..." % len(activations))
    features = np.array(activations)
    pca = PCA(n_components=300)
    pca.fit(features)
    pca_features = pca.transform(features)
    return images, pca_features

def find_closest_lookup(images_path, output_path, num_closest):
    images, pca_features = analyze_images(images_path)
    lookups = []
    for i, p1 in enumerate(pca_features):
        distances = [distance.euclidean(p1, p2) for p2 in pca_features]
        idx_closest = sorted(range(len(distances)), key=lambda k: distances[k])[1:num_closest+1]
        lookups.append(idx_closest)
    json_data = [{"path":os.path.abspath(join(images_path, images[i])), "lookup":lookup} for i,lookup in enumerate(lookups)]
    with open(output_path, 'w') as outfile:
        json.dump(json_data, outfile)
    

if __name__ == '__main__':
    params = process_arguments(sys.argv[1:])
    images_path = params['images_path']
    output_path = params['output_path']
    num_closest = int(params['num_closest'])
    find_closest_lookup(images_path, output_path, num_closest)
    print("finished saving %s" % output_path)

#!/bin/sh

echo "Downloading shape_predictor_68_face_landmarks.dat"
curl -L -o data/shape_predictor_68_face_landmarks.dat.bz2 --progress-bar https://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2

echo "Extracting shape_predictor_68_face_landmarks.dat"
bzip2 -d data/shape_predictor_68_face_landmarks.dat.bz2

echo "Downloading image-net-2012.sqlite3 and image-net-2012.words"
curl -L -o data/image-net-2012.sqlite3 --progress-bar https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3
curl -L -o data/image-net-2012.words --progress-bar https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.words

echo "Downloading darknet files"
curl -L -o data/darknet.cfg --progress-bar https://github.com/pjreddie/darknet/blob/master/cfg/darknet.cfg
curl -L -o data/imagenet.shortnames.list --progress-bar https://github.com/pjreddie/darknet/blob/master/cfg/imagenet.shortnames.list
curl -L -o data/darknet.weights --progress-bar http://pjreddie.com/media/files/darknet.weights

echo "Done"

#!/bin/sh

echo "Downloading shape_predictor_68_face_landmarks.dat"
curl -L -o shape_predictor_68_face_landmarks.dat.bz2 --progress-bar https://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2

echo "Extracting shape_predictor_68_face_landmarks.dat"
bzip2 -d shape_predictor_68_face_landmarks.dat.bz2

echo "Downloading image-net-2012.sqlite3 and image-net-2012.words"
curl -L -o image-net-2012.sqlite3 --progress-bar https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3
curl -L -o image-net-2012.words --progress-bar https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.words

echo "Downloading darknet files"
curl -L -o darknet.cfg --progress-bar https://github.com/pjreddie/darknet/blob/master/cfg/darknet.cfg
curl -L -o imagenet.shortnames.list --progress-bar https://github.com/pjreddie/darknet/blob/master/cfg/imagenet.shortnames.list
curl -L -o darknet.weights --progress-bar http://pjreddie.com/media/files/darknet.weights

echo "Done"

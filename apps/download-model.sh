#!/bin/sh

echo "Downloading shape_predictor_68_face_landmarks.dat"
curl -L -o model/shape_predictor_68_face_landmarks.dat.bz2 --progress-bar https://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2

echo "Extracting shape_predictor_68_face_landmarks.dat"
bzip2 -d model/shape_predictor_68_face_landmarks.dat.bz2

echo "Done"

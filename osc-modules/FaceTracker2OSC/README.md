## FaceTracker2OSC

FaceTracker2OSC will track a face and send its raw tracked points over OSC. 
To compile it from source, you need the addons [ofxFaceTracker2](https://github.com/HalfdanJ/ofxFaceTracker2), [ofxCv](https://github.com/kylemcdonald/ofxCv), and [ofxControl](https://github.com/genekogan/ofxControl).

To set the port, ip address, and osc address to send the data to, modify the file `settings_facetracker2.xml`.

When a face is detected, the application will send its mesh points (68 xy-pairs = 136 numbers) over OSC to the port and address written at the bottom of the app, which was set in the xml file mentioned above. By default, the points are normalized, which means the points are all relative to the bounding box (all between 0 and 1), by subtracting the center point of the face and dividing by the maximum size of the bounding box. This makes the points invariant to position and zoom, and therefore dependent mainly on facial gesture. You can deselect normalize, in which case the raw points will be sent without any post-processing.

For this app to work, you need the file `shape_predictor_68_face_landmarks.dat` placed in the data folder, which can be downloaded and unzipped from [here](http://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2).
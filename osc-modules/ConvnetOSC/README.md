## ConvnetOSC

Application which sends ConvNet activations over OSC.

The GUI lets you toggle whether to send the classification layer probabilities (1000 elements) or the activations from the pre-classification last fully connected layer ("fc2", 4096 elements). The classification probabilities give you a probability distribution over all the classes in ImageNet, whereas the fc2 features tend to be preferred for transfer learning applications.

To compile it from source, you need the addon [ofxCcv](https://github.com/kylemcdonald/ofxCcv).

If you'd like the associated class names of the classifications, you can get them from the file `image-net-2012.words` which can be found in ofxCcv.

Make sure the files `image-net-2012.sqlite3` and  are inside the data folder. This can be found in ofxCcv or downloaded directly from [here](https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3).

To change the OSC host, port, and address, edit the `settings_convnet.xml` file in the data folder. By default, it is set to localhost, port 6448, address `/wek/inputs`, which works out of the box with Wekinator. An additional parameter `sendClassificationsByDefault` toggles whether the app launches by default sending classifications or fc2.


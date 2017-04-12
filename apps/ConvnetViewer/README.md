## ConvnetViewer

[A complete guide to this application can be found here](http://ml4a.github.io/guides/ConvnetViewer/)

Application which lets you live-browse the activations of a convnet. By default, will run on the webcam stream, but you can load an image for analysis as well. The encoding is run in a separate thread.

This application requires the addon [ofxCcv](https://github.com/kylemcdonald/ofxCcv).

Make sure the file `image-net-2012.sqlite3` is located inside the folder `ml4a-ofx/data`, or change the line `ccv.setup("../../../../data/image-net-2012.sqlite3");` to reflect the location of this file. The `image-net-2012.sqlite3` file can be found in [ofxCcv](https://github.com/kylemcdonald/ofxCcv) or downloaded directly from [here](https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3).

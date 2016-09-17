## ConvnetOSC

Sends ConvNet activations as a 4096-bit input vector over OSC.

To change the OSC host, port, and address, edit the `settings.xml` file in the data folder. By default, it is set to localhost, port 6448, address "/wek/inputs", which works out of the box with [Wekinator](http://www.wekinator.org/).

Also make sure `image-net-2012.sqlite3` is inside the data folder. This can be found in ofxCcv or downloaded directly from [here](https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3).

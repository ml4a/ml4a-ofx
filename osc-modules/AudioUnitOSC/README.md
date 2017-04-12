## AudioUnitOSC

This OSX-only application lets you play and control parameters in your [audio units](https://developer.apple.com/library/content/documentation/MusicAudio/Conceptual/AudioUnitProgrammingGuide/Introduction/Introduction.html) over OSC. Mac OS comes with many built-in audio units, and there are many more that can be downloaded for free or pay.

To compile it from source, you need the addons [ofxAudioUnit](https://github.com/admsyn/ofxAudioUnit) and [ofxControl](https://github.com/genekogan/ofxControl).

When you launch the application, it will populate a menu of all your audio units found in your system. You can select multiple audio units, further populating menus to the right which show all the available parameters that you can control remotely. 

Edit the file `settings_audiounit.xml` to set the OSC port and address that AudioUnitOSC should be expecting OSC messages to come in on.

Once you launch AudioUnitOSC and select parameters, the way to control them is to send a message with the given address sent to the port mentioned above, which contains a list of float values between 0.0 and 1.0, one for each parameter you've selected to control. Each 0-1 value interpolates its associated audiounit parameter between its minimum and maximum values.
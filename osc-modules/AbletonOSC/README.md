## AbletonOSC

This application presents an interface for choosing parameters from your Ableton set and routing OSC messages to them. It is meant to live in between your OSC-sending application and Ableton, facilitating the process of latching parameters without having to derive their addresses yourself (which is somewhat tedious using LiveOSC).

To compile it from source, you need the addons [ofxAbletonLive](https://github.com/genekogan/ofxAbletonLive) and [ofxControl](https://github.com/genekogan/ofxControl).

#### Instructions

1) Install and activate [LiveOSC](http://livecontrol.q3f.org/ableton-liveapi/liveosc/) in Ableton. Follow the instructions there, or:
   - Place the "LiveOSC" folder (inside "trunk") into the Midi Remote Scripts folder in your Live App.
   - On OSX, go to your applications folder, right click Ableton and select "show package contents", then navigate to Contents/App-Resources/MIDI Remote Scripts and paste there.
   - On Windows, paste into \path\to\ableton\Live x.x.x\Resources\MIDI\Remote Scripts
   - Restart Ableton. Go to preferences -> MIDI. LiveOSC should show up in the drop-down menu under "Control Surface". Select it.

2) Compile this application (requires ofxAbletonLive and ofxControl or run the standalone app found in the applications folder.

3) Launch Ableton, and then launch AbletonOSC. AbletonOSC will then parse your current Ableton Live set and return all the available parameters, at which point you can select them from your gui.

4) To control the selected parameters, see the output address in the OSC gui (by default `/wek/outputs`) and port (default 12000), or change them in the file `settings_ableton.xml` found in the data folder.  Sending a multi-value array of parameter values (between 0 and 1) to this address on localhost will change the values. Alternatively, you can change one parameter at a time by sending to `/wek/outputs/#` where \# is the index of the parameter.

5) At this point, changing the outputs in your application should change them in Ableton via AbletonOSC.

6) ♩♪♫♪

#### Fix for Ableton 9.6+

If LiveOSC fails to work in most recent version of Ableton, [this apparently fixes it](http://disq.us/p/15j82c7) -- find the file `socket_live8.py` and comment out the following block of code (lines 74-79):

    # if _have_ssl:
    # _realssl = ssl
    # def ssl(sock, keyfile=None, certfile=None):
    # if hasattr(sock, "_sock"):
    # sock = sock._sock
    # return _realssl(sock, keyfile, certfile)
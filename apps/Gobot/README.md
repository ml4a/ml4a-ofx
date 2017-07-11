## Gobot

[A complete guide to this application can be found here](http://ml4a.github.io/guides/GoBot/)

To build from source, the following addons are required:
- [ofxDarknet](https://github.com/mrzl/ofxDarknet)
- [ofxTemplateMatching](https://github.com/genekogan/ofxTemplateMatching)
- [ofxScreenGrab](https://github.com/genekogan/ofxScreenGrab)

This application assists you in playing competitive Go online. It uses a [convolutional neural policy network](https://pjreddie.com/darknet/darkgo-go-in-darknet/) to "recommend" (i.e. predict) the next move a human player should make. The app uses a screengrabber and OpenCV template matching to scrape board position from your browser and automatically detect the current board position at any point in time. At this time, the CV detection is made specifically to detect pieces from [online-go.com](https://www.online-go.com). It can allegedly play at the level of a [1-dan](https://en.wikipedia.org/wiki/Go_ranks_and_ratings).

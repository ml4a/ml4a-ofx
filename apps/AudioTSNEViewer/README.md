## Audio t-SNE viewer

[A complete guide to this application can be found here](http://ml4a.github.io/guides/AudioTSNEViewer/)

This app requires [ofxJSON](https://github.com/jeffcrouse/ofxJSON)

The above instructions, more succinctly:

First run tSNE-audio.py script to generate the t-SNE. You can do it on an entire directory of sounds, or on a single sound which is segmented by onsets. To run it on a directory of sounds:

	python tSNE-audio.py --input_dir path/to/input/directory --output_file path/to/output/json

for example:

	python tSNE-audio.py --input_dir "/Users/gene/audio/Drum Samples" --output_file ../apps/AudioTSNEViewer/bin/data/points.json

Or you can run it on a single audio file, in which case it will segment the audio by onsets, analyze each chunk and save it to a directory, then analyze those.

	python tSNE-audio.py --input_file path/to/your/input/file --output_audio_dir path/to/output/chunks --output_file path/to/output/json

for example:

	python tSNE-audio.py --input_file /Users/gene/Downloads/bohemianrhapsody.mp3 --output_audio_dir /Users/gene/Desktop/mySounds --output_file ../apps/AudioTSNEViewer/bin/data/points.json

Then run this application, where `path` has been set to the location of your JSON file generated above. Alternatively, name it `points.json` and place it in the data folder, which is the default path.
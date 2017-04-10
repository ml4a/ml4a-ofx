import argparse
import sys
import fnmatch
import os
import numpy as np
import librosa
from sklearn.manifold import TSNE
import json


def process_arguments(args):
	parser = argparse.ArgumentParser(description='tSNE on audio')
	parser.add_argument('--input_file', action='store', help='path to the input file (wav, mp3, etc)')
	parser.add_argument('--output_audio_dir', action='store', help='path to directory to store audio chunks (if using input_file)')
	parser.add_argument('--input_dir', action='store', help='path to directory of input files')
	parser.add_argument('--output_file', action='store', help='path to where to store t-SNE analysis in json')
	parser.add_argument('--num_dimensions', action='store', default=2, help='dimensionality of t-SNE points (default 2)')
	parser.add_argument('--perplexity', action='store', default=150, help='perplexity of t-SNE (default 30)')
	params = vars(parser.parse_args(args))
	return params

def get_audio_files(path, extension):
	files = []
	for root, dirnames, filenames in os.walk(path):
	    for filename in fnmatch.filter(filenames, '*.'+extension):
	        files.append(os.path.join(root, filename))
	return files

def get_features(y, sr):
	y = y[0:sr] 	# analyze just first second
	#S = librosa.feature.melspectrogram(y, sr=sr, n_mels=128)
	#S = librosa.feature.mfcc(y, sr=sr)
	#log_S = librosa.logamplitude(S, ref_power=np.max)
	S = librosa.feature.melspectrogram(y, sr=sr, n_mels=128)
	log_S = librosa.logamplitude(S, ref_power=np.max)
	mfcc = librosa.feature.mfcc(S=log_S, n_mfcc=13)
	delta_mfcc = librosa.feature.delta(mfcc)
	delta2_mfcc = librosa.feature.delta(mfcc, order=2)
	#mean_mfcc = np.mean(S, 1)
	#mean_mfcc = (mean_mfcc-np.mean(mean_mfcc))/np.std(mean_mfcc)
	#var_mfcc = np.var(S, 1)
	#var_mfcc = (var_mfcc-np.mean(var_mfcc))/np.std(var_mfcc)
	#feature_vector = np.concatenate((mean_mfcc, var_mfcc))
	feature_vector = np.concatenate((np.mean(mfcc,1), np.mean(delta_mfcc,1), np.mean(delta2_mfcc,1)))
	feature_vector = (feature_vector-np.mean(feature_vector)) / np.std(feature_vector)
	return feature_vector

def segment_analyze_audio_file(source_audio, save_path_audio, hop_length=512):
	y, sr = librosa.load(source_audio)
	onsets = librosa.onset.onset_detect(y=y, sr=sr, hop_length=hop_length)
	if os.path.isdir(save_path_audio) == False:
		os.system('mkdir %s'%save_path_audio)
	feature_vectors = []
	for i in range(len(onsets)-1):
		idx_y1 = onsets[i] * hop_length
		idx_y2 = onsets[i+1] * hop_length
		y_ = y[idx_y1:idx_y2]
		feat = get_features(y_, sr)
		file_path = '%s/onset_%d.wav' % (save_path_audio, i)
		feature_vectors.append({"file":file_path, "features":feat})
		librosa.output.write_wav(file_path, y_, sr)
		print("analyzed %d/%d = %s"%(i, len(onsets)-1, file_path))
	return feature_vectors

def analyze_directory(source_audio):	
	files = get_audio_files(source_audio, 'wav')
	feature_vectors = []
	for i,f in enumerate(files):
		print("analyze: %d/%d = %s"%(i, len(files), f))
		y, sr = librosa.load(f)
		feat = get_features(y, sr)
		feature_vectors.append({"file":f, "features":feat})
	return feature_vectors

def run_tSNE(feature_vectors, tsne_path, tsne_dimensions, tsne_perplexity=30):
	tsne = TSNE(n_components=tsne_dimensions, learning_rate=200, perplexity=tsne_perplexity, verbose=2, angle=0.1).fit_transform([f["features"] for f in feature_vectors])
	data = []
	for i,f in enumerate(feature_vectors):
		point = [ (tsne[i,k] - np.min(tsne[:,k]))/(np.max(tsne[:,k]) - np.min(tsne[:,k])) for k in range(tsne_dimensions) ]
		data.append({"path":os.path.abspath(f["file"]), "point":point})
	with open(tsne_path, 'w') as outfile:
		json.dump(data, outfile)


if __name__ == '__main__':
	params = process_arguments(sys.argv[1:])
	output_file_path = params['output_file']
	tsne_dimensions = int(params['num_dimensions'])
	tsne_perplexity = int(params['perplexity'])
	if params['input_file'] is not None:
		source_audio = params['input_file']
		output_audio_dir = params['output_audio_dir']
		print("segmenting and getting feature vectors from %s, putting audio chunks into %s"%(source_audio,output_audio_dir))
		feature_vectors = segment_analyze_audio_file(source_audio, output_audio_dir, 512)
		run_tSNE(feature_vectors, output_file_path, tsne_dimensions, tsne_perplexity)
		print("finished saving %s"%output_file_path)
	elif params['input_dir'] is not None:
		source_audio = params['input_dir']
		print("getting feature vectors from audio in %s"%source_audio)
		feature_vectors = analyze_directory(source_audio)
		run_tSNE(feature_vectors, output_file_path, tsne_dimensions, tsne_perplexity)
		print("finished saving %s"%output_file_path)
	else:
		print("Error: no input specified!")

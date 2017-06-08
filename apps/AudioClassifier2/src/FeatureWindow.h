#include "ofMain.h"

class FeatureWindow {
public:
    void setup(int d, int n) {
        this->d = d;
        this->n = n;
        mean.resize(d);
        std.resize(d);
    }
    
    void update(double * feat) {
        vector<double> frame;
        for (int i=0; i<d; i++) {
            frame.push_back(feat[i]);
        }
        frames.push_back(frame);
        if (frames.size() > n) {
            frames.erase(frames.begin());
        }
        calcMean();
        calcStd();
    }
    
    void calcMean() {
        vector<double> sums(d);
        for (int i=0; i<d; i++) {
            sums[i] = 0.0;
            for (int f=0; f<frames.size(); f++) {
                sums[i] += frames[f][i];
            }
            mean[i] = sums[i] / frames.size();
        }
    }
    
    void calcStd() {
        for (int i=0; i<d; i++) {
            std[i] = 0.0;
            for (int f=0; f<frames.size(); f++) {
                float diff = frames[f][i] - mean[i];
                std[i] += (diff * diff);
            }
            std[i] = sqrt(std[i]/frames.size());
        }
    }
    
    VectorFloat getFeatureVector() {
        VectorFloat inputVector(d * 2);
        for (int i=0; i<d; i++) {
            inputVector[i] = mean[i];
            inputVector[d+i] = std[i];
        }
        return inputVector;
    }
    
    vector<vector<double> > frames;
    vector<double> mean, std;
    int n, d;
    
};

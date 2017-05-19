#pragma once

#include "ofMain.h"

class KeyInput {
    public:
        KeyInput();
        ~KeyInput();
        

        void setup(int _keyCode, string _keyString, int _oscId);

        
    string keyString;
    int keyCode;
    int oscId;
        
};

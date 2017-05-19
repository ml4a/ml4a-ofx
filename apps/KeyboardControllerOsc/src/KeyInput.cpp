#include "KeyInput.h"

KeyInput::KeyInput() {
    int keyCode;
    string keyString;
    int oscId;
}

KeyInput::~KeyInput() {
    
}

void KeyInput::setup(int _keyCode, string _keyString, int _oscId) {
    keyCode = _keyCode;
    keyString = _keyString;
    oscId = _oscId;
}

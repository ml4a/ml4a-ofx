#include "KeyCodes.h"


KeyCodes::KeyCodes() {

}

KeyCodes::~KeyCodes() {
    
}

int KeyCodes::convertStringToKeyCode(string keyString) {
    int keyId = -1;
    
    if (keyString == "a") {
        keyId = 0;
    }
    if (keyString == "s") {
        keyId = 1;
    }
    if (keyString == "d") {
        keyId = 2;
    }
    if (keyString == "f") {
        keyId = 3;
    }
    if (keyString == "h") {
        keyId = 4;
    }
    if (keyString == "g") {
        keyId = 5;
    }
    if (keyString == "z") {
        keyId = 6;
    }
    if (keyString == "x") {
        keyId = 7;
    }
    if (keyString == "c") {
        keyId = 8;
    }
    if (keyString == "v") {
        keyId = 9;
    }
    if (keyString == "b") {
        keyId = 11;
    }
    if (keyString == "q") {
        keyId = 12;
    }
    if (keyString == "w") {
        keyId = 13;
    }
    if (keyString == "e") {
        keyId = 14;
    }
    if (keyString == "r") {
        keyId = 15;
    }
    if (keyString == "y") {
        keyId = 16;
    }
    if (keyString == "t") {
        keyId = 17;
    }
    if (keyString == "1") {
        keyId = 18;
    }
    if (keyString == "2") {
        keyId = 19;
    }
    if (keyString == "3") {
        keyId = 20;
    }
    if (keyString == "4") {
        keyId = 21;
    }
    if (keyString == "6") {
        keyId = 22;
    }
    if (keyString == "5") {
        keyId = 23;
    }
    if (keyString == "=") {
        keyId = 24;
    }
    if (keyString == "9") {
        keyId = 25;
    }
    if (keyString == "7") {
        keyId = 26;
    }
    if (keyString == "-") {
        keyId = 27;
    }
    if (keyString == "8") {
        keyId = 28;
    }
    if (keyString == "0") {
        keyId = 29;
    }
    if (keyString == "]") {
        keyId = 30;
    }
    if (keyString == "o") {
        keyId = 31;
    }
    if (keyString == "u") {
        keyId = 32;
    }
    if (keyString == "[") {
        keyId = 33;
    }
    if (keyString == "i") {
        keyId = 34;
    }
    if (keyString == "p") {
        keyId = 35;
    }
    if (keyString == "RETURN") {
        keyId = 36;
    }
    if (keyString == "l") {
        keyId = 37;
    }
    if (keyString == "j") {
        keyId = 38;
    }
    if (keyString == "'") {
        keyId = 39;
    }
    if (keyString == "k") {
        keyId = 40;
    }
    if (keyString == ";") {
        keyId = 41;
    }
    if (keyString == "\\") {
        keyId = 42;
    }
    if (keyString == ",") {
        keyId = 43;
    }
    if (keyString == "/") {
        keyId = 44;
    }
    if (keyString == "n") {
        keyId = 45;
    }
    if (keyString == "m") {
        keyId = 46;
    }
    if (keyString == ".") {
        keyId = 47;
    }
    if (keyString == "TAB") {
        keyId = 48;
    }
    if (keyString == " " || keyString == "SPACE") {
        keyId = 49;
    }
    if (keyString == "`") {
        keyId = 50;
    }
    if (keyString == "DELETE") {
        keyId = 51;
    }
    if (keyString == "ENTER") {
        keyId = 52;
    }
    if (keyString == "ESCAPE") {
        keyId = 53;
    }
    if (keyString == ".") {
        keyId = 65;
    }
    if (keyString == "*") {
        keyId = 67;
    }
    if (keyString == "+") {
        keyId = 69;
    }
    if (keyString == "CLEAR") {
        keyId = 71;
    }
    if (keyString == "/") {
        keyId = 75;
    }
    if (keyString == "ENTER") {
        keyId = 76;
    }
    if (keyString == "-") {
        keyId = 78;
    }
    if (keyString == "=") {
        keyId = 81;
    }
    if (keyString == "0") {
        keyId = 82;
    }
    if (keyString == "1") {
        keyId = 83;
    }
    if (keyString == "2") {
        keyId = 84;
    }
    if (keyString == "3") {
        keyId = 85;
    }
    if (keyString == "4") {
        keyId = 86;
    }
    if (keyString == "5") {
        keyId = 87;
    }
    if (keyString == "6") {
        keyId = 88;
    }
    if (keyString == "7") {
        keyId = 89;
    }
    if (keyString == "8") {
        keyId = 91;
    }
    if (keyString == "9") {
        keyId = 92;
    }
    if (keyString == "F5") {
        keyId = 96;
    }
    if (keyString == "F6") {
        keyId = 97;
    }
    if (keyString == "F7") {
        keyId = 98;
    }
    if (keyString == "F3") {
        keyId = 99;
    }
    if (keyString == "F8") {
        keyId = 100;
    }
    if (keyString == "F9") {
        keyId = 101;
    }
    if (keyString == "F11") {
        keyId = 103;
    }
    if (keyString == "F13") {
        keyId = 105;
    }
    if (keyString == "F14") {
        keyId = 107;
    }
    if (keyString == "F10") {
        keyId = 109;
    }
    if (keyString == "F12") {
        keyId = 111;
    }
    if (keyString == "F15") {
        keyId = 113;
    }
    if (keyString == "HELP") {
        keyId = 114;
    }
    if (keyString == "HOME") {
        keyId = 115;
    }
    if (keyString == "PGUP") {
        keyId = 116;
    }
    if (keyString == "DELETE") {
        keyId = 117;
    }
    if (keyString == "F4") {
        keyId = 118;
    }
    if (keyString == "END") {
        keyId = 119;
    }
    if (keyString == "F2") {
        keyId = 120;
    }
    if (keyString == "PGDN") {
        keyId = 121;
    }
    if (keyString == "F1") {
        keyId = 122;
    }
    if (keyString == "LEFT") {
        keyId = 123;
    }
    if (keyString == "RIGHT") {
        keyId = 124;
    }
    if (keyString == "DOWN") {
        keyId = 125;
    }
    if (keyString == "UP") {
        keyId = 126;
    }
    
    return keyId;
}

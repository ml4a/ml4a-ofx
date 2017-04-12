#include "ofApp.h"

void ofApp::setup()
{
    std::string cfgfile = ofToDataPath( "cfg/darknet.cfg" );
    std::string weightfile = "/Users/gene/Learn/darknet_old/darknet.weights";
    std::string nameslist = ofToDataPath( "cfg/imagenet.shortnames.list" );
    
//    darknet.init( cfgfile, weightfile, nameslist );
    
    video.setDeviceID( 0 );
    video.setDesiredFrameRate( 30 );
    video.initGrabber( 640, 480 );
}

void ofApp::update()
{
//    ofLog() << ofGetFrameRate();
    video.update();
}

void ofApp::draw()
{
    video.draw( 0, 0 );
    /*
    if( video.isFrameNew() ) {
        classifications = darknet.classify( video.getPixels() );
    }
    
    int offset = 20;
    for( classification c : classifications )
    {
        std::stringstream ss;
        ss << c.label << " : " << ofToString( c.probability );
        ofDrawBitmapStringHighlight( ss.str(), 20, offset );
        offset += 20;
    }*/
}

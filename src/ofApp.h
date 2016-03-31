#pragma once

#include "ofMain.h"
#include "ofxWatchFile.h"
#include "gdbMemoryDump.h"
#include "Views.h"


class ofApp : public ofBaseApp{
    
    typedef struct {
        ofVec2f ulc;
        ofVec2f size;
    } clickableArea_t;

	public:
		void setup();
		void update();
		void draw();

        void copyDumpDataToOfVec2fVectors();
        void getRuntimeConfigAsString();
//        void ofApp::checkForNewData(ofxWatchFile file);
//        void readNewDataFromFileDump(string file_name);
//        float convertLittleEndianFloatAsBytesToBigEndianFloat(char * byte_ptr, uint32_t pos);
//        float getFloatFromBytes(char * byte_ptr, uint32_t pos);
        void setupViews();
    
        void checkSerial();
        void parseSerial(string line);
        int getDebugMessageType(string debugID);
        void addClickableArea(ofVec2f ulc, ofVec2f size, void (*func_cb)(void));
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    private:
//        string linebuffer;
        vector<clickableArea_t> clickable_areas;
		
};
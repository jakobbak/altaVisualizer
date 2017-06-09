#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

        void copyDumpDataToOfVec2fVectors();
        void getRuntimeConfigAsString();
        void setupViews();
        void updatePrinterStatus();
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
    
};


typedef struct {
    float alive;
    float hotend;
    float setpoint;
    int node_buffer;
    // int mode, etc....
} printer_t;

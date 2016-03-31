//
//  altaSerial.h
//  altaVisualizer
//
//  Created by Jakob Bak on 30/03/16.
//
//

#ifndef ALTASERIAL_H
#define ALTASERIAL_H

#include <stdio.h>
#include <ofThread.h>
#include "ofApp.h"
#include "Views.h"

class altaSerial : public ofThread {
    
public:
    void init();
    void checkSerial();
    void parseSerial(string line);
    int getDebugMessageType(string debugID);
    void addRealtimePlotter(ViewRealtimePlotter2D * plotter) { realtime_plotter = plotter; }
    
    // the thread function
    void threadedFunction() {
        
        // start
        
        while(isThreadRunning()) {
            
            checkSerial();
        }
        
        // done
    }
    
protected:
    string linebuffer;
    ViewRealtimePlotter2D * realtime_plotter;

    
};



#endif /* ALTASERIAL_H */

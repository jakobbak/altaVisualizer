//
//  CheckSerial.hpp
//  altaVisualizer
//
//  Created by Jakob Bak on 30/03/16.
//
//

#ifndef CHECKSERIAL_H
#define CHECKSERIAL_H

#include <stdio.h>
#include <ofThread.h>
#include "ofApp.h"

class CheckSerial : public ofThread {
    
    // the thread function
    void threadedFunction() {
        
        // start
        
        while(isThreadRunning()) {
            
//            checkSerial();
        }
        
        // done
    }
    
};



#endif /* CHECKSERIAL_H */

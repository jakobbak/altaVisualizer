#include <stdio.h>
#include <string.h>
#include "ofApp.h"
//#include <ofSerial.h>
#include "altaSerial.h"

//ofSerial serial;

gdbMemoryDump dump;
string dumpFileName = "flash_sector_11";

//vector<ofVec2f> axisTable;
//vector<ofVec2f> phaseReference;
//vector<ofVec2f> models;

vector<ofVec2f> data_arm_phase_reference;
vector<ofVec2f> data_platter_phase_reference;
vector<ofVec2f> data_arm_axis_table_nominals;
vector<ofVec2f> data_platter_axis_table_nominals;
vector<ofVec2f> data_arm_ecm;
vector<ofVec2f> data_platter_ecm;
string runtime_config_info;

ofFile random_file;
string random_text;

ViewGraph2D view_arm_phase_reference;
ViewGraph2D view_platter_phase_reference;
ViewGraph2D view_arm_axis_table_nominals;
ViewGraph2D view_platter_axis_table_nominals;
ViewGraph2D view_arm_ecm;
ViewGraph2D view_platter_ecm;
ViewRealtimePlotter2D view_realtime_plotter;

ViewTextBox view_config_info;

altaSerial alta;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofBackground(0, 0, 0);
    
    dump.readNewDataFromFileDump(dumpFileName);
    copyDumpDataToOfVec2fVectors();
    getRuntimeConfigAsString();
    
    setupViews();
    
    alta.addRealtimePlotter(&view_realtime_plotter);
    alta.init();
    alta.startThread();
    
//    serial.listDevices();
//    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
//    serial.setup(0, 115200); //open the first device and talk to it at 57600 baud
//    
//    linebuffer = "";

}


//--------------------------------------------------------------
void ofApp::update(){
//    checkSerial();

}


//--------------------------------------------------------------
void ofApp::draw(){

//    ofDrawBitmapStringHighlight("Everything is cool - stay tuned!", 20, 20);
    view_arm_phase_reference.draw();
    view_platter_phase_reference.draw();
    view_arm_axis_table_nominals.draw();
    view_platter_axis_table_nominals.draw();
    view_arm_ecm.draw();
    view_platter_ecm.draw();
    view_config_info.draw();
    view_realtime_plotter.draw();
}


void ofApp::setupViews(){
    view_arm_phase_reference = ViewGraph2D(ofVec2f(20,20), ofVec2f(400, 400), data_arm_phase_reference, PLOT2D_LINE, "ARM PHASE REFERENCE");
    view_platter_phase_reference = ViewGraph2D(ofVec2f(440,20), ofVec2f(400, 400), data_platter_phase_reference, PLOT2D_LINE, "PLATTER PHASE REFERENCE");
    view_arm_axis_table_nominals = ViewGraph2D(ofVec2f(860,20), ofVec2f(400, 400), data_arm_axis_table_nominals, PLOT2D_LINE, "ARM AXIS TABLE NOMINALS");
    view_platter_axis_table_nominals = ViewGraph2D(ofVec2f(1280,20), ofVec2f(400, 400), data_platter_axis_table_nominals, PLOT2D_LINE, "PLATTER AXIS TABLE NOMINALS");
    view_arm_ecm = ViewGraph2D(ofVec2f(20,440), ofVec2f(820,400), data_arm_ecm, PLOT2D_SCATTER, "ARM ECM");
    view_platter_ecm = ViewGraph2D(ofVec2f(860,440), ofVec2f(820,400), data_platter_ecm, PLOT2D_SCATTER, "PLATTER ECM");

    view_realtime_plotter = ViewRealtimePlotter2D(ofVec2f(20, 860), ofVec2f(1660, 400), "REALTIME LOGGING");
    view_config_info = ViewTextBox(ofVec2f(20,1280), ofVec2f(400,120), runtime_config_info);
    
    view_arm_phase_reference.changeGraphColor(ofColor(255,0,0));
    view_platter_phase_reference.changeGraphColor(ofColor(255,0,0));
    view_arm_axis_table_nominals.changeGraphColor(ofColor(0, 196, 0));
    view_platter_axis_table_nominals.changeGraphColor(ofColor(0, 196, 0));
    view_arm_ecm.changeGraphColor(ofColor(0, 128, 255));
    view_platter_ecm.changeGraphColor(ofColor(0, 128, 255));
    
    view_realtime_plotter.setGraphsCentered(true);
    
//    random_file.open(ofToDataPath("random.txt"), ofFile::ReadOnly, false);
//    ofBuffer random_buff = random_file.readToBuffer();
//    random_text = random_buff.getText();
}




void ofApp::copyDumpDataToOfVec2fVectors() {
    
    // making a copy of Axis Tables Nominals for the ARM that we can pass to the visualisation
    int size_arm_axis_table_nominals = sizeof(dump.memory.axis_table[0].nominals)/sizeof(dump.memory.axis_table[0].nominals[0]);
    float * ptr_arm_axis_table_nominals = dump.memory.axis_table[0].nominals;
    for(int i=0; i < size_arm_axis_table_nominals; i++) {
        data_arm_axis_table_nominals.push_back(ofVec2f(i, ptr_arm_axis_table_nominals[i]));
    }
    
    // making a copy of Axis Tables Nominals for the PLATTER that we can pass to the visualisation
    int size_platter_axis_table_nominals = sizeof(dump.memory.axis_table[1].nominals)/sizeof(dump.memory.axis_table[1].nominals[0]);
    float * ptr_platter_axis_table_nominals = dump.memory.axis_table[1].nominals;
    for(int i=0; i < size_platter_axis_table_nominals; i++) {
        data_platter_axis_table_nominals.push_back(ofVec2f(i, ptr_platter_axis_table_nominals[i]));
    }
    
    // making a copy of Phase References for the ARM that we can pass to the visualisation
    int size_arm_phase_reference = sizeof(dump.memory.phase_reference[0])/sizeof(dump.memory.phase_reference[0][0]);
    float * ptr_arm_phase_reference = dump.memory.phase_reference[0];
    for(int i=0; i < size_arm_phase_reference; i++) {
        data_arm_phase_reference.push_back(ofVec2f(i, ptr_arm_phase_reference[i]));
        // below is for checking whether the values actually differentiate in respect to ideal positions
        //        float apr = ptr_arm_phase_reference[i];
        //        float ideal_position = (M_PI * 2 * (float)i / 200);
        //        if(i >= 200) ideal_position = 0;
        //        float filtered_value = apr - ideal_position;
        //        cout << "apr=" << apr << "\t ip=" << ideal_position << "\t fv=" << filtered_value << endl;
        //        arm_phase_reference.push_back(ofVec2f(i, filtered_value));
    }
    
    // making a copy of Phase References for the PLATTER that we can pass to the visualisation
    int size_platter_phase_reference = sizeof(dump.memory.phase_reference[1])/sizeof(dump.memory.phase_reference[1][0]);
    float * ptr_platter_phase_reference = dump.memory.phase_reference[1];
    for(int i=0; i< size_platter_phase_reference; i++) {
        data_platter_phase_reference.push_back(ofVec2f(i, ptr_platter_phase_reference[i]));
        // below is for checking whether the values actually differentiate in respect to ideal positions
        //        float apr = ptr_platter_phase_reference[i];
        //        if(i == 0) apr = 0;
        //        float ideal_position = (M_PI * 2 * (float)i / 200);
        //        if(i >= 200) ideal_position = 0;
        //        float filtered_value = apr - ideal_position;
        //        cout << "apr=" << apr << "\t ip=" << ideal_position << "\t fv=" << filtered_value << endl;
        //        platter_phase_reference.push_back(ofVec2f(i, filtered_value));
    }
    
    
    // making a copy of ECM Samples for the ARM that we can pass to the visualisation
    gdbMemoryDump::sample_t * ptr_arm_ecm = dump.memory.models[0].samples;
    int size_arm_ecm = sizeof(dump.memory.models[0].samples)/sizeof(dump.memory.models[0].samples[0]);
    int nan_counter = 0;
    for(int i=0; i< size_arm_ecm; i++) {
        //        dump.printDataBytesAsFloats(&ptr_arm_ecm[i], 8);
        float offset = ptr_arm_ecm->offset;
        float acceleration = ptr_arm_ecm->acceleration;
        if(isnan(offset) || isnan(acceleration)) { // check if any are NaN, meaning they were not set in the flash sector.
//            data_arm_ecm.push_back(ofVec2f(i, i));
            nan_counter++;
        }
//        else data_arm_ecm.push_back(ofVec2f(offset, acceleration)); // <--- THIS IS THE LINE WE SHOULD USE INSTEAD OF BELOW
        else data_arm_ecm.push_back(ofVec2f(i - size_arm_ecm/2 - 10, i - size_arm_ecm/2 + 10));  // JUST FOR TESTING. DELETE WHEN WE GOT WORKING DATA
    }
    cout << "ARM ECM has " << dec << size_arm_ecm << " samples. " << dec << nan_counter << " of them are NaN" << endl;
    
    
    // making a copy of ECM Samples for the PLATTER that we can pass to the visualisation
    gdbMemoryDump::sample_t * ptr_platter_ecm = dump.memory.models[1].samples;
    int size_platter_ecm = sizeof(dump.memory.models[1].samples)/sizeof(dump.memory.models[1].samples[0]);
    nan_counter = 0;
    for(int i=0; i< size_platter_ecm; i++) {
        //        dump.printDataBytesAsFloats(&ptr_platter_ecm[i], 8);
        float offset = ptr_platter_ecm->offset;
        float acceleration = ptr_platter_ecm->acceleration;
        if(isnan(offset) || isnan(acceleration)) { // check if any are NaN, meaning they were not set in the flash sector.
//            data_platter_ecm.push_back(ofVec2f(i, i));
            nan_counter++;
        }
//        else data_platter_ecm.push_back(ofVec2f(offset, acceleration)); // <--- THIS IS THE LINE WE SHOULD USE INSTEAD OF BELOW
        else data_platter_ecm.push_back(ofVec2f(i - size_platter_ecm/2 - 40, i - size_platter_ecm/2 - 10));  // JUST FOR TESTING. DELETE WHEN WE GOT WORKING DATA
    }
    cout << "PLATTER ECM has " << dec << size_platter_ecm << " samples. " << dec << nan_counter << " of them are NaN" << endl;
    
    
    
}



void ofApp::getRuntimeConfigAsString() {
    gdbMemoryDump::runtime_config_t * runtime_config_ptr = &dump.memory.config;
//    char * buff[256];
    ostringstream buff;
    buff << "Phases per Revolution:  ";
    buff << "ARM=" << dec << runtime_config_ptr->phases_per_revolution[0];
    buff << "  PLATTER=" << dec << runtime_config_ptr->phases_per_revolution[1] << endl;
    buff << "Arm Home Position: " << runtime_config_ptr->arm_home_position << endl;
    buff << "Arm Clockwise Extent: " << runtime_config_ptr->arm_clockwise_extent << endl;
    buff << "Arm Anti-Clockwise Extent: " << runtime_config_ptr->arm_anti_clockwise_extent << endl;
    
    runtime_config_info = buff.str();
//    cout << runtime_config_info << endl;
    
}



//void ofApp::checkSerial() {
//    stringstream inbuffer;
//    inbuffer << linebuffer;
//    while(serial.available()) {
//        char in_char;
//        in_char = serial.readByte();
//        if(in_char == '\r') continue;
//        if(in_char == OF_SERIAL_NO_DATA) {
//            cout << "no data was read" << endl;
//            return;
//        } else if(in_char == OF_SERIAL_ERROR) {
//            cout << "an error occurred" << endl;
//            return;
//        } else {
//            inbuffer << in_char;
//        }
//    }
//    if(!inbuffer.str().empty()) {
//        string newline = inbuffer.str();
//        vector<string> newlines = ofSplitString(newline, "\n", false, false);
//        int nindex;
//        int numlines = newlines.size();
//        for(int i=0; i < numlines-1; i++) {
//            // parse the line here
//            parseSerial(newlines[i]);
////            cout << "[" << i << "] " << newlines[i] << endl;
////            nindex = i;
//        }
//        
//        linebuffer = newlines[numlines-1];
////        cout << "nindex is " << nindex << endl;
////        cout << "remaining text is: " << linebuffer << endl;
//        inbuffer.str("");
//    }
//    
//}
//
//void ofApp::parseSerial(string line) {
//    if(ofIsStringInString(line, "_DEBUG")) {
//        vector<string> debug_msg = ofSplitString(line, "@");
//        int debugID = getDebugMessageType(debug_msg[0]);
//        switch(debugID) {
//            case 0:
//            {
//                vector<string> runtime_value = ofSplitString(debug_msg[1], " ");
//                for(int i=0; i < runtime_value.size(); i++) {
////                    cout << "[" << i << "]" << runtime_value[i] << endl;
//                }
//                string label = runtime_value[0];
//                float value;
//                try {
//                    value = stof(runtime_value[1]);
//                } catch (invalid_argument ia) {
//                    break;
//                }
//                view_realtime_plotter.addDataPoint(label, value);
////                cout << label << "=" << defaultfloat << setprecision(5) << value << endl;
//            }
//                break;
//            case 1:
//                break;
//            default:
//                break;
//        }
//        
//    } else {
//        cout << line << endl;
//    }
//}
//
//int ofApp::getDebugMessageType(string debugID) {
//    if(debugID == "_DEBUG_") return 0;
//    else return 0;
//}
//
//
//



void ofApp::addClickableArea(ofVec2f ulc, ofVec2f size, void (*func_cb)(void)) {
    clickableArea_t area;
    area.ulc = ulc;
    area.size = size;
    clickable_areas.push_back(area);
}




//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') view_realtime_plotter.setViewUpdate(false);
    if(key == '1') view_realtime_plotter.toggleGraphVisibility(0);
    if(key == '2') view_realtime_plotter.toggleGraphVisibility(1);
    if(key == '3') view_realtime_plotter.toggleGraphVisibility(2);
    if(key == '4') view_realtime_plotter.toggleGraphVisibility(3);
    if(key == '5') view_realtime_plotter.toggleGraphVisibility(4);
    if(key == '6') view_realtime_plotter.toggleGraphVisibility(5);
    if(key == '7') view_realtime_plotter.toggleGraphVisibility(6);
    if(key == '8') view_realtime_plotter.toggleGraphVisibility(7);
    if(key == '9') view_realtime_plotter.toggleGraphVisibility(8);

    if(key == '-') {
        float current_time_span = view_realtime_plotter.getTimeSpan();
        current_time_span *= 2;
        if(current_time_span > 1200000) current_time_span = 1200000;
        view_realtime_plotter.setTimeSpan(current_time_span);
    }
    if(key == '+') {
        float current_time_span = view_realtime_plotter.getTimeSpan();
        current_time_span /= 2;
        if(current_time_span < 1000) current_time_span = 1000;
        view_realtime_plotter.setTimeSpan(current_time_span);
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' ') view_realtime_plotter.setViewUpdate(true);

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    cout << "PRESSED BUTTON " << button << "\t at coordinates: " << x << " " << y << endl;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//void ofApp::exit() {
//    
//    // stop the thread
//    alta.stopThread();
//}

#include <stdio.h>
#include <string.h>
//#include "ofxWatchFile.h"
#include "gdbMemoryDump.h"
#include "Views.h"
#include "ofApp.h"
#include "AltaSerial.h"

gdbMemoryDump dump;
string dumpFileName = "flash_sector_11";

vector<ofVec2f> data_arm_phase_reference;
vector<ofVec2f> data_platter_phase_reference;
vector<ofVec2f> data_arm_axis_table_nominals;
vector<ofVec2f> data_platter_axis_table_nominals;
vector<ofVec3f> data_arm_ecm;
vector<ofVec3f> data_platter_ecm;
vector<ofVec3f> data_psamples_ecm;
vector<vector<ofVec3f>> data_asamples3D_ecm;
vector<vector<ofVec3f>> data_psamples3D_ecm;
vector<vector<ofVec3f>> data_pacc_vel_ecm;
vector<ofVec3f> data_trend_arm_ecm;
vector<ofVec3f> data_trend_platter_ecm;
string runtime_config_info;
string temperature_string;
string node_buffer_string;
printer_t printer;
vector<ofVec4f> dump_for_simen;

ViewGraph2D view_arm_phase_reference;
ViewGraph2D view_platter_phase_reference;
ViewGraph2D view_arm_axis_table_nominals;
ViewGraph2D view_platter_axis_table_nominals;
ViewGraph3D view_arm_ecm;
ViewGraph3D view_platter_ecm;
ViewGraph3D view_pacc_vel_ecm;
ViewRealtimePlotter2D view_realtime_plotter;

ViewTextBox view_config_info;
ViewTextBox view_temperature;
ViewTextBox view_node_buffer;

AltaSerial alta;

bool table_nominals_compare_to_ideal_positions = true;
bool phase_reference_compare_to_ideal_positions = true;

int table_nominals_points_to_compare = 256;
int phase_reference_points_to_compare = 50;


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofBackground(0, 0, 0);
    
    dump.readNewDataFromFileDump(dumpFileName);
    copyDumpDataToOfVec2fVectors();
    getRuntimeConfigAsString();
    
    temperature_string = "273.1 / 273.1";
    node_buffer_string = "56789";

    setupViews();
    
    alta.addRealtimePlotter(&view_realtime_plotter);
    alta.addPrinter(&printer);
    alta.init();
    alta.startThread();
}


//--------------------------------------------------------------
void ofApp::update(){
    updatePrinterStatus();
}


//--------------------------------------------------------------
void ofApp::draw(){
    printer.alive *= 1.05;
    if(printer.alive >= 1.0) printer.alive = 1.0;
    ofBackground(int(printer.alive * 0x80), 0, 0);

    view_arm_phase_reference.draw();
    view_platter_phase_reference.draw();
    view_arm_axis_table_nominals.draw();
    view_platter_axis_table_nominals.draw();
//    view_arm_ecm.draw();
    view_platter_ecm.draw();
    view_pacc_vel_ecm.draw();
    view_realtime_plotter.draw();
    view_config_info.draw();
    view_temperature.draw();
    view_node_buffer.draw();
}


void ofApp::setupViews(){
    view_arm_phase_reference = ViewGraph2D(ofVec2f(20,20), ofVec2f(400, 400), data_arm_phase_reference, PLOT2D_LINE, "ARM PHASE REFERENCE");
    view_platter_phase_reference = ViewGraph2D(ofVec2f(440,20), ofVec2f(400, 400), data_platter_phase_reference, PLOT2D_LINE, "PLATTER PHASE REFERENCE");
    view_arm_axis_table_nominals = ViewGraph2D(ofVec2f(860,20), ofVec2f(400, 400), data_arm_axis_table_nominals, PLOT2D_LINE, "ARM AXIS TABLE NOMINALS");
    view_platter_axis_table_nominals = ViewGraph2D(ofVec2f(1280,20), ofVec2f(400, 400), data_platter_axis_table_nominals, PLOT2D_LINE, "PLATTER AXIS TABLE NOMINALS");
//    view_arm_ecm = ViewGraph3D(ofVec2f(20,440), ofVec2f(820,400), data_asamples3D_ecm, PLOT3D_SCATTER, "ARM ECM");
//    view_platter_ecm = ViewGraph3D(ofVec2f(860,440), ofVec2f(820,400), data_platter_ecm, PLOT3D_SCATTER, "PLATTER ECM");
//    view_platter_ecm = ViewGraph3D(ofVec2f(860,440), ofVec2f(820,400), data_psamples3D_ecm, PLOT3D_SCATTER, "PSAMPLES ECM");
    view_platter_ecm = ViewGraph3D(ofVec2f(20,440), ofVec2f(820,400), data_psamples3D_ecm, PLOT3D_SCATTER, "PSAMPLES ECM");
    view_pacc_vel_ecm = ViewGraph3D(ofVec2f(860,440), ofVec2f(820,400), data_pacc_vel_ecm, PLOT3D_SCATTER, "PSAMPLES ECM");

    view_realtime_plotter = ViewRealtimePlotter2D(ofVec2f(20, 860), ofVec2f(1660, 400), "REALTIME LOGGING");
    view_config_info = ViewTextBox(ofVec2f(20,1280), ofVec2f(400,120), runtime_config_info);
    
    view_temperature = ViewTextBox(ofVec2f(440,1280), ofVec2f(400,120), temperature_string);
    view_temperature.changeTitle("TEMPERATURE");
    view_temperature.changeFontSize(48);

    view_node_buffer = ViewTextBox(ofVec2f(860,1280), ofVec2f(140,120), node_buffer_string);
    view_node_buffer.changeTitle("BUFFER");
    view_node_buffer.changeFontSize(48);
    
    view_arm_phase_reference.changeGraphColor(ofColor(255,0,0));
    view_platter_phase_reference.changeGraphColor(ofColor(255,0,0));
    view_arm_axis_table_nominals.changeGraphColor(ofColor(0, 196, 0));
    view_platter_axis_table_nominals.changeGraphColor(ofColor(0, 196, 0));
//    view_arm_ecm.changeGraphColor(ofColor(0, 128, 255));
    view_platter_ecm.changeGraphColor(ofColor(0, 128, 255));
    
//    view_arm_ecm.changeCircleRadius(2);
    view_platter_ecm.changeCircleRadius(2);
    view_pacc_vel_ecm.changeCircleRadius(2);
    view_realtime_plotter.setGraphsCentered(true);
    
    view_platter_ecm.updateCurveFittingParameters(data_trend_platter_ecm);
}




void ofApp::copyDumpDataToOfVec2fVectors() {
    
    // making a copy of Axis Tables Nominals for the ARM that we can pass to the visualisation
    int size_arm_axis_table_nominals = table_nominals_points_to_compare;

//    int size_arm_axis_table_nominals = sizeof(dump.memory.axis_table[0].nominals)/sizeof(dump.memory.axis_table[0].nominals[0]);
    float * ptr_arm_axis_table_nominals = dump.memory.axis_table[0].nominals;
    for(int i=0; i < size_arm_axis_table_nominals; i++) {
        if(table_nominals_compare_to_ideal_positions) {
            static int index_offset = 0;
            // below is for checking whether the values actually differentiate in respect to ideal positions
            float atn = ptr_arm_axis_table_nominals[i];
            float ideal_position = (M_PI * 2 * (float)i / table_nominals_points_to_compare);
            if(i >= table_nominals_points_to_compare) ideal_position = 0;
            float filtered_value = atn - ideal_position;
//            if(filtered_value > 0.5) filtered_value = 0;
            if(filtered_value > M_PI) filtered_value -= M_PI * 2;
            if(filtered_value < -M_PI) filtered_value += M_PI * 2;
            // cout << "apr=" << apr << "\t ip=" << ideal_position << "\t fv=" << filtered_value << endl;
            data_arm_axis_table_nominals.push_back(ofVec2f(i, filtered_value));
        } else {
            data_arm_axis_table_nominals.push_back(ofVec2f(i, ptr_arm_axis_table_nominals[i]));
        }
    }
    
    // making a copy of Axis Tables Nominals for the PLATTER that we can pass to the visualisation
    int size_platter_axis_table_nominals = table_nominals_points_to_compare;

    //    int size_platter_axis_table_nominals = sizeof(dump.memory.axis_table[1].nominals)/sizeof(dump.memory.axis_table[1].nominals[0]);
    float * ptr_platter_axis_table_nominals = dump.memory.axis_table[1].nominals;
    for(int i=0; i < size_platter_axis_table_nominals; i++) {
        if(table_nominals_compare_to_ideal_positions) {
            static int index_offset = 0;
            // below is for checking whether the values actually differentiate in respect to ideal positions
            float ptn = ptr_platter_axis_table_nominals[i];
            float ideal_position = (M_PI * 2 * (float)i / size_platter_axis_table_nominals);
            if(i >= size_platter_axis_table_nominals) ideal_position = 0;
            float filtered_value = ptn - ideal_position;
//            if(filtered_value > 0.5) filtered_value = 0;
            if(filtered_value > M_PI) filtered_value -= M_PI * 2;
            if(filtered_value < -M_PI) filtered_value += M_PI * 2;
            // cout << "apr=" << apr << "\t ip=" << ideal_position << "\t fv=" << filtered_value << endl;
            data_platter_axis_table_nominals.push_back(ofVec2f(i, filtered_value));
        } else {
            data_platter_axis_table_nominals.push_back(ofVec2f(i, ptr_platter_axis_table_nominals[i]));
        }
    }
    
    
    // making a copy of Phase References for the ARM that we can pass to the visualisation
    int size_arm_phase_reference = phase_reference_points_to_compare;
//    int size_arm_phase_reference = sizeof(dump.memory.phase_reference[0])/sizeof(dump.memory.phase_reference[0][0]);
    float * ptr_arm_phase_reference = dump.memory.phase_reference[0];
    for(int i=0; i < size_arm_phase_reference; i++) {
        if(phase_reference_compare_to_ideal_positions) {
            static int index_offset = 0;
            // below is for checking whether the values actually differentiate in respect to ideal positions
            float apr = ptr_arm_phase_reference[i];
            float ideal_position = (M_PI * 2 * (float)i / phase_reference_points_to_compare);
            if(i >= phase_reference_points_to_compare) ideal_position = 0;
            float filtered_value = apr - ideal_position;
            if(filtered_value > M_PI) filtered_value -= M_PI * 2;
            if(filtered_value < -M_PI) filtered_value += M_PI * 2;
//            if(i == 0 && filtered_value > 1 ) filtered_value = 0;
            // cout << "apr=" << apr << "\t ip=" << ideal_position << "\t fv=" << filtered_value << endl;
            data_arm_phase_reference.push_back(ofVec2f(i, filtered_value));
        } else {
            data_arm_phase_reference.push_back(ofVec2f(i, ptr_arm_phase_reference[i]));
        }
    }
    
    // making a copy of Phase References for the PLATTER that we can pass to the visualisation
    int size_platter_phase_reference = phase_reference_points_to_compare;
//    int size_platter_phase_reference = sizeof(dump.memory.phase_reference[1])/sizeof(dump.memory.phase_reference[1][0]);
    float * ptr_platter_phase_reference = dump.memory.phase_reference[1];
    for(int i=0; i< size_platter_phase_reference; i++) {
        if(phase_reference_compare_to_ideal_positions) {
            // below is for checking whether the values actually differentiate in respect to ideal positions
            float apr = ptr_platter_phase_reference[i];
//            if(i == 0) apr = 0;
            float ideal_position = (M_PI * 2 * (float)i / phase_reference_points_to_compare);
            if(i >= phase_reference_points_to_compare) ideal_position = 0;
            float filtered_value = apr - ideal_position;
            if(filtered_value > M_PI) filtered_value -= M_PI * 2;
            if(filtered_value < -M_PI) filtered_value += M_PI * 2;
            //                cout << "apr=" << apr << "\t ip=" << ideal_position << "\t fv=" << filtered_value << endl;
            data_platter_phase_reference.push_back(ofVec2f(i, filtered_value));
        } else {
            data_platter_phase_reference.push_back(ofVec2f(i, ptr_platter_phase_reference[i]));
        }
    }
    
    // making a copy of ECM Samples for the ARM that we can pass to the visualisation
    gdbMemoryDump::ecm_sample_t * ptr_arm_ecm = dump.memory.models[0].samples;
    int size_arm_ecm = sizeof(dump.memory.models[0].samples)/sizeof(dump.memory.models[0].samples[0]);
    int nan_counter = 0;
    for(int i=0; i< size_arm_ecm; i++) {
//                dump.printDataBytesAsFloats(&ptr_arm_ecm[i], 8);
        float offset = ptr_arm_ecm[i].offset;
        float acceleration = ptr_arm_ecm[i].acceleration;
        if(isnan(offset) || isnan(acceleration)) { // check if any are NaN, meaning they were not set in the flash sector.
//            data_arm_ecm.push_back(ofVec2f(i, i));
            nan_counter++;
        }
        else data_arm_ecm.push_back(ofVec2f(offset, acceleration)); // <--- THIS IS THE LINE WE SHOULD USE INSTEAD OF BELOW
//        else data_arm_ecm.push_back(ofVec2f(i - size_arm_ecm/2 - 10, i - size_arm_ecm/2 + 10));  // JUST FOR TESTING. DELETE WHEN WE GOT WORKING DATA
    }
    cout << "ARM ECM has " << dec << size_arm_ecm << " samples. " << dec << nan_counter << " of them are NaN" << endl;
    gdbMemoryDump::ecm_model_t * ptr_arm_ecm_model = &dump.memory.models[0];
//    data_trend_arm_ecm[0] = ptr_arm_ecm_model->alpha;
//    data_trend_arm_ecm[1] = ptr_arm_ecm_model->beta;
//    data_trend_arm_ecm[2] = ptr_arm_ecm_model->bias;
    cout << endl;

    
    
    // making a copy of ECM Samples for the PLATTER that we can pass to the visualisation
    gdbMemoryDump::ecm_sample_t * ptr_platter_ecm = dump.memory.models[1].samples;
    int size_platter_ecm = sizeof(dump.memory.models[1].samples)/sizeof(dump.memory.models[1].samples[0]);
    nan_counter = 0;
    for(int i=0; i< size_platter_ecm; i++) {
//        dump.printDataBytesAsFloats(&ptr_platter_ecm[i], 8);
        float offset = ptr_platter_ecm[i].offset;
        float acceleration = ptr_platter_ecm[i].acceleration;
        float velocity = ptr_platter_ecm[i].velocity;
        if(isnan(offset) || isnan(acceleration) || isnan(velocity)) { // check if any are NaN, meaning they were not set in the flash sector.
//            data_platter_ecm.push_back(ofVec2f(i, i));
            nan_counter++;
        }
        else data_platter_ecm.push_back(ofVec3f(offset, acceleration, velocity)); // <--- THIS IS THE LINE WE SHOULD USE INSTEAD OF BELOW
//        else data_platter_ecm.push_back(ofVec2f(i - size_platter_ecm/2 - 40, i - size_platter_ecm/2 - 10));  // JUST FOR TESTING. DELETE WHEN WE GOT WORKING DATA
    }
    cout << "PLATTER ECM has " << dec << size_platter_ecm << " samples. " << dec << nan_counter << " of them are NaN" << endl;
    gdbMemoryDump::ecm_model_t * ptr_platter_ecm_model = &dump.memory.models[1];
//    data_trend_platter_ecm[0] = ptr_platter_ecm_model->alpha;
//    data_trend_platter_ecm[1] = ptr_platter_ecm_model->beta;
//    data_trend_platter_ecm[2] = ptr_platter_ecm_model->bias;

    cout << endl;

    
    // making a copy of GRAPHER Samples for the PLATTER that we can pass to the visualisation
    bool grapher_values = false;
    int axis = 0;
    for(int iteration=0; iteration < ITERATIONS; iteration++) {
        data_psamples3D_ecm.push_back(vector<ofVec3f>{});
        data_pacc_vel_ecm.push_back(vector<ofVec3f>{});
        gdbMemoryDump::grapher_t * ptr_pgrapher = &dump.memory.grapher[axis];
        gdbMemoryDump::sample_t * ptr_psamples = ptr_pgrapher->samples[iteration];
        int size_psamples = sizeof(dump.memory.grapher[axis].samples[iteration])/sizeof(dump.memory.grapher[axis].samples[iteration][0]);
        nan_counter = 0;
        float velocity_smoothing_factor = 0.0;
        for(int cnt=0; cnt< size_psamples; cnt++) {
    //        dump.printDataBytesAsFloats(&ptr_psamples[i], 8);
            static float t0, v0, a;
            float x = ptr_psamples[cnt].x;
            float y = ptr_psamples[cnt].y;
            float z = ptr_psamples[cnt].z;
            float t = x;
            static float v;
            if(t <= t0) {
                v = y;
            } else {
                v = v * velocity_smoothing_factor + y * (1 - velocity_smoothing_factor);
            }
            if(isnan(x) || isnan(y) || isnan(z)) { // check if any are NaN, meaning they were not set in the flash sector.
                //            data_platter_ecm.push_back(ofVec2f(i, i));
                nan_counter++;
            } else {
                data_psamples3D_ecm[iteration].push_back(ofVec3f(t, v, z)); // <--- THIS IS THE LINE WE SHOULD USE INSTEAD OF BELOW
            //        else data_platter_ecm.push_back(ofVec2f(i - size_platter_ecm/2 - 40, i - size_platter_ecm/2 - 10));  // JUST FOR TESTING. DELETE WHEN WE GOT WORKING DATA
                if(cnt > 0) {
                    a = (v - v0) / (t - t0);
                }
                t0 = t;
                v0 = v;
                data_pacc_vel_ecm[iteration].push_back(ofVec3f(v, a, z));
                if(isnan(a) == false) {
                    dump_for_simen.push_back(ofVec4f(z,t,v,a));
                }
            }
        }
        data_trend_platter_ecm.push_back(ofVec3f());
        data_trend_platter_ecm[iteration].x = ptr_pgrapher->alpha[iteration];
        data_trend_platter_ecm[iteration].y = ptr_pgrapher->beta[iteration];
        data_trend_platter_ecm[iteration].z = ptr_pgrapher->gamma[iteration];
        cout << "PSAMPLES[" << iteration << "] has " << dec << size_psamples << " samples. " << dec << nan_counter << " of them are NaN" << endl;
        cout << endl;
        if(grapher_values) {
            cout << "X - COORDINATES" << endl;
            for(int i=0; i < size_psamples; i++) {
                cout << fixed << setprecision(4) << ptr_psamples[i].x << endl;
            }
            cout << "Y - COORDINATES" << endl;
            for(int i=0; i < size_psamples; i++) {
                cout << fixed << setprecision(4) << ptr_psamples[i].y << endl;
            }
            cout << endl;
            cout << "Z - COORDINATES" << endl;
            for(int i=0; i < size_psamples; i++) {
                cout << fixed << setprecision(4) << ptr_psamples[i].z << endl;
            }
            cout << endl;
        }
    }
    if(true) {
        for(int i=0; i < dump_for_simen.size(); i++) {
            cout << "[" << fixed << setprecision(8) << dump_for_simen[i].x << ","
                        << fixed << setprecision(8) << dump_for_simen[i].y << ","
                        << fixed << setprecision(8) << dump_for_simen[i].z << ","
                        << fixed << setprecision(8) << dump_for_simen[i].w << "],"
                        << endl;
        }
    }
//    cout << defaultfloat << endl;
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


void ofApp::updatePrinterStatus() {
    char temp_string[16];
    sprintf(temp_string, "%.1f / %.1f", printer.hotend, printer.setpoint);
    temperature_string = temp_string;
    view_temperature.changeText(temperature_string);
    view_node_buffer.changeText(to_string(printer.node_buffer));
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

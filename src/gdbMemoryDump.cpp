//
//  gdbMemoryDump.cpp
//  altaVisualizer
//
//  Created by Jakob Bak on 16/03/16.
//
//

#include "stdint.h"
#include "gdbMemoryDump.h"

/////////////////////
// GDB_MEMORY_DUMP //
/////////////////////

gdbMemoryDump::gdbMemoryDump() {
    // put futher initialisation code here if needed
}



char * gdbMemoryDump::setSignature(char * byte_ptr) {
    memory.permastore_signature = * ((uint32_t *)byte_ptr);
    return byte_ptr + sizeof(memory.permastore_signature);
}


char * gdbMemoryDump::setPhasesPerRevolution(char * byte_ptr) {
    for(int i=0; i < NUMBER_OF_ECM_DRIVEN_MOTORS; i++) {
        memory.config.phases_per_revolution[i] = * (((uint16_t *)byte_ptr) + i);
    }
    return byte_ptr + sizeof(memory.config.phases_per_revolution);
}


char * gdbMemoryDump::setArmHomePosition(char * byte_ptr) {
    memory.config.arm_home_position = * ((float *) byte_ptr);
    return byte_ptr + sizeof(memory.config.arm_home_position);
    
}


char * gdbMemoryDump::setArmClockwiseExtent(char * byte_ptr) {
    memory.config.arm_clockwise_extent = * ((float *) byte_ptr);
    return byte_ptr + sizeof(memory.config.arm_clockwise_extent);
}


char * gdbMemoryDump::setArmAntiClockwiseExtent(char * byte_ptr) {
    memory.config.arm_anti_clockwise_extent = * ((float *) byte_ptr);
    return byte_ptr + sizeof(memory.config.arm_anti_clockwise_extent);
}


char * gdbMemoryDump::setConfigWithSignature(char * byte_ptr) {
    char * data_ptr = byte_ptr;
    data_ptr = setSignature(data_ptr);
    data_ptr = setPhasesPerRevolution(data_ptr);
    data_ptr = setArmHomePosition(data_ptr);
    data_ptr = setArmClockwiseExtent(data_ptr);
    data_ptr = setArmAntiClockwiseExtent(data_ptr);
    printConfigWithSignature();
    return data_ptr;
}


void gdbMemoryDump::printConfigWithSignature() {
    cout << "Signature is: ";
    cout << hex << memory.permastore_signature << endl;
    cout << "Phases per revolution are: ARM=" << dec << memory.config.phases_per_revolution[0] << " "
                                << "PLATTER=" << dec << memory.config.phases_per_revolution[1] << endl;
    cout << "ARM home position is " << memory.config.arm_home_position << endl;
    cout << "ARM clockwise extent is " << memory.config.arm_clockwise_extent << endl;
    cout << "ARM anti clockwise extent is " << memory.config.arm_anti_clockwise_extent << endl;
    cout << endl;

}


void gdbMemoryDump::printSignature() {
    cout << "Signature is: ";
    cout << hex << memory.permastore_signature << endl;
}


char * gdbMemoryDump::loadPhaserPermastoreData(char * byte_ptr) {
    char * data_ptr = byte_ptr;
    memcpy(&memory.axis_table, data_ptr, sizeof(memory.axis_table));
    data_ptr = data_ptr + sizeof(memory.axis_table);
    cout << "Loaded AXIS_TABLE from permastore dump. Size is " << dec
         << sizeof(memory.axis_table) << " bytes" << endl;

    memcpy(&memory.phase_reference, data_ptr, sizeof(memory.phase_reference));
    data_ptr = data_ptr + sizeof(memory.phase_reference);
    cout << "Loaded PHASE_REFERENCE from permastore dump. Size is " << dec
         << sizeof(memory.phase_reference) << " bytes" << endl;
    cout << endl;
    
    memcpy(&memory.phase_reference_bias, data_ptr, sizeof(memory.phase_reference_bias));
    data_ptr = data_ptr + sizeof(memory.phase_reference_bias);
    cout << "Loaded PHASE_REFERENCE_BIAS from permastore dump. Size is " << dec
    << sizeof(memory.phase_reference_bias) << " bytes" << endl;
    cout << endl;

    
    return data_ptr;
}


char * gdbMemoryDump::loadEcmPermastoreData(char * byte_ptr) {
    char * data_ptr = byte_ptr;
    memcpy(&memory.models, data_ptr, sizeof(memory.models));
    data_ptr = data_ptr + sizeof(memory.models);
    cout << "Loaded MODELS from permastore dump. Size is " << dec
         << sizeof(memory.models) << " bytes" << endl;
    cout << endl;

    memcpy(&memory.grapher, data_ptr, sizeof(memory.grapher));
    data_ptr = data_ptr + sizeof(memory.grapher);
    cout << "Loaded GRAPHER from permastore dump. Size is " << dec
    << sizeof(memory.grapher) << " bytes" << endl;
    cout << endl;

    return data_ptr;
}


void gdbMemoryDump::readNewDataFromFileDump(string file_name) {
    
    ofFile dataFile;
    dataFile.open(ofToDataPath(file_name), ofFile::ReadOnly, true);
    ofBuffer buffer = dataFile.readToBuffer();
    long data_size = buffer.size();
    char * data_ptr = buffer.getData();
    
    // make sure these get called in the order corresponding to the order in the firmware!
    data_ptr = setConfigWithSignature(data_ptr);
    data_ptr = loadPhaserPermastoreData(data_ptr);
    data_ptr = loadEcmPermastoreData(data_ptr);

}


void gdbMemoryDump::printDataBytesAsFloats(void * mem_ptr, int num_bytes) {

//    int channel = 0;
    int float_size = sizeof(float);
    for(int i = 0; i < num_bytes / float_size; i++) {
        float value = getFloatFromBytes(mem_ptr, float_size * i);
        char * mem_addr = (char *)&value;
        
        cout << setiosflags( ios::scientific );
        cout << setprecision( 5 );
        cout << "value[" << dec << (int)i << "] is: " << float(value) << "    ";
        
        cout << showbase // show the 0x prefix
        << internal // fill between the prefix and the number
        << setfill('0'); // fill with 0s
        
        for(int j = 0; j < 4; j++) {
            cout << hex << setw(4) << (int)(unsigned char)mem_addr[j] << " ";
        }
        cout << endl;
    }
    
    
    if(false) {
        cout << "data_size is: " << (unsigned)num_bytes << endl;
        
        cout << showbase // show the 0x prefix
        << internal // fill between the prefix and the number
        << setfill('0'); // fill with 0s
        
        for(int i=0; i < num_bytes; i++) {
            if(i % 4 == 0) cout << endl;
            // we have to cast the data_ptr to uint8_t first and then int, otherwise it will see numbers >= 0x80 as negative
            cout << hex << setw(4) << (int)(uint8_t)((char *)mem_ptr)[i] << endl;
        }
    }

}



float gdbMemoryDump::getFloatFromBytes(void * byte_ptr, uint32_t pos) {
    float * float_value = (float *)&((char *)byte_ptr)[pos];
    return *float_value;
}


// We are not using this one anyway, but leaving it for now in case we need it.
float gdbMemoryDump::convertLittleEndianFloatAsBytesToBigEndianFloat(char * byte_ptr, uint32_t pos) {
    float float_value;
    uint64_t mem_addr = (uint64_t)&float_value;
    for(int i=0; i<4; i++) {
        memcpy((void *)(mem_addr + i), &byte_ptr[pos + (3 - i)], sizeof(char));
    }
    return float_value;
}


//
//  Views.cpp
//  altaVisualizer
//
//  Created by Jakob Bak on 19/03/16.
//
//

#include "Views.h"

// template for signum function used further down. Copy-pasted from Stackoverflow.com
// http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

View::View(ofVec2f upperLeftCorner, ofVec2f size, string title)
{
    view_config.update_view = true;
    view_config.position = upperLeftCorner;
    view_config.size = size;
    view_config.title = title;
    view_config.title_position = ofVec2f(10,10);
    view_config.title_color = ofColor(255, 255, 255);
    view_config.font_name = "OpenSans-Regular.ttf";
    view_config.font_size = 12;
    view_config.using_ttf = view_config.font.load(view_config.font_name, view_config.font_size);
    view_config.background_color = ofColor(64, 64, 64);
}


void View::draw()
{
    float xulc = view_config.position.x;
    float yulc = view_config.position.y;
    float width = view_config.size.x;
    float height = view_config.size.y;
    string title = view_config.title;
    float title_x = view_config.title_position.x;
    float title_y = view_config.title_position.y;
    ofTrueTypeFont font = view_config.font;
    bool using_ttf = view_config.using_ttf;
    ofColor background_color = view_config.background_color;
    ofColor title_color = view_config.title_color;
    
    ofFill();
    ofSetColor(background_color);
    ofDrawRectangle(xulc, yulc, width, height);
    
    ofSetColor(title_color);
    if(using_ttf) {
        float ttf_height_offset = font.stringHeight(title);
        font.drawString(title, xulc + title_x, yulc + ttf_height_offset + title_y);
    }
    else ofDrawBitmapString(title, xulc + title_x, yulc + 10 + title_y);
}




ViewTextBox::ViewTextBox(ofVec2f upperLeftCorner, ofVec2f size, string text, string title)
    : View(upperLeftCorner, size, title)
{
    textBox_config.text = text;
    textBox_config.text_position = ofVec2f(10,10);
    textBox_config.text_color = view_config.title_color;
    textBox_config.text_font_name = view_config.font_name;
    textBox_config.text_font_size = view_config.font_size;
    textBox_config.text_using_ttf = textBox_config.text_font.load(textBox_config.text_font_name, textBox_config.text_font_size);
    
}


void ViewTextBox::draw() {
    View::draw();
    float xulc = view_config.position.x;
    float yulc = view_config.position.y;
    float width = view_config.size.x;
    float height = view_config.size.y;
    float text_x = textBox_config.text_position.x;
    float text_y = textBox_config.text_position.y;
    ofTrueTypeFont title_font = view_config.font;
    string title = view_config.title;
    
    string text = textBox_config.text;
    ofColor text_color = textBox_config.text_color;
    bool using_ttf = textBox_config.text_using_ttf;
    ofTrueTypeFont font = textBox_config.text_font;

    ofSetColor(text_color);
    if(using_ttf) {
        float ttf_height_offset = font.stringHeight("abcdefghijklmnopqrstuvwyxz0123456789"); // all letters and digits to make sure the proper ascenders and descenders are taken into account
        float ttf_title_height_offset = title_font.stringHeight(title);
        float title_margin = (ttf_title_height_offset > 0 ? 5 : 0);
        font.drawString(text, xulc + text_x, yulc + ttf_height_offset + ttf_title_height_offset + title_margin + text_y);
    }
    else ofDrawBitmapString(text, xulc + text_x, yulc + 10 + text_y);

}

ViewGraph2D::ViewGraph2D(ofVec2f upperLeftCorner, ofVec2f size, std::vector<ofVec2f> data_set, plot2D_type type, string title)
    : View(upperLeftCorner, size, title)
{
    graph2D_config.type = type;
    
    graph2D_config.graph_color = ofColor(255, 255, 255);
    graph2D_config.axis_color = ofColor(255, 255, 255);
    graph2D_config.circle_radius = 1;
    graph2D_config.line_width = 1;
    graph2D_config.text_font_name = view_config.font_name;
    graph2D_config.text_font_size = view_config.font_size;
    graph2D_config.text_using_ttf = graph2D_config.text_font.load(graph2D_config.text_font_name, graph2D_config.text_font_size);
    
    float xmin = data_set[0].x;
    float xmax = data_set[0].x;
    float ymin = data_set[0].y;
    float ymax = data_set[0].y;
    
    for(int i=0; i < data_set.size(); i++) {
        ofVec2f data_point = data_set[i];
        if(data_point.x < xmin) xmin = data_point.x;
        if(data_point.x > xmax) xmax = data_point.x;
        if(data_point.y < ymin) ymin = data_point.y;
        if(data_point.y > ymax) ymax = data_point.y;
        data.push_back(data_point);
        
//        cout << "x=" << dataPoint.x << " \t";
//        cout << "y=" << dataPoint.y << endl;
    }
    float xrange = xmax - xmin;
    float yrange = ymax - ymin;
    
    cout << "Graph " << title << ":" << endl;
    cout << "xmin=" << xmin << endl;
    cout << "xmax=" << xmax << endl;
    cout << "ymin=" << ymin << endl;
    cout << "ymax=" << ymax << endl;
    cout << "xrange=" << xrange << endl;
    cout << "yrange=" << yrange << endl;
    
    if(xrange == 0) {
//        xrange = 1;
        cout << "For " << title << " xrange was 0, setting it to 1 to avoid division by zero" << endl;
    }
    if(yrange == 0) {
//        yrange = 1;
        cout << "For " << title << " yrange was 0, setting it to 1 to avoid division by zero" << endl;
    }
    
    graph2D_config.xmin = xmin;
    graph2D_config.xmax = xmax;
    graph2D_config.ymin = ymin;
    graph2D_config.ymax = ymax;

    graph2D_config.xrange = xrange;
    graph2D_config.yrange = yrange;
    
    cout << "Unit Values: " << endl;
    getUnitLines(&graph2D_config.x_unit_values, &graph2D_config.xprecision, xmin, xmax);
    getUnitLines(&graph2D_config.y_unit_values, &graph2D_config.yprecision, ymin, ymax);
    cout << endl;
}


void ViewGraph2D::draw()
{
    View::draw();
    float xulc = view_config.position.x;
    float yulc = view_config.position.y;
    float width = view_config.size.x;
    float height = view_config.size.y;

    plot2D_type type = graph2D_config.type;
    float xmin = graph2D_config.xmin;
    float xmax = graph2D_config.xmax;
    float ymin = graph2D_config.ymin;
    float ymax = graph2D_config.ymax;
    float xrange = graph2D_config.xrange;
    float yrange = graph2D_config.yrange;
    ofColor graph_color = graph2D_config.graph_color;
    ofColor axis_color = graph2D_config.axis_color;
    float line_width = graph2D_config.line_width;
    float circle_radius = graph2D_config.circle_radius;
    std::vector<float> x_axis_units = graph2D_config.x_unit_values; // <------
    std::vector<float> y_axis_units = graph2D_config.y_unit_values; // <------
    int xprecision = graph2D_config.xprecision;
    int yprecision = graph2D_config.yprecision;
    bool using_truetypefont = graph2D_config.text_using_ttf; // <-------------
    ofTrueTypeFont font = graph2D_config.text_font;
    
    // Prepare variables for unit lines and axes
    float xpos_multiplier = width / xrange;
    float ypos_multiplier = height / yrange;
    float xpos_offset = -xmin * xpos_multiplier;
    float ypos_offset = ymin * ypos_multiplier;
    
    float x_axis_x0 = xulc;
    float x_axis_x1 = xulc + width;
    float x_axis_y = yulc + ypos_offset + height;
    float y_axis_y0 = yulc + height;
    float y_axis_y1 = yulc;
    float y_axis_x = xulc + xpos_offset;
    
    // Draw unit lines
    ofSetColor(axis_color * 0.4);
    for(int i=0; i < x_axis_units.size(); i++) {
        float x_unit_x = xulc + xpos_offset + (x_axis_units[i] * xpos_multiplier);
        ofDrawLine(x_unit_x, y_axis_y0, x_unit_x, y_axis_y1);
    }
    for(int i=0; i < y_axis_units.size(); i++) {
        float y_unit_y = yulc + ypos_offset + (height - (y_axis_units[i] * ypos_multiplier));
        ofDrawLine(x_axis_x0, y_unit_y, x_axis_x1, y_unit_y);
    }
    
    // Draw unit digits
    if(using_truetypefont) {
        for(int i=0; i < y_axis_units.size(); i++) {
            if(yprecision < 0) yprecision = 0;
            stringstream unit_float;
            unit_float << fixed << setprecision(yprecision) << y_axis_units[i];
            string unit_value = unit_float.str();
//            string unit_value = to_string(float(y_axis_units[i]));
            float text_height = font.stringHeight(unit_value);
            float text_width = font.stringWidth(unit_value);
            float unit_xpos = xulc + xpos_offset + 5;
            float unit_ypos = yulc - 5 + ypos_offset + (height - (y_axis_units[i] * ypos_multiplier));
            if(unit_xpos > xulc + width - text_width - 5) {
                unit_xpos = xulc + width - text_width -5;
                if(unit_value == "0") unit_value = "";
            }
            if(unit_xpos < xulc + 5) {
                unit_xpos = xulc + 5;
                if(unit_value == "0") unit_value = "";
            }
            if(unit_ypos < yulc + text_height + 5) continue;
            if(unit_ypos > yulc + height - 5) continue;
            font.drawString(unit_value, unit_xpos, unit_ypos);
        }
        for(int i=0; i < x_axis_units.size(); i++) {
            string unit_value = to_string(int(x_axis_units[i]));
            float text_height = font.stringHeight(unit_value);
            float text_width = font.stringWidth(unit_value);
            float unit_xpos = xulc + xpos_offset + 5 + (x_axis_units[i] * xpos_multiplier);
            float unit_ypos = yulc + ypos_offset + height - 5;
            if(unit_ypos < yulc + text_height + 5) {
                unit_ypos = yulc + text_height + 5;
                if(unit_value == "0") unit_value = "";
            }
            if(unit_ypos > yulc + height - 5) {
                unit_ypos = yulc + height - 5;
                if(unit_value == "0") unit_value = "";
            }
            if(unit_xpos > xulc + width - text_width - 5) continue;
            if(unit_xpos < xulc + 5) continue;
            font.drawString(unit_value, unit_xpos, unit_ypos);
        }
    } else {
        cout << "No TrueTypeFont loaded - couldn't write unit text on axes." << endl;
    }

    // Draw axes
    ofSetColor(axis_color);
    if(x_axis_y >= yulc && x_axis_y <= yulc + height) {
        ofDrawLine(x_axis_x0, x_axis_y, x_axis_x1, x_axis_y);
    }
    if(y_axis_x >= xulc && y_axis_x <= xulc + width) {
        ofDrawLine(y_axis_x, y_axis_y0, y_axis_x, y_axis_y1);
    }
    
    ofSetColor(graph_color);
    switch(type) {
        case PLOT2D_SCATTER:
            for(int i=0; i<data.size(); i++) {
                float x1 = xulc + xpos_offset + (data[i].x * xpos_multiplier);
                float y1 = yulc + ypos_offset + (height - (data[i].y * ypos_multiplier));
                ofDrawCircle(x1, y1, circle_radius);
            }
            break;
        case PLOT2D_LINE:
            ofSetLineWidth(line_width);
            for(int i=1; i<data.size(); i++) {
                float x0 = xulc + xpos_offset + (data[i-1].x * xpos_multiplier);
                float y0 = yulc + ypos_offset + (height - (data[i-1].y * ypos_multiplier));
                float x1 = xulc + xpos_offset + (data[i].x * xpos_multiplier);
                float y1 = yulc + ypos_offset + (height - (data[i].y * ypos_multiplier));
                ofDrawLine(x0, y0, x1, y1);
            }
            break;
        default:
            break;
    }
    
}

void ViewGraph2D::getUnitLines(std::vector<float> * unit_values, int * precision, float min, float max) {
    float range = max - min;
    float decade = pow(10, floor(log10(range)));
//    cout << "decade=" << decade << endl;
    float unit = decade / 10;
//    cout << "unit=" << unit << endl;
    float unit_lines = floor(range/unit);
//    cout << "unit_lines=" << unit_lines << endl;
    
    float unit_multiplier = 1;
    if(unit_lines > 20) unit_multiplier *= 5;
    float unit_interval = unit * unit_multiplier;
//    cout << "unit_interval=" << unit_interval << endl;
    
//    cout << "float_precision=" << ceil(log10(1/unit_interval)) << endl;
    * precision = ceil(log10(1/unit_interval)); // <--- check if this works for a range of different values
    
    int unit_line_start = 0;
    
//    cout << "got this far" << endl;
    
    for(float i=0; i<max;i+=unit_interval) {
        if(i >= min && i<=max) unit_values->push_back(i);
    }
    for(float i=0; i>min;i-=unit_interval) {
        if(i >= min && i<=max) unit_values->push_back(i);
    }
    for(int i=0; i<unit_values->size(); i++) {
        if(*precision > 0) cout << fixed << setprecision(*precision);
        else cout << fixed << setprecision(0);
        cout << unit_values->at(i) << "\t";
    }
    cout << endl;
}


ViewRealtimePlotter2D::ViewRealtimePlotter2D(ofVec2f upperLeftCorner, ofVec2f size, string title)
    : View(upperLeftCorner, size, title)
{
    pallette = plotColors();
    realtime_plotter_config.graph_time_span = 2000;

    realtime_plotter_config.axis_color = ofColor(255, 255, 255);
    realtime_plotter_config.line_width = 1;
    realtime_plotter_config.text_font_name = view_config.font_name;
    realtime_plotter_config.text_font_size = view_config.font_size;
    realtime_plotter_config.text_using_ttf = realtime_plotter_config.text_font.load(realtime_plotter_config.text_font_name, realtime_plotter_config.text_font_size);

    realtime_plotter_config.centered = false;
    
}


void ViewRealtimePlotter2D::draw()
{
    if(view_config.update_view) updateGraph();
    View::draw();
    float xulc = view_config.position.x;
    float yulc = view_config.position.y;
    float width = view_config.size.x;
    float height = view_config.size.y;
    
    float time_span = realtime_plotter_config.graph_time_span;
    
    for(int value_type=0; value_type < graph.size(); value_type++) {
        if(!graph[value_type].empty()) {
            float range = realtime_plotter_config.range[value_type];
            float min = realtime_plotter_config.min[value_type];
            float max = realtime_plotter_config.max[value_type];
            
//            cout << "Value Type: " << value_type << "\t min=" << min << "\t max=" << max << "\t range=" << realtime_plotter_config.range[value_type] << endl;

            ofSetColor(realtime_plotter_config.graph_color[value_type]);
            ofSetLineWidth(realtime_plotter_config.line_width);
            for(int value_index = 1; value_index < graph[value_type].size(); value_index++) {
                float x0 = -(graph[value_type][value_index-1].x / time_span * width) + xulc + width;
                float y0 = -((graph[value_type][value_index-1].y - min) / range * height) + yulc + height;
                float x1 = -(graph[value_type][value_index].x / time_span * width) + xulc + width;
                float y1 = -((graph[value_type][value_index].y - min) / range * height) + yulc + height;
//                cout << x0 << "\t" << y0 << "\t" << x1 << "\t" << y1 << endl;
                ofDrawLine(x0, y0, x1, y1);
            }
        }
    }
    float label_pos_x = xulc;
    float label_pos_y = yulc + height;
    for(int value_type=0; value_type < graph.size(); value_type++) {
        bool visible = realtime_plotter_config.visible[value_type];
        ofTrueTypeFont font = realtime_plotter_config.text_font;
        string label = realtime_plotter_config.value_type[value_type];
        ofColor color = realtime_plotter_config.graph_color[value_type];
        ofColor background_color = view_config.background_color * 0.5;
        float label_width = font.stringWidth(label);
        float label_height = font.stringHeight(label);
        float label_margin = 10;
        float xbox = label_pos_x + label_margin;
        float ybox = label_pos_y - label_margin;
        float wbox = label_width + label_margin*2;
        float hbox = label_height + label_margin*2;
        ofSetColor(background_color);
        ofDrawRectangle(xbox, ybox, wbox, -hbox);
        float xlabel = xbox + label_margin;
        float ylabel = ybox - label_margin;
        if(visible) ofSetColor(color);
        else ofSetColor(background_color * 2);
        font.drawString(label, xlabel, ylabel);
        label_pos_x += wbox + label_margin;
    }
}


void ViewRealtimePlotter2D::updateGraph() {
    // Get the current time in millis and calculate the threshold under which we shouldn't
    // pass any data to the graph.
    float time_now = float(ofGetElapsedTimeMillis());
    float time_threshold = time_now - realtime_plotter_config.graph_time_span;
    
    // Iterate over all of the data labels stored
    for(int value_type=0; value_type < data.size(); value_type++) {

        graph[value_type].clear();
        if(!realtime_plotter_config.visible[value_type]) continue;
        // Get the last value and the time it was reported. If it is too old continue to next value_type.
        int last_value_index = data[value_type].size() - 1;
        float last_value_time = data[value_type][last_value_index].first;
        if(last_value_time < time_threshold) continue;

        // Also initiate min and max values for the time_span we want to draw.
        float last_value = data[value_type][last_value_index].second;
        float min = last_value;
        float max = last_value;


        // Iterate backwards over the datapoints for the value_type, break if we get below the time_threshold.
        for(int value_index=last_value_index; value_index >= 0; value_index--) {
            
            float value_time = data[value_type][value_index].first;
            if(value_time < time_threshold) break;
            
            // Update the min and max values
            float value = data[value_type][value_index].second;
            if(value < min) min = value;
            if(value > max) max = value;
            
            // Add the value and its relative position in time (with respect to time_now)
            // to the graph vector.
            float graph_time = time_now - value_time;
            graph[value_type].push_back(ofVec2f(graph_time, value));
        }
        if(realtime_plotter_config.centered) {
            if(fabsf(min) > fabsf(max)) max = sgn(max) * fabsf(min);
            else if(fabsf(max) > fabsf(min) && min < 0) min = sgn(min) * fabsf(max);
            else if(fabsf(max) > fabsf(min) && min == 0) min = 0;
            // maybe a condition when min > 0 ?
        }
        realtime_plotter_config.min[value_type] = min;
        realtime_plotter_config.max[value_type] = max;
        realtime_plotter_config.range[value_type] = max - min;
    }
}


void ViewRealtimePlotter2D::addDataPoint(string value_type, float value) {
    int value_type_index = getValueTypeVectorIndex(value_type);
    
    float time_now = float(ofGetElapsedTimeMillis());
    pair<float, float> new_value_pair = {time_now, value};
    data[value_type_index].push_back(new_value_pair);
    
}


int ViewRealtimePlotter2D::getValueTypeVectorIndex(string value_type) {
    vector<string> * value_types_ptr = &realtime_plotter_config.value_type;
    
    for(int i=0; i < value_types_ptr->size(); i++) {
        if(value_types_ptr->at(i) == value_type) return i;
    }

    value_types_ptr->push_back(value_type);

    data.push_back(vector<pair<float,float>>());
    graph.push_back(vector<ofVec2f>());
    realtime_plotter_config.min.push_back(0);
    realtime_plotter_config.max.push_back(0);
    realtime_plotter_config.range.push_back(0);
    realtime_plotter_config.graph_color.push_back(pallette.getNewColor());
    realtime_plotter_config.visible.push_back(true);
    
    return value_types_ptr->size() - 1;
}


void ViewRealtimePlotter2D::hideGraph(int value_type) {
    if(value_type < realtime_plotter_config.visible.size()) {
        realtime_plotter_config.visible[value_type] = false;
    }
}


void ViewRealtimePlotter2D::showGraph(int value_type) {
    if(value_type < realtime_plotter_config.visible.size()) {
        realtime_plotter_config.visible[value_type] = true;
    }
}

void ViewRealtimePlotter2D::toggleGraphVisibility(int value_type) {
    if(value_type < realtime_plotter_config.visible.size()) {
        if(realtime_plotter_config.visible[value_type]) realtime_plotter_config.visible[value_type] = false;
        else realtime_plotter_config.visible[value_type] = true;
    }
}






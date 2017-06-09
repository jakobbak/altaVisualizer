//
//  Views.h
//  altaVisualizer
//
//  Created by Jakob Bak on 19/03/16.
//
//

#ifndef Views_h
#define Views_h

#include <stdio.h>
#include "ofUtils.h"
#include "ofTrueTypeFont.h"
#include "ofGraphics.h"




enum plot2D_type {
    PLOT2D_SCATTER = 0,
    PLOT2D_LINE = 1
};

enum plot3D_type {
    PLOT3D_SCATTER = 0,
    PLOT3D_LINE = 1
};


class plotColors {
public:
    plotColors() {
        used_colors = 0;
        colors.push_back(ofColor(255, 0, 0));   // RED
        colors.push_back(ofColor(0, 255, 0));   // GREEN
        colors.push_back(ofColor(0, 255, 255)); // CYAN
        colors.push_back(ofColor(255, 0, 255)); // MAGENTA
        colors.push_back(ofColor(255, 255, 0)); // YELLOW
        colors.push_back(ofColor(128, 128, 255)); // LIGHT BLUE
        colors.push_back(ofColor(255, 128, 128)); // PINK
        colors.push_back(ofColor(128, 255, 128)); // LIGHT GREEN
        colors.push_back(ofColor(255, 192, 0));   // ORANGE
    }
    ~plotColors(){}
    
    ofColor getNewColor() {
        if(used_colors >= colors.size()) return ofColor(255, 255, 255);
        else return colors[used_colors++];
    }
    
protected:
    vector<ofColor> colors;
    int used_colors;

};




class View {

    typedef struct {
        ofVec2f size;
        ofVec2f position;
        string title;
        ofTrueTypeFont font;
        string font_name;
        int font_size;
        bool using_ttf;
        ofVec2f title_position;
        ofColor title_color;
        ofColor background_color;
        bool update_view;
    } view_config_t;
    
    public:
        View(){}
        View(ofVec2f upperLeftCorner, ofVec2f viewSize, string title = "");
        ~View(){}
        
        void draw();
        
        void changePosition(ofVec2f upperLeftCorner) { view_config.position = upperLeftCorner; }
        void changeSize(ofVec2f view_size) { view_config.size = view_size; }
        
        void changeBackgroundColor(ofColor color) { view_config.background_color = color; }
        
        void changeTitle(string title) { view_config.title = title; }
        void changeTitlePosition(ofVec2f pos) { view_config.title_position = pos; }
        void changeTitleFont(ofTrueTypeFont font) { view_config.font = font; }
        void changeTitleColor(ofColor color) { view_config.title_color = color; }
        
        void setConfiguration(view_config_t new_config) { view_config = new_config; }
        view_config_t getConfiguration() { return view_config; }
    
        void setViewUpdate(bool update) { view_config.update_view = update; }
    
    protected:
        view_config_t view_config;

};




class ViewTextBox : public View {
    typedef struct {
        string text;
        ofVec2f text_position;
        ofColor text_color;
        ofTrueTypeFont text_font;
        int text_font_size;
        string text_font_name;
        bool text_using_ttf;
    } textBox_config_t;
    
public:
    
    ViewTextBox() {}
    ViewTextBox(ofVec2f upperLeftCorner, ofVec2f size, string text, string title = "");
    ~ViewTextBox(){}

    void draw();
    void changeText(string text) { textBox_config.text = text; }
    void changeFontSize(int size);

private:
    textBox_config_t textBox_config;
};




class ViewGraph2D : public View {
    
    typedef struct {
        float xmin;
        float xmax;
        float xrange;
        float ymin;
        float ymax;
        float yrange;
        plot2D_type type;
        ofColor graph_color;
        ofColor axis_color;
        float circle_radius;
        float line_width;
        std::vector<float> x_unit_values;
        std::vector<float> y_unit_values;
        int xprecision;
        int yprecision;
        ofTrueTypeFont text_font;
        int text_font_size;
        string text_font_name;
        bool text_using_ttf;
        float alpha, beta, bias;
    } graph2D_config_t;
    
    public:
        ViewGraph2D() {}
        ViewGraph2D(ofVec2f upperLeftCorner, ofVec2f size, std::vector<ofVec2f> dataSet, plot2D_type type, string title = "[NO TITLE]");
        ~ViewGraph2D(){}
        
        void draw();
        void updateDataSet(std::vector<ofVec2f> dataSet);
        void changeGraphColor(ofColor color) { graph2D_config.graph_color = color; }
        void changeAxisColor(ofColor color) { graph2D_config.axis_color = color; }
        void changeLineWidth(float width) { graph2D_config.line_width = width; }
        void changeCircleRadius(float radius) { graph2D_config.circle_radius = radius; }
    
        void getUnitLines(std::vector<float> * unit_values, int * precision, float min, float max);
    
    
    private:
        std::vector<ofVec2f> data;
        graph2D_config_t graph2D_config;
        
};




class ViewRealtimePlotter2D : public View {
    typedef struct {
        vector<string> value_type;
        vector<float> min;
        vector<float> max;
        vector<float> range;
        bool centered;
        vector<ofColor> graph_color;
        vector<bool> visible;
        
        float graph_time_span;
        ofColor axis_color;
        float line_width;
        
        ofTrueTypeFont text_font;
        int text_font_size;
        string text_font_name;
        bool text_using_ttf;
    } RealtimeGraph2D_config_t;
    
public:
    ViewRealtimePlotter2D() {}
    ViewRealtimePlotter2D(ofVec2f upperLeftCorner, ofVec2f size, string title = "REALTIME PLOTTER");
    ~ViewRealtimePlotter2D(){}
    
    void draw();
    void updateGraph();
    void addDataPoint(string s, float value);
    int getValueTypeVectorIndex(string value_type);
    void setTimeSpan(float time_span) { realtime_plotter_config.graph_time_span = time_span; }
    float getTimeSpan() { return realtime_plotter_config.graph_time_span; }
    void setGraphsCentered(bool centered) { realtime_plotter_config.centered = centered; }
    void hideGraph(int value_type);
    void showGraph(int value_type);
    void toggleGraphVisibility(int value_type);
    
private:
    vector<vector<pair<float,float>>> data;
    vector<vector<ofVec2f>> graph;
    RealtimeGraph2D_config_t realtime_plotter_config;
    plotColors pallette;
    
};


class ViewGraph3D : public View {
    
    typedef struct {
        float xmin;
        float xmax;
        float xrange;
        float ymin;
        float ymax;
        float yrange;
        float zmin;
        float zmax;
        float zrange;
        plot3D_type type;
        ofColor graph_color;
        ofColor axis_color;
        float circle_radius;
        float line_width;
        std::vector<float> x_unit_values;
        std::vector<float> y_unit_values;
        std::vector<float> z_unit_values;
        int xprecision;
        int yprecision;
        int zprecision;
        ofTrueTypeFont text_font;
        int text_font_size;
        string text_font_name;
        bool text_using_ttf;
        vector<ofVec3f> curveFittingParameters;
        float alpha, beta, gamma;
    } graph3D_config_t;
    
public:
    ViewGraph3D() {}
    ViewGraph3D(ofVec2f upperLeftCorner, ofVec2f size, std::vector<vector<ofVec3f>> dataSet, plot3D_type type, string title = "[NO TITLE]");
    ~ViewGraph3D(){}
    
    void draw();
    void updateDataSet(std::vector<vector<ofVec3f>> dataSet);
    void updateCurveFittingParameters(vector<ofVec3f> parameters) { graph3D_config.curveFittingParameters = parameters; }
    void changeGraphColor(ofColor color) { graph3D_config.graph_color = color; }
    void changeAxisColor(ofColor color) { graph3D_config.axis_color = color; }
    void changeLineWidth(float width) { graph3D_config.line_width = width; }
    void changeCircleRadius(float radius) { graph3D_config.circle_radius = radius; }
    
    void getUnitLines(std::vector<float> * unit_values, int * precision, float min, float max);
    
    
private:
    std::vector<vector<ofVec3f>> data;
    graph3D_config_t graph3D_config;
    plotColors pallette;
    
};




#endif /* Views_h */

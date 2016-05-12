#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxGrt.h"

using namespace GRT;

class ofApp : public ofBaseApp{

public:
    void setup();
	void update();
	void draw();

    void scanDirectory(ofDirectory dir);
	void keyPressed(int key);
    
    //Worksheet format
    int optionIndex;
    void option(int index, int m, int startX, int startY, int endX, int len);
    void drawText(string text);
    
    //Create variables for GUI
    ofDirectory dir;
    ofTrueTypeFont font;
    ofxDatGuiButton* button;
    ofxDatGuiButton* buttonB;
    ofxDatGuiButton* buttonC;
    ofxDatGuiButton* buttonD;
    ofxDatGuiDropdown* menuA;
    ofxDatGuiScrollView* menuB;
    ofxDatGuiTextInput* textGUI;
    ofxDatGuiButton* myToggle;
    string dataName;
    string dataSavingPath;
    string dataLoadingPath;
    int m;
    bool mode;
    vector<string> options;
    float rNumber;
    
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onClearButtonEvent(ofxDatGuiButtonEvent e);
    void onButtonEventC(ofxDatGuiButtonEvent e);
    void onButtonEventD(ofxDatGuiButtonEvent e);
    void onButtonEventE(ofxDatGuiButtonEvent e);
    void onDropDownEvent(ofxDatGuiDropdownEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    void onScrollViewEvent(ofxDatGuiScrollViewEvent e);
    
    //Create variables for GRT
    TimeSeriesClassificationData trainingData;
    MatrixFloat timeseries;
    GestureRecognitionPipeline pipeline;
    UINT trainingClassLabel;
    string infoText;
    string infoText2;
    string dataLabel;
    string textInput;
    bool down;
    bool record;
    bool load;
    bool predict;
    vector<ofVec3f> aStroke;
    string modeX;
    string getName;
    
    void drawTimeseries();
    void drawGuideline();
    void drawInput();
    void loadGuideLine();
    
    //Create variables for training
    int numClasses;
    vector<ofFile> dataFiles;
    vector<string> data;
    UINT learningClassLabel;
    vector<MatrixDouble> guidePerClass;
    
    double accuracy;
    TimeSeriesClassificationData testData;
    UINT predictedClassLabel;
    double maximumLikelihood;
    VectorDouble classLikelihoods;
    VectorDouble classDistances;
    Vector< VectorFloat> inputData;
    int average;

		
};

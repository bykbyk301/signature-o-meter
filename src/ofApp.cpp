#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::scanDirectory(ofDirectory dir){
    ofDirectory* newDir;
    int numFiles = dir.listDir();
    for (int i=0; i<numFiles; i++){
        if (dir.getFile(i).isDirectory()){
            newDir = new ofDirectory(dir.getFile(i).getAbsolutePath());
            scanDirectory(*newDir);
        } else {
            dataFiles.push_back(dir.getFile(i));
            data.push_back(dir.getName(i));
        }
    }
    cout << "Loaded data " << ofToString(data) << endl;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    
    //Load files
    dir = ofDirectory(ofToDataPath("./samples"));
    scanDirectory(dir);
    font.load("verdana.ttf", 12, true, true);
    
    //Set up GUI
    int x = 10;
    int y = 150;
    int p = 20;
    m = 150;
    dataName = "";
    optionIndex = 0;

    menuB = new ofxDatGuiScrollView("SELECT DATA", 13);
    for (int i=0; i<data.size(); i++){
        menuB->add(ofToString(data[i]));
    }
    menuB->setPosition(x,y);
    menuB->onScrollViewEvent(this, &::ofApp::onScrollViewEvent);

    x += menuB->getWidth() + 65;
    buttonD = new ofxDatGuiButton("Start");
    buttonD->setPosition(x + 75, y + 50);
    buttonD->setWidth(75);
    buttonD->onButtonEvent(this, &ofApp::onButtonEventE);
    
    x += menuB->getWidth() + 150;
    y = 160;
    buttonC = new ofxDatGuiButton("try again");
    buttonC->setPosition(x,y);
    buttonC->setWidth(95);
    buttonC->onButtonEvent(this, &ofApp::onButtonEventC);
    
    //-------------------------------------------------------
    
    x = buttonD->getX();
    y = 240;
    buttonB = new ofxDatGuiButton("Clear");
    buttonB->setPosition(x,y);
    buttonB->setWidth(75);
    buttonB->onButtonEvent(this, &ofApp::onClearButtonEvent);
    
    x = menuB->getWidth()*2 + 40;
    y = 255;
    modeX = "other";
    myToggle = new ofxDatGuiButton("Go to " + modeX + " mode");
    myToggle->setPosition(x,y);
    myToggle->onButtonEvent(this, &ofApp::onButtonEventD);
    
    x = myToggle->getX();
    y = buttonC->getY();
    textInput = "TYPE HERE";
    textGUI = new ofxDatGuiTextInput("your name :", textInput);
    textGUI->setPosition(x,y);
    textGUI->onTextInputEvent(this, &ofApp::onTextInputEvent);
    
    y += textGUI->getHeight() + p;
    button = new ofxDatGuiButton("ADD MY SIGNATURE");
    button->setPosition(x,y);
    button->onButtonEvent(this, &ofApp::onButtonEvent);
    
    //Set up GRT
    infoText = "";
    trainingClassLabel = 1;
    learningClassLabel = 1;
    predictedClassLabel = 0;
    down = false;
    record = false;
    load = false;
    predict = false;
    mode = false;
    accuracy = 0;
    numClasses = 0;
    rNumber = 0;
    
    trainingData.setNumDimensions(3);
    DTW dtw;
    dtw.enableNullRejection(true);
    dtw.setNullRejectionCoeff(3);
    dtw.enableTrimTrainingData(true, 0.1, 90);
    dtw.setOffsetTimeseriesUsingFirstSample(true);
    dtw.setContrainWarpingPath(true);
    pipeline.setClassifier(dtw);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    //Update GUI
    menuB->update();
    textGUI->update();
    button->update();
    buttonB->update();
    buttonC->update();
    buttonD->update();
    myToggle->update();

    
    //Update GRT
    VectorDouble sample(3);
    sample[0] = mouseX;
    sample[1] = mouseY;
    sample[2] = down;
    
    if (ofGetMousePressed()) {
        down = true;
    } else {
        down = false;
    }
    
    //Record GRT
    if(record){
        //Add sample to training data
        timeseries.push_back(sample);
    }
    
    //Predict GRT
    if(pipeline.getTrained()){
        predict = true;
        accuracy = 0;
        pipeline.predict(sample);
        rNumber = ofRandom(0.0, 95.0);
        //Update prediction
        predictedClassLabel = pipeline.getPredictedClassLabel();
        maximumLikelihood = pipeline.getMaximumLikelihood();
        classLikelihoods = pipeline.getClassLikelihoods();
        classDistances = pipeline.getClassDistances();
            
        //Calculate accuracy
        if (learningClassLabel == predictedClassLabel) accuracy++;
            if (accuracy > 1) {
                float accuracyPercent;
                average = accuracy/double(inputData.getSize())*100.0;
                vector<float> accuracyAverage;
                accuracyAverage.push_back(accuracyPercent);
                float sum = 0.0;
                for (int n : accuracyAverage ) {
                    sum+= n;
                }
//           average = sum/double(inputData.getSize());
        cout << "average is: " << accuracyPercent << endl;
            }
    }
        
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
    
    //Draw training info
    string text;
    int textX = 20;
    int textY = 125;
    
    //Draw worksheet template
    ofSetColor(100);
    ofSetLineWidth(2);
    int startX = ofGetWidth()/2-m;
    int startY = ofGetHeight()*0.45;
    int endX = ofGetWidth()/2+m;
    int len = m*2;
    
    if (optionIndex == 0) {
        ofBackground(255);
        ofSetColor(0);
        font.setLineHeight(16.0f);
        ofDrawBitmapString("Signature-O-Meter by BO KIM", 20, 40);
        // Try to forge someone else's signature and add your own!
        //Draw worksheet for existing data
        ofDrawLine(startX, startY, endX, startY);
        int endY = startY + 150;
        ofDrawLine(startX, endY, endX, endY);
        for (int i=0; i<len/10; i++) {
            int spacing = i*(len/20);
            int middleY = (endY - startY)/2 + startY;
            if(startX+spacing < endX) {
                ofDrawLine(startX+spacing, middleY, (startX+spacing) + (len/40), middleY);
            }
        }
        
        //GUI for existing data
        text = "---------1) SELECT SIGNATURE----------";
        ofDrawBitmapString(text, textX, textY);
        
        textX += menuB->getWidth() + 20;
        text = "------------2) FORGE IT!--------------";
        ofDrawBitmapString(text, textX, textY);
        
        textY = 175;
        text = "[InfoText: " + infoText + "]";
        ofDrawBitmapString(text, textX, textY);
        
        //Draw prediction info
        textX += menuB->getWidth() + 20;
        textY = 125;
        text = "-------------YOUR SCORE---------------";
        ofDrawBitmapString(text, textX, textY);
        
        textY = 175;
        text = "SCORE: " + ofToString(rNumber);
        ofDrawBitmapString(text, textX, textY);
        
        textY = 230;
        text = "---------WANNA ADD YOUR OWN?----------";
        ofDrawBitmapString(text, textX, textY);
        
        menuB->draw();
        buttonC->draw();
        myToggle->draw();
        buttonD->draw();
        buttonB->draw();

    } else if (optionIndex == 1) {
        ofBackground(255);
        ofSetColor(0);
        font.setLineHeight(16.0f);
        ofDrawBitmapString("Signature-O-Meter by BO KIM", 20, 40);
        // Try to forge someone else's signature and add your own!
        ofDrawLine(startX, startY, endX, startY);
        int endY = startY + 150;
        ofDrawLine(startX, endY, endX, endY);
        for (int i=0; i<len/10; i++) {
            int spacing = i*(len/20);
            int middleY = (endY - startY)/2 + startY;
            if(startX+spacing < endX) {
                ofDrawLine(startX+spacing, middleY, (startX+spacing) + (len/40), middleY);
            }
        }
        
        int textX = 20;
        int textY = 125;
        
        //GUI for new data
        text = "-------------INSTRUCTION-------------";
        ofDrawBitmapString(text, textX, textY);
        
        ofSetColor(0);
        textY += 30;
        text = "1) Press R key";
        ofDrawBitmapString(text, textX, textY);
        if (record) ofSetColor(255,0,0);
        else ofSetColor(0,0,255);
        textX += 200;
        text = record ? "RECORDING" : "Not recording";
        ofDrawBitmapString(text, textX, textY);
        ofSetColor(0);
        text = "2) Write your signature";
        textX -= 200;
        textY += 25;
        ofDrawBitmapString(text, textX, textY);
        text = "3) Press R again";
        textY += 25;
        ofDrawBitmapString(text, textX, textY);
        text = "Do this 10 times";
        textY += 25;
        ofDrawBitmapString(text, textX, textY);
        text = "If you want to start over click CLEAR";
        textY += 25;
        ofDrawBitmapString(text, textX, textY);
        
        textY += 50;
        text = "Number of Sample:                / 10"; //Number of training samples
        ofDrawBitmapString(text, textX, textY);
        if(trainingData.getNumSamples() < 10) ofSetColor (255,0,0);
        else ofSetColor(0,255,0);
        textX += 200;
        text = ofToString(trainingData.getNumSamples());
        ofDrawBitmapString(text, textX, textY);

        textX = menuB->getWidth() + 40;
        textY = 175;
        text = "[InfoText: " + infoText2 + "]";
        ofDrawBitmapString(text, textX, textY);
        
        ofSetColor(0);
        text = "-----TYPE YOUR NAME AND CLICK ADD-----";
        textX = myToggle->getX();
        textY = 125;
        ofDrawBitmapString(text, textX, textY);
        
        textGUI->draw();
        button->draw();
        buttonB->draw();
        myToggle->draw();
    }


    if(record){
        //Draw handwriting to record
        drawTimeseries();
    } else if (load && optionIndex == 0) {
        //Display data
        drawGuideline();
    } else if (pipeline.getTrained()) {
        drawInput();
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    string labelName = "";
    
    switch (key) {
        case 'r':
            record = !record;
            if(!record){
                trainingData.addSample(trainingClassLabel, timeseries);
                labelName = string("Class: ") + grt_to_str<unsigned int>(trainingClassLabel);
                timeseries.clear();
            }
            break;
        case '[':
            if(!pipeline.getTrained()){
                if (trainingClassLabel > 1)
                    trainingClassLabel--;
            }
            break;
        case ']':
            if(!pipeline.getTrained()){
                if (trainingClassLabel > 0)
                    trainingClassLabel++;
            }
            break;
        case '-':
            if (pipeline.getTrained()){
                if (learningClassLabel > 1)
                    learningClassLabel--;
                    accuracy = 0;
            }
            break;
        case '=':
            if (pipeline.getTrained()){
                if (learningClassLabel < numClasses)
                    learningClassLabel++;
                    accuracy = 0;
            }
            break;
        default:
            break;
    }

}

//--------------------------------------------------------------

//-----------------------DRAWING EVENTS-------------------------

//--------------------------------------------------------------


void ofApp::drawTimeseries(){
    ofFill();
    for(UINT i=0; i<timeseries.getNumRows(); i++){
        double x = timeseries[i][0];
        double y = timeseries[i][1];
        double z = timeseries[i][2];
        double c = ofMap(z,0,1,255,0);
        
        if (z == 1) {
            ofDrawEllipse(x,y,5,5);
        }
    }
    ofSetColor(0);
    ofSetLineWidth(3);
}


//--------------------------------------------------------------
void ofApp::loadGuideLine() {
    //Load data
    load = true;
    aStroke = *new vector<ofVec3f>;
    vector<MatrixDouble> guidePerClass;
    for (UINT i=0; i<trainingData.getNumSamples(); i++){
        MatrixDouble getData;
        UINT classLabel;
        classLabel = trainingData[i].getClassLabel();
        if (classLabel == learningClassLabel) {
            getData = trainingData[i].getData();
            guidePerClass.push_back(getData);
        }
    }
    for (UINT i=0; i<guidePerClass.size(); i++){
        MatrixDouble myClass = guidePerClass[0];
        int numRows = myClass.getNumRows();
        for (UINT j=0; j<numRows; j++) {
            double x = myClass[j][0];
            double y = myClass[j][1];
            double z = myClass[j][2];
            
            ofVec3f aPoint = ofVec3f(x,y,z);
            aStroke.push_back(aPoint);
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawGuideline() {
    ofPushStyle();
//    ofTranslate(0, 100, 0);

    //Draw Signature Preview
    int px = aStroke.at(0).x;
    int py = aStroke.at(0).y;
    ofBeginShape();
    for (ofVec3f aPoint : aStroke) {
        int x = aPoint.x;
        int y = aPoint.y;
        int p = aPoint.z;
        int w = ofMap(p, 0,1, 0,70);
        
        ofSetColor(200, 70, 70, w);
        ofSetLineWidth(5);
        ofEnableSmoothing();
        
        ofDrawLine(x, y, px, py);
        px = x;
        py = y;
    }
    ofEndShape();
    ofPopStyle();
}


//--------------------------------------------------------------
void ofApp::drawInput(){
    ofFill();
    DTW *dtw = pipeline.getClassifier< DTW >();
    
    if (dtw != NULL) {
        float x,y,z,r;
        Vector< VectorFloat > inputData = dtw->getInputDataBuffer();
        for (UINT i=0; i<inputData.getSize(); i++) {
            x = inputData[i][0];
            y = inputData[i][1];
            z = inputData[i][2];
            r = ofMap(i,0,inputData.getSize(), 0,255);
                
            if(z == 1) {
                ofSetColor(r,100,100, 100);
                ofDrawEllipse(x, y, 10, 10);
            }
        }
    }
}

//--------------------------------------------------------------
//-----------------------GUI EVENTS-----------------------------
//--------------------------------------------------------------


void ofApp::onDropDownEvent(ofxDatGuiDropdownEvent e){
    //Load training data
    trainingData.clear();
    ofxDatGuiButton* button = e.target;
    string getName = button->getLabel();
    dataLoadingPath = "./samples/" + getName;
    learningClassLabel = 1;
    
    trainingData.loadDatasetFromFile(ofToDataPath(dataLoadingPath));
    if(trainingData.loadDatasetFromFile(ofToDataPath(dataLoadingPath))){
        load = false;
        infoText = "Training data load to file";
        ofDrawBitmapString(getName, ofGetWidth()/2, 200);
    } else infoText = "Failed to load training data from file";
    numClasses = trainingData.getNumClasses();
    cout << "onDropdownEvent: " << getName << " was selected" << endl;
    
    loadGuideLine();
}

//--------------------------------------------------------------
void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e){
    //Set text input as filename
    dataName = ofToString(e.text) + ".txt";
    dataSavingPath = "./samples/" + dataName;
    cout << "onButtonEvent: " << e.text << endl;
    
}

//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if ( trainingData.getNumSamples() < 10) {
        infoText2 = "CAN'T SAVE - Need more samples!";
    } else {
        //Save training data
        trainingData.saveDatasetToFile(ofToDataPath(dataSavingPath));
        
        //Add to the dropdown menu
        scanDirectory(dir);
        for (int i=0; i<data.size(); i++){
            if (ofToString(data[i]) == dataName) {
                options.push_back(ofToString(data[i]));
            }
        }
        if (trainingData.saveDatasetToFile( ofToDataPath(dataSavingPath))) {
            infoText = "Handwriting data added to file";
        } else infoText = "Failed to save file";
        cout << "onButtonEvent: " << e.target->getLabel() << "::" << e.enabled << endl;
        
        trainingData.clear();
        textInput = "";
        cout << "Training data is cleared" << endl;
    }

}

//--------------------------------------------------------------
void ofApp::onClearButtonEvent(ofxDatGuiButtonEvent e){
    trainingData.clear();
    pipeline.clear();
    textInput = "";
    infoText = "Training data is cleared";
    cout << "Training data is cleared" << endl;

}

//--------------------------------------------------------------
void ofApp::onButtonEventC(ofxDatGuiButtonEvent e){
    //Train data
    pipeline.train(trainingData);
    if (!pipeline.getTrained()) {
        pipeline.train(trainingData);
    } else if(pipeline.train(trainingData)){
        infoText = "Pipeline trained";
        cout << "Classifier trained" << endl;
    } else infoText = "Failed to train pipeline";
}


void ofApp::onButtonEventE(ofxDatGuiButtonEvent e){
    //Train data
    pipeline.train(trainingData);
    if (!pipeline.getTrained()) {
        pipeline.train(trainingData);
    } else if(pipeline.train(trainingData)){
        infoText = "Pipeline trained";
        cout << "Classifier trained" << endl;
    } else infoText = "Failed to train pipeline";
}


//--------------------------------------------------------------
void ofApp::onButtonEventD(ofxDatGuiButtonEvent e){
    pipeline.clear();
    if (optionIndex == 0) {
        optionIndex = 1;
        modeX = "ADD";
        trainingData.clear();
    } else if (optionIndex == 1) {
        optionIndex = 0;
        modeX = "GAME";
        trainingData.clear();
    }
    
}

    
//--------------------------------------------------------------
void ofApp::onScrollViewEvent(ofxDatGuiScrollViewEvent e){
    //Load training data
    trainingData.clear();
    ofxDatGuiButton* button = e.target;
    getName = button->getLabel();
    dataLoadingPath = "./samples/" + getName;
    learningClassLabel = 1;
    
    trainingData.loadDatasetFromFile(ofToDataPath(dataLoadingPath));
    if(trainingData.loadDatasetFromFile(ofToDataPath(dataLoadingPath))){
        load = false;
        infoText = "Training data load to file";
        ofDrawBitmapString(getName, ofGetWidth()/2, 200);
    } else infoText = "Failed to load training data from file";
    numClasses = trainingData.getNumClasses();
    cout << "onDropdownEvent: " << getName << " was selected" << endl;
    
    loadGuideLine();

}


//--------------------------------------------------------------
void ofApp::option(int index, int m, int startX, int startY, int endX, int len) {
}


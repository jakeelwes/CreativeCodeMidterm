#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ofxDatGui.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    bool mFullscreen;
    void refreshWindow();
    void toggleFullscreen();
    void keyPressed(int key);
    
    
    // particles
    
    std::vector<Particle> particles;
    
    float cageSize, numNodes;
    int count;
    
    
    // NOISE FIELD (for moving attractor)
    
    ofPoint noiseField(ofPoint position);
    
    float phase = TWO_PI; // separate u-noise from v-noise
    float speedLim = 0.8; // particle speed cap
    float jitter = 3; // wind complexity
    float timeSpeed = .05; // wind variation speed
    float t, cx, cy;
    
    ofPoint attractorCenter;
    
    
    // GUI
    
    void guiSetup();
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);
    //        void on2dPadEvent(ofxDatGui2dPadEvent e); // using co-ordinates to move center before noise field
    
    void movingCenterToggle();
    
    ofxDatGui* gui;
    ofxDatGuiFolder* AMSFolder;
    
    bool guiVis = true;
    bool movingCenter = false;
    
    float soundReactivityAlpha;
    float soundReactivityAttract;
    float bgBrightness;

    
    
    // SOUND
    
    void soundSetup();
    void audioIn(float * input, int bufferSize, int nChannels);
    void soundPlayer();
    
    ofSoundStream soundStream;
    ofSoundPlayer bach;
    
    std::vector <float> left;
    std::vector <float> right;
    
    int bufferCounter;
    int drawCounter;
    bool soundP;
    
    float smoothedVol;
    float scaledVol;
    
};

#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ofxDatGui.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    // particles
    
    
        std::vector<Particle> particles;
		
        float cageSize, numNodes;
    

    
    // GUI
    
        void guiSetup();
        void onSliderEvent(ofxDatGuiSliderEvent e);
        void onButtonEvent(ofxDatGuiButtonEvent e);
//        void on2dPadEvent(ofxDatGui2dPadEvent e);

    
        ofxDatGui* gui;
    
        float soundReactivityAlpha;
        float soundReactivityAttract;
        float bgBrightness;
//        float attractorCenterx;
//        float attractorCentery;
    
    
    // SOUND
    
        void soundSetup();
        void audioIn(float * input, int bufferSize, int nChannels);
    
        ofSoundStream soundStream;

        std::vector <float> left;
        std::vector <float> right;
    
        int bufferCounter;
        int drawCounter;
    
        float smoothedVol;
        float scaledVol;

};

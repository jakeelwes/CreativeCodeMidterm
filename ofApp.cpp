#include "ofApp.h"
//#include "Particle.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    cageSize = ofGetHeight()-100;
    
    numNodes = 2000;
    
    soundReactivityAlpha = 200;
    soundReactivityAttract = 4;
    bgBrightness = 60;
    
    
    // Initialize particles
    for(std::size_t i = 0; i < numNodes; i++){
        Particle particle;
        particle.position = ofPoint(ofRandom(cageSize), ofRandom(cageSize), ofRandom(cageSize));
        particle.velocity = ofPoint(ofRandom(-3,3), ofRandom(-3,3), ofRandom(-3,3));
        
        particles.push_back(particle);
    }
    
    soundSetup();
    guiSetup();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // collision detection
    for(std::size_t i = 0; i < numNodes; i++){
        particles[i].update();
        
        if(particles[i].position.x > cageSize || particles[i].position.x < 0) {
            particles[i].velocity.x *= -1.0;
        }
        
        if(particles[i].position.y > cageSize || particles[i].position.y < 0) {
            particles[i].velocity.y *= -1.0;
        }
        if(particles[i].position.z > cageSize || particles[i].position.z < 0) {
            particles[i].velocity.z *= -1.0;
        }
    }
    
    // normalize volume input
	scaledVol = ofMap(sqrt(smoothedVol), 0.0, 0.412, 0.0, 1.0, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cageSize = ofGetHeight()-100;

    ofBackgroundGradient(bgBrightness,bgBrightness-60);
    
    // translate 3d object to center
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2-cageSize/2,ofGetHeight()/2-cageSize/2,-ofGetHeight()/2);
    
//    ofRotate(mouseX/2, 0, 1, 0 );
//    ofRotate(mouseY/2, 1, 0, 0);

    float minimumDist = scaledVol * soundReactivityAlpha;
    
    for (std::size_t i = 0; i < particles.size(); i++)
    {
        
        // seeking force (to center), attraction power based on sound input
        particles[i].seek(ofPoint(cageSize/2,cageSize/2,cageSize/2));
        particles[i].power = scaledVol * soundReactivityAttract;


        for (std::size_t j = 0; j < i; j++)
        {
            float ppDist = particles[i].position.distance(particles[j].position);
            
            if (ppDist < minimumDist)
            {
                float alpha = ofMap(ppDist, 0, minimumDist, 255, 0);
                float col;
                if (bgBrightness < 150){
                    col = 255;
                } else {
                   col = 0;
                }
                ofSetColor(col, alpha);
                
                ofDrawLine(particles[i].position, particles[j].position);
            }
        }
    }
    
    ofPopMatrix();
}


// GUI

void ofApp::guiSetup(){
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    gui->addSlider("Sound Reactivity Alpha", 50, 800);
    gui->addSlider("Sound Reactivity Attract", -10, 16);
    gui->addSlider("maxAttractionForce", 0, 0.2);
//    ofxDatGui2dPad* pad = gui->add2dPad("Attraction Center");
    gui->addSlider("Background Brightness", 60, 255);
    gui->addButton("Reset Velocity");
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
//    gui->on2dPadEvent(this, &ofApp::on2dPadEvent);
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->setOpacity(0.2);
    gui->addFooter();
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e){
    if (e.target->is("Sound Reactivity Alpha")) soundReactivityAlpha = e.target->getValue();
    if (e.target->is("Sound Reactivity Attract")) soundReactivityAttract = e.target->getValue();
    if (e.target->is("maxAttractionForce"))
        for(std::size_t i = 0; i < numNodes; i++){
            particles[i].maxForce = e.target->getValue();
        }
    if (e.target->is("Background Brightness")) bgBrightness = e.target->getValue();
    
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if (e.target->is("Reset Velocity"))
        for(std::size_t i = 0; i < numNodes; i++){
            particles[i].velocity = ofPoint(0,0,0);
        }
}

//void ofApp::on2dPadEvent(ofxDatGui2dPadEvent e){
//    attractorCenterx = ofMap(e.x, 9.9, 1014,0,cageSize);
//    attractorCentery = ofMap(e.y, 14.4, 763.2,0,cageSize);
//}


// SOUND

void ofApp::soundSetup(){
    
    int bufferSize = 256;
    
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
}

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}



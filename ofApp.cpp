#include "ofApp.h"

void ofApp::setup(){
    
    cageSize = ofGetHeight()-100;
    
    numNodes = 2000;
    
    soundReactivityAlpha = 100;
    soundReactivityAttract = 4;
    bgBrightness = 60;
    soundP = false;
    
    
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

void ofApp::update(){
    

    // particle collision detection
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
    
    //attractor noise and collision detection
    
    t = ofGetFrameNum() * timeSpeed; // time value for noiseField
    
    ofPoint field = noiseField(attractorCenter);
    float speed = (1 + ofNoise(t, field.x, field.y)) / speedLim;
    
    attractorCenter.x += ofLerp(-speed, speed, field.x);
    attractorCenter.y += ofLerp(-speed, speed, field.y);

    if(attractorCenter.x > cageSize || attractorCenter.x < 0) {
        attractorCenter.x = ofGetWidth()/2;
    }
    
    if(attractorCenter.y > cageSize || attractorCenter.y < 0) {
        attractorCenter.y = ofGetHeight()/2;
    }
    
    // normalize volume input
    if(!soundP){
        scaledVol = ofMap(sqrt(smoothedVol), 0.0, 0.412, 0.0, 1.0, true);
    }
    
    // for SoundPlayer get levels
    if (soundP) {
        float * val = ofSoundGetSpectrum(1);
        scaledVol = val[0]*10;
    }
}


void ofApp::draw(){
    
    cageSize = ofGetHeight()-100;

    ofBackgroundGradient(bgBrightness,bgBrightness-60);
    
    // translate 3d object to center
    ofPushMatrix();
    
// failed attempt to rotate object (can't find midpoint)
//    count = (count + 1) % 360;
//    ofRotate(count, 0, 1, 0);

    ofTranslate(ofGetWidth()/2-cageSize/2,ofGetHeight()/2-cageSize/2, -cageSize);

    float minimumDist = scaledVol * soundReactivityAlpha;
    
    for (std::size_t i = 0; i < particles.size(); i++)
    {
        
        // seeking force (to center), attraction power based on sound input
        particles[i].seek(ofPoint(attractorCenter.x,attractorCenter.y,cageSize/2));
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

// ---------------- NOISE FIELD ----------------

ofPoint ofApp::noiseField(ofPoint position) {
    float normx = ofNormalize(position.x, 0, ofGetWidth());
    float normy = ofNormalize(position.y, 0, ofGetHeight());
    float u = ofNoise(t + phase, normx * jitter + phase, normy * jitter + phase);
    float v = ofNoise(t - phase, normx * jitter - phase, normy * jitter + phase);
    return ofPoint(u, v);
}

// ---------------- GUI ----------------

void ofApp::guiSetup(){
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );

    gui->addSlider("Sound React Alpha", 50, 800);
    gui->addSlider("Sound Attract", -10, 16);
    gui->addSlider("max Attract Force", 0, 0.2);
    gui->addSlider("Brightness", 60, 255);
    gui->addButton("Reset Velocity");
    gui->addButton("Play Sound");
    gui->addButton("Stop Sound");
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->setOpacity(0.2);
    gui->addHeader(":: drag to reposition ::");
    gui->addFooter();
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e){
    if (e.target->is("Sound React Alpha")) soundReactivityAlpha = e.target->getValue();
    if (e.target->is("Sound Attract")) soundReactivityAttract = e.target->getValue();
    if (e.target->is("max Attract Force"))
        for(std::size_t i = 0; i < numNodes; i++){
            particles[i].maxForce = e.target->getValue();
        }
    if (e.target->is("Brightness")) bgBrightness = e.target->getValue();
    
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if (e.target->is("Reset Velocity"))
        for(std::size_t i = 0; i < numNodes; i++){
            particles[i].velocity = ofPoint(0,0,0);
        }
    if (e.target->is("Play Sound")) soundPlayer(), soundP = true;
    if (e.target->is("Stop Sound")) bach.stop(), soundP = false;
}

// ---------------- SOUND ----------------

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
    
    int numCounted = 0;
    
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    curVol /= (float)numCounted;
    
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}

void ofApp::soundPlayer(){
    
    bach.load("bach.mp3");
    bach.play();
    bach.setLoop(true);
}


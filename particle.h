#pragma once

#include "ofMain.h"

class Particle : public ofBaseApp{
    
public:
    
    Particle();
    void update();
    void applyForce(ofPoint force);
    void seek(ofPoint target);
    
    ofPoint position, velocity, acceleration, desired;
    
    float maxSpeed, maxForce, power;
    
};
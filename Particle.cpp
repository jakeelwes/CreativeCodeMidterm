#include "Particle.h"


Particle::Particle(){
    acceleration = ofPoint(0,0,0);
    position = ofPoint(0,0,0);
    velocity = ofPoint(0,0,0);
    
    maxSpeed = 4;
    maxForce = 0.2;
}

void Particle::update() {
    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    acceleration *= 0;
}

void Particle::applyForce(ofPoint force){
    acceleration += power*force;
}

void Particle::seek(ofPoint target){
    
    desired = ofPoint(target - position);
    desired.getNormalized();
    desired * maxSpeed;
    
    ofPoint steer = ofPoint(desired - velocity);
    
    steer.limit(maxForce);
    applyForce(steer);
}
#include "P6Particle.h"

using namespace P6;

void P6Particle::UpdatePosition(float time) {
	//p2 = p1 + v t
	//Position = Position + Velocity * time;

	//p2 = p1 Vt+ [(At^2)/2]

	float NewTime = time * time;
	this->Position = this->Position + (this->Velocity.scalarMultiplication(time)) + (this->Acceleration.scalarMultiplication(NewTime).scalarMultiplication(0.5f));
}

void P6Particle::UpdateVelocity(float time) {
	//Vf = Vi A t
	this->Velocity = this->Velocity + (this->Acceleration.scalarMultiplication(time));
}

void P6Particle::update(float time) {

	//update position first
	this->UpdatePosition(time);

	//update velocity next
	this->UpdateVelocity(time);

}

void P6Particle::StopParticle() {
	if (glm::length(glm::vec2(Position.x, Position.y)) < 1.0f) {
		this->Velocity = MyVector(0.0f, 0.0f, 0.0f);
		this->moving = false;
	}
}
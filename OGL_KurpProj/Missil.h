#pragma once
#include "Model.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
class Missil
{
	glm::vec3 velocity;
	glm::vec3 position;
	float life;

public:
	Missil(glm::vec3 velocity, glm::vec3 position, float life) : velocity(velocity), position(position), life(life) {}

	void Update(float dt) {
		life -= dt;
		if (life > 0.0f)
			position += dt * velocity;
	}

	float getLife() {
		return life;
	}

	glm::vec3 getVelocity() {
		return velocity;
	}

	glm::vec3 getPosition() {
		return position;
	}
};

class MissilRejected {
	vector<Missil> rejected;

public:
	vector<Missil>& getRejected() {
		return rejected;
	}

	void addMissil(Missil missil) {
		rejected.push_back(missil);
	}
	void Update(float dt) { 
		vector<Missil> alive;
		for (Missil& missil : rejected) {
			missil.Update(dt);
			if (missil.getLife() > 0)
				alive.push_back(missil);
		}
		rejected = alive;

	}
};


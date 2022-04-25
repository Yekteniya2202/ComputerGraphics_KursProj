#pragma once
#include "Model.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
using namespace std;
namespace TankIds {

	static vector<int> ids;

	

	static int GetId() {
		if (ids.empty()) {
			ids.push_back(1);
			return 1;
		}
		int newId = ids[ids.size() - 1];
		newId++;
		ids.push_back(newId);
		return newId;
	}

};
class Tank
{
private:

	//вращаем вокруг y
	float yaw = 0.0f;
	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec3 dir = glm::vec3(sin(glm::radians(yaw)), 0, cos(glm::radians(yaw)));
	float x = 0, y = 0, z = 0;
	float scale = 0.001f;
	float speed = 2.0f;
	float rotationSpeed = 1.f;
	float angle = 0;
	bool selected = false;;
	float deltaTime;
	int id;
	
public:
	Tank(){
		this->id = TankIds::GetId();
	}
	Tank(float x, float z){
		pos = glm::vec3(x, 0, z);
		this->id = TankIds::GetId();
	}
	Tank(float x, float z, float scale, float speed, float RotationSpeed) : x(x), z(z), scale(scale), speed(speed), rotationSpeed(RotationSpeed) {}

	

	void setSelected(int id) {
		if (id == this->id) selected = true;
	}

	void setNotSelected() {
		selected = false;
	}
	void Rotate(int dir, float delta) {
		if (selected) {
			yaw += 1;
			auto dirDelta = dir * delta;
			this->dir = glm::normalize(glm::vec3(dirDelta * sin(glm::radians(yaw)), 0, dirDelta * cos(glm::radians(yaw))));
		}
		
	}
	
	void Move(float delta) {
		if (selected) {
			pos += delta * speed * dir;
		}
	}

	void TransInfo(ModelTransform* trans) {
		trans->position = pos;
		trans->rotation.y = yaw;
		trans->setScale(scale);
	}

	int GetId() {
		return id;
	}


};


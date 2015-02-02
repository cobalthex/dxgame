#pragma once

#include "Camera.hpp"

//A camera that can "chase" after a target
class ChaseCamera : public Camera
{
public:

	void Update(); //Update the matrices and positions of the camera

	Vector3 target; //The position of the target object
	Vector3 targetDirection; //The direction the target is facing

	Vector3 desiredOffset = Vector3(0, 2, 2); //desired offset from the target
	Vector3 DesiredPosition() const { return desiredPosition; } //Return the desired position of the camera, may not be up to date
	Vector3 DesiredPosition() { Update(); return desiredPosition; } //Return the desired position of the camera

	Vector3 lookAtOffset = Vector3(0, 2.8f, 0); //Where to look at relative to the target

protected:
	Vector3 desiredPosition;
};
#pragma once

#include "Engine/Common/Math.hpp"
#include "Engine/Graphics/ConstantBuffer.hpp"

using namespace Math;

//A single camera that can view a scene. Defines a chase camera
class Camera
{
public:
	inline virtual void CalcView() //Update the camera's view matrix (using right-hand coordinates)
	{
		view = Matrix::CreateLookAt(position, lookAt, up).Transpose();
	}
	inline virtual void CalcProjection() //Update the camera's projection matrix (uses orientation)
	{
		projection = Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
		projection = (projection * orientation).Transpose();
	}
	inline virtual void CalcMatrices() //Update all camera's matrices (view, projection)
	{
		CalcView();
		CalcProjection();
	}

	float aspectRatio = 4.f / 3.f; //The aspect ratio of the camera (defaults to 4:3)
	float fieldOfView = 0.7854f; //The field of view of the camera (defaults to 45°)

	float nearPlaneDistance = 1; //When to clip objects close to the camera
	float farPlaneDistance = 100000; //When to clip objects far away from the camera

	Vector3 up = Vector3(0, 1, 0); //The up direction

	Vector3 position = Vector3(); //the current position of the camera
	Vector3 lookAt = Vector3(); //Where the camera is looking at

	inline Matrix View() const { return view; }
	inline Matrix Projection() const { return projection; }
	Matrix orientation = Matrix::Identity; //the orientation of this camera (necessary on devices that change orientation like mobile devices)

protected:
	Matrix view = Matrix::Identity;
	Matrix projection = Matrix::Identity;
};

//The basic camera constant buffer (view and projection matrix)
struct CameraConstantBufferDef : public ConstantBufferDef
{
	Matrix view;
	Matrix projection;

	inline void FillFromCamera(const Camera& Camera)
	{
		view = Camera.View();
		projection = Camera.Projection();
	}
};
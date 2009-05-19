#pragma once
#include "vector.h"

class CCamera
{
public:
	CCamera(void);
	~CCamera(void);

	void normalizeAngle(int *angle);
	void Rotate(int dx, int dy);
	void RotateView(float angle, float x, float y, float z);
	void MoveCamera(float speed);
	void StrafeCamera(float speed);
	float lastRotX;
	float currentRotX;

	CVector3 center;	//Camera's position
	CVector3 up;		//Camera's Up Vector
	CVector3 eye;		//Camera's target
	CVector3 m_vStrafe;
	
	int angleX;
	int angleY;
	int angleZ;
};

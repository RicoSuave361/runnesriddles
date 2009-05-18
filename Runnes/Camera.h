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

	CVector3 center; // posicion de la camara
	CVector3 up; // vector up de la cam
	CVector3 eye; // punto de vision
CVector3 m_vStrafe;
	int angleX;
	int angleY;
	int angleZ;
};

#ifndef _CVECTOR
#define _CVECTOR

#include <math.h>

class CVector3
{
public:
	
	// A default constructor
	CVector3() : x(0),y(0),z(0) {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	CVector3 operator+(const CVector3 &vVector)
	{
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	CVector3 operator-(const CVector3 &vVector)
	{
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the * operator so we can multiply by scalars
	CVector3 operator*(const float &num)
	{
		return CVector3(x * num, y * num, z * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	CVector3 operator/(const float &num)
	{
		// Return the scale vector
		return CVector3(x / num, y / num, z / num);
	}

	static CVector3 Cross(const CVector3 &v1, const CVector3 &v2)
	{
		CVector3 ret;

		ret.x = v1.y * v2.z - v2.y * v1.z;
		ret.y = -v1.x * v2.z + v2.x * v1.z;
		ret.z = v1.x * v2.y - v2.x * v1.y;

		return ret;
	}

	void CVector3::Normalize()
	{
		float distance = (float)sqrt((float)(x*x + y*y+ z*z));
		if (distance==0.0) return;

		x /= distance;
		y /= distance;
		z /= distance;
	}


	float x, y, z;						
};

// This is our basic 2D point class.  This will be used to store the UV coordinates.
class CVector2 
{
public:

	// A default constructor
	CVector2() : x(0),y(0) {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector2(float X, float Y) 
	{ 
		x = X; y = Y;
	}


	CVector2 operator+(CVector2 vVector)
	{
		return CVector2(vVector.x + x, vVector.y + y);
	}

	CVector2 operator-(CVector2 vVector)
	{
		return CVector2(x - vVector.x, y - vVector.y);
	}
	
	CVector2 operator*(float num)
	{
		return CVector2(x * num, y * num);
	}

	CVector2 operator/(float num)
	{
		return CVector2(x / num, y / num);
	}

	float x, y;
};

class CVector4
{
public:
	
	// A default constructor
	CVector4() : x(0),y(0),z(0),w(0) {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector4(float X, float Y, float Z, float W) 
	{ 
		x = X; y = Y; z = Z; w=W;
	}
 
	CVector4 operator+(const CVector4 &vVector)
	{
		return CVector4(vVector.x + x, vVector.y + y, vVector.z + z, vVector.w + w);
	}

	CVector4 operator-(const CVector4 &vVector)
	{
		return CVector4(x - vVector.x, y - vVector.y, z - vVector.z, w - vVector.w);
	}
	
	CVector4 operator*(const float &num)
	{
		return CVector4(x * num, y * num, z * num, w * num);
	}

	CVector4 operator/(const float &num)
	{
		return CVector4(x / num, y / num, z / num, w / num);
	}

	CVector4& operator/=(const float &num)
	{
		// Return the scale vector
		x/=num;
		y/=num;
		z/=num;
		w/=num;

		return *this;
	}

	float x, y, z, w;						
};

#endif
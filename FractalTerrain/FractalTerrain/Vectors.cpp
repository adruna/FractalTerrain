#include "Vector.h"

//Vector2
const Vector2 Vector2::zero = Vector2(0,0);
const Vector2 Vector2::one = Vector2(1,1);
const Vector2 Vector2::up = Vector2(0,1);
const Vector2 Vector2::right = Vector2(1,0);

Vector2::Vector2(const Vector& other)
	: Vector(other)
{ if (dim != 2) throw; }

Vector2::Vector2(float* comp) 
	: Vector(comp)
{ if (dim != 2) throw; }

Vector2::Vector2(float xVal, float yVal)
	: Vector()
{ x = xVal; y = yVal; }

void Vector2::put_X(float xVal) { comp[0] = xVal; }
void Vector2::put_Y(float yVal) { comp[1] = yVal; }
float Vector2::get_X() { return comp[0]; }
float Vector2::get_Y() { return comp[1]; }

//Vector3
const Vector3 Vector3::zero = Vector3(0,0,0);
const Vector3 Vector3::one = Vector3(1,1,1);
const Vector3 Vector3::up = Vector3(0,1,0);
const Vector3 Vector3::right = Vector3(1,0,0);
const Vector3 Vector3::forward = Vector3(0,0,1);

Vector3::Vector3(const Vector& other)
	: Vector(other)
{ if (dim != 3) throw; }

Vector3::Vector3(float* comp) 
	: Vector(comp)
{ if (dim != 3) throw; }

Vector3::Vector3(float xVal, float yVal, float zVal)
	: Vector(3)
{ x = xVal; y = yVal; z = zVal; }

void Vector3::put_X(float xVal) { comp[0] = xVal; }
void Vector3::put_Y(float yVal) { comp[1] = yVal; }
void Vector3::put_Z(float zVal) { comp[2] = zVal; }
float Vector3::get_X() { return comp[0]; }
float Vector3::get_Y() { return comp[1]; }
float Vector3::get_Z() { return comp[2]; }

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
	Vector3 vec;

	vec[0] = a[1] * b[2] - a[2] * b[1];
	vec[1] = a[2] * b[0] - a[0] * b[2];
	vec[2] = a[0] * b[1] - a[1] * b[0];

	return vec;
}

//Vector4

Vector4::Vector4(const Vector& other)
	: Vector(other)
{ if (dim != 4) throw; }

Vector4::Vector4(float* comp) 
	: Vector(comp)
{ if (dim != 4) throw; }

Vector4::Vector4(float xVal, float yVal, float zVal, float wVal)
	: Vector(4)
{ x = xVal; y = yVal; z = zVal; w = wVal; }

void Vector4::put_X(float xVal) { comp[0] = xVal; }
void Vector4::put_Y(float yVal) { comp[1] = yVal; }
void Vector4::put_Z(float zVal) { comp[2] = zVal; }
void Vector4::put_W(float wVal) { comp[3] = wVal; }
float Vector4::get_X() { return comp[0]; }
float Vector4::get_Y() { return comp[1]; }
float Vector4::get_Z() { return comp[2]; }
float Vector4::get_W() { return comp[3]; }
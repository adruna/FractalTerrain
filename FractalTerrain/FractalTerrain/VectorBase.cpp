#include "Vector.h"
#include <math.h>

// statics
float Vector::Dot(const Vector& a, const Vector& b)
{
	CheckDimensions(a, b);

	float result;
	for (int i = 0; i < a.dim; i++)
	{ result = a.comp[i] * b.comp[i]; }
	return result;
}

float Vector::AngleBetween(const Vector& a, const Vector& b)
{ 
	CheckDimensions(a,b);
	return acos(Vector::Dot(a,b) / (a.magnitude * b.magnitude)); 
}

void Vector::CheckDimensions(const Vector& a, const Vector& b)
{ if (a.dimension != b.dimension) throw; }

// properties
float* Vector::get_Components() const { return comp; }
int Vector::get_Dimension() const { return dim; }
float Vector::get_SqrMagnitude() const { return Vector::Dot(*this, *this); }
float Vector::get_Magnitude() const { return sqrt(sqrMagnitude); }

// Constructors
Vector::Vector(int dimensions)
{
	dim = dimensions;
	comp = new float[dim];
}

Vector::Vector(float* components)
{ 
	dim = sizeof(components) / sizeof(float); 
	comp = new float[dim];

	for (int i = 0; i < dim; i++)
	{ comp[i] = components[i]; }
}

Vector::Vector(const Vector& base)
{
	dim = base.dimension;
	comp = new float[dim];

	for (int i = 0; i < dim; i++)
	{ comp[i] = base[i]; }
}

// Destructor
Vector::~Vector() 
{
	if (comp != nullptr)
	{
		delete[] comp;
		comp = nullptr;
	}
}

// operators 
Vector Vector::operator= (const Vector& rhs)
{
	dim = rhs.dim;
	for (int i = 0; i < dim; i++)
	{ comp[i] = rhs[i]; }
	return *this;
}

// casts

Vector::operator float *()
{ return comp; }
Vector::operator float *() const
{ return comp; }

// Math
Vector Vector::operator+ (const Vector& rhs) const
{
	CheckDimensions(*this, rhs);

	Vector vec = Vector(dim);
	for (int i = 0; i < dim; i++)
	{ vec[i] = comp[i] + rhs[i]; }
	return vec;
}

Vector Vector::operator- (const Vector& rhs) const
{
	CheckDimensions(*this, rhs);

	Vector vec = Vector(dim);
	for (int i = 0; i < dim; i++)
	{ vec[i] = comp[i] - rhs[i]; }
	return vec;
}

Vector Vector::operator* (const float rhs) const
{
	Vector vec = Vector(dim);
	for (int i = 0; i < dimension; i++)
	{ vec[i] = comp[i] * rhs; }
	return vec;
}

Vector Vector::operator/ (const float rhs) const
{
	Vector vec = Vector(dim);
	for (int i = 0; i < dim; i++)
	{ vec[i] = comp[i] / rhs; }
	return vec;
}

void Vector::operator*= (float scalar)
{ for (int i = 0; i < dim; i++) { comp[i] *= scalar; } }

void Vector::operator/= (float scalar)
{ for (int i = 0; i < dim; i++) { comp[i] /= scalar; } }

void Vector::operator+= (const Vector rhs)
{
	CheckDimensions(*this, rhs);

	for (int i = 0; i < dim; i++)
	{ comp[i] += rhs[i]; }
}
void Vector::operator-= (const Vector rhs)
{
	CheckDimensions(*this, rhs);

	for (int i = 0; i < dim; i++)
	{ comp[i] -= rhs[i]; }
}

// index
float& Vector::operator[] (int index)
{ return comp[index]; }

float Vector::operator [] (int index) const
{ return comp[index]; }

// compare
bool Vector::operator== (const Vector& rhs) const
{
	CheckDimensions(*this, rhs);

	for (int i = 0; i < dim; i++)
	{ if (comp[i] != rhs[i]) return false; }
	return true;
}
bool Vector::operator!= (const Vector& rhs) const
{
	CheckDimensions(*this, rhs);

	for (int i = 0; i < dimension; i++)
	{ if (comp[i] != rhs[i]) return true; }
	return false;
}
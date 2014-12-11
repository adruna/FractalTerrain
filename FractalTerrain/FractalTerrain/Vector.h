#ifndef VECTOR_H
#define VECTOR_H

/*
Future work:
	Decide whether to scrap or not.
	Get rid of the property thing...
	Seperate object-methods into seperate functions.
*/
class Vector
{
protected:
	float* comp;
	int dim;

public:
	Vector(int = 2);
	Vector(float*);
	Vector(const Vector&);
	~Vector(void);
	
	float get_Magnitude() const;
	float get_SqrMagnitude() const;
	float* get_Components(void) const;
	int get_Dimension(void) const;

	__declspec(property(get = get_Magnitude))float magnitude;
	__declspec(property(get = get_SqrMagnitude))float sqrMagnitude;
	__declspec(property(get = get_Components))float* components;
	__declspec(property(get = get_Dimension))int dimension;

	Vector operator= (const Vector&);
	
	operator float* ();
	operator float* () const;
	
	Vector operator+ (const Vector&) const;
	Vector operator- (const Vector&) const;
	Vector operator* (const float) const;
	Vector operator/ (const float) const;
	void operator*= (const float);
	void operator/= (const float);
	void operator+= (const Vector);
	void operator-= (const Vector);
	float& operator[] (int);
	float operator[] (int) const;

	bool operator== (const Vector&) const;
	bool operator!= (const Vector&) const;

	static float AngleBetween(const Vector&, const Vector&);
	static float Dot(const Vector&, const Vector&);
	static void CheckDimensions(const Vector&, const Vector&);
};

class Vector2 : public Vector
{
public:
	static const Vector2 zero;
	static const Vector2 one;
	static const Vector2 up;
	static const Vector2 right;

	Vector2(const Vector&);
	Vector2(float*);
	Vector2(float = 0, float = 0);

	float get_X();
	void put_X(float);
	float get_Y();
	void put_Y(float);

	__declspec(property(get = get_X, put = put_X))float x;
	__declspec(property(get = get_Y, put = put_Y))float y;
};

class Vector3 : public Vector
{
public:
	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 up;
	static const Vector3 right;
	static const Vector3 forward;

	Vector3(const Vector&);
	Vector3(float*);
	Vector3(float = 0, float = 0, float = 0);

	float get_X();
	void put_X(float);
	float get_Y();
	void put_Y(float);
	float get_Z();
	void put_Z(float);

	__declspec(property(get = get_X, put = put_X))float x;
	__declspec(property(get = get_Y, put = put_Y))float y;
	__declspec(property(get = get_Z, put = put_Z))float z;

	static Vector3 Cross(const Vector3&, const Vector3&);
};

class Vector4 : public Vector
{
public:
	Vector4(const Vector&);
	Vector4(float*);
	Vector4(float = 0, float = 0, float = 0, float = 0);
	
	float get_X();
	void put_X(float);
	float get_Y();
	void put_Y(float);
	float get_Z();
	void put_Z(float);
	float get_W();
	void put_W(float);

	__declspec(property(get = get_X, put = put_X))float x;
	__declspec(property(get = get_Y, put = put_Y))float y;
	__declspec(property(get = get_Z, put = put_Z))float z;
	__declspec(property(get = get_W, put = put_W))float w;
};
#endif
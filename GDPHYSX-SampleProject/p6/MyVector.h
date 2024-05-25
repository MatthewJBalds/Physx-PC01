#pragma once 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace P6 {
	class MyVector {
	public:
		float x, y, z;

		MyVector() : x(0), y(0), z(0) {}
		MyVector(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}

		explicit operator glm::vec3() const { return glm::vec3(x, y, z); }

		float Magnitude() const;

		MyVector Direction() const;

		void operator+=(const MyVector v);

		void operator-=(const MyVector v);

		MyVector operator+(const MyVector v);

		MyVector operator-(const MyVector v);

		MyVector operator*(const MyVector v);

		MyVector operator/(const MyVector v);

		MyVector scalarMultiplication(float scale);

		void operator*=(const MyVector v); 

		float dotProduct(const MyVector v);

		MyVector vectorProduct(const MyVector v);



	};
}
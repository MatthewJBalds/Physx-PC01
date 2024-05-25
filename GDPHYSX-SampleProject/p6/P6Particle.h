#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "MyVector.h"

namespace P6 {
	class P6Particle {
		public:
			float mass = 0;
			MyVector Position;
			MyVector Velocity;
			MyVector Acceleration;

		protected:
			void UpdatePosition(float time);
			void UpdateVelocity(float time);

		public:
			void update(float time);
	};

}



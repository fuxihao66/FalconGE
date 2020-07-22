#pragma once

namespace Falcon {

	typedef  unsigned int UINT;

	namespace Math {
		template<int k, class T>
		struct Point {

		};

		template<int i, int j, class T>
		struct Matrix {

		};

		typedef Point<3, UINT> Point3u;
		typedef Point<3, float> Point3f;
		typedef Point<2, UINT> Point2u;
		typedef Point<2, float> Point2f;
		typedef Point<4, UINT> Point4u;
		typedef Point<4, float> Point4f;
		
		typedef Matrix<4, 4, float> Mat4x4f;
		typedef Matrix<3, 3, float> Mat3x3f;
		typedef Matrix<4, 3, float> Mat4x3f;
		typedef Matrix<3, 3, float> Mat3x3f;
		typedef Matrix<4, 4, UINT> Mat4x4u;
		typedef Matrix<3, 3, UINT> Mat3x3u;
		typedef Matrix<4, 3, UINT> Mat4x3u;
		typedef Matrix<3, 3, UINT> Mat3x3u;
	}
}
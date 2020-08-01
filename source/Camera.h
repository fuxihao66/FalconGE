#pragma once
#include "Utility.h"


namespace Falcon {
	struct Camera {
	private:
		float _far;
		float _near;
		float _fov;
	public:
		Vec3<float> Position;
		Vec3<float> Orientation;
		
		//std::string RenderTarget;
		UINT RenderTargetIndex;

		ViewPort;
		Scissor;

		Camera();
		void Translate(const Vec3<float>&);
		void Rotate(const Vec3<float>&);
		Mat4x4<float> GetViewMat() const;
		Mat4x4<float> GetProjMat() const;
	};
}

#pragma once
#include "Texture.h"

namespace Falcon {
	class RenderTexture : public Texture {
	private:
		Vec2<UINT> _size;
		UINT _bitDepth;
		std::string _id;
	public:
		RenderTexture();
		RenderTexture(const std::string&, UINT w, UINT h, UINT depth);
		std::string GetID() const;

	};
}

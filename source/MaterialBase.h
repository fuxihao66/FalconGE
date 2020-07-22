#pragma once
#include <map>
#include <memory>
#include "RenderCore/Texture.h"
#include "Utility.h"


namespace Falcon {
	class MaterialBase
	{
	protected:
		std::map<std::string, std::shared_ptr<Texture>> _pTexPara;
		std::map<std::string, float> _floatPara;
		std::map<std::string, Vec4<double>> _colorPara;
		std::shared_ptr<Shader> _shader;
		//std::map<std::string, Matrix> _matPara;

	};

}


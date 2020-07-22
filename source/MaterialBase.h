#pragma once
class MaterialBase
{
protected:
	std::map<std::string, std::shared_ptr<Texture2d>> _pMainTex;
	std::map<std::string, float> _floatPara;
	std::shared_ptr<Shader> _shader;
	//std::map<std::string, Matrix> _matPara;

};


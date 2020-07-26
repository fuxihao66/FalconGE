#pragma once
#include <memory>

#include "RenderBackend.h"
#include "Camera.h"
#include "Renderable.h"
#include "MaterialBase.h"
#include "Shader.h"
#include "RenderCore/RenderTexture.h"
#include "RenderCore/AfterEffect.h"
namespace Falcon {
	class Scene
	{
	protected:
		std::map<std::string, std::shared_ptr<Renderable>> _objMap;
		std::vector<std::shared_ptr<AfterEffect>> _aeContainer;

		std::map<std::string, std::shared_ptr<RenderTexture>> _rtMap;
		std::map<std::string, std::shared_ptr<Shader>> _shaderMap;
		std::map<std::string, std::shared_ptr<Texture>> _texMap;
		std::map<std::string, std::shared_ptr<MaterialBase>> _matMap;

		std::vector<std::shared_ptr<Camera>> _camList;
		std::shared_ptr<Camera> _mainCam;

		std::shared_ptr<MaterialBase> LoadEffectFromFile(const std::string&, const std::string& key); // key 用于构建renderable 
		std::shared_ptr<RenderTexture> CreateRenderTexture(int, int, , const std::string& id);
		//bool LoadMeshFromFile(const std::string&);
	public:
		//void Simulate();
		Scene();
		virtual void OnStart();
		virtual void OnRender();
		virtual void OnUpdate();
		virtual void OnDestroy();
		void AddRenderable(std::shared_ptr<Renderable> obj);
		void AddCam();
		// 如果把输出设置为nullptr，则renderBackend会认为是最终需要输出的画面
		void AddAE(std::shared_ptr<MaterialBase> afterEffect, std::shared_ptr<RenderTexture> output);

	};
}



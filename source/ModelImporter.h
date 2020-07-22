#pragma once
#include <memory>

#include "Renderable.h"

namespace Falcon {
	class ModelImporter
	{
	public:
		static std::shared_ptr<Renderable> ImportFromFile();
	};
}



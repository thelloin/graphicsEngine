#pragma once
#include "TextureCache.h"
#include <string>

namespace Tengine {

	class ResourceManager
	{
	public:
		static GLTexture getTexture(std::string texturePath);


	private:
		static TextureCache _textureCache;
	};

}
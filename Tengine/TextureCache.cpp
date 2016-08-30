#include "TextureCache.h"
#include "ImageLoader.h"
#include <iostream>

namespace Tengine {

	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}

	GLTexture TextureCache::getTexture(std::string texturePath) {

		// Lookup the texture and see if its in the map
		auto mit = _textureMap.find(texturePath);

		// check if its in the map
		if (mit == _textureMap.end())
		{
			// Load the texture
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);

			// Insert the texture into the map
			_textureMap.insert(make_pair(texturePath, newTexture));

			//std::cout << "Loaded texture!\n";
			return newTexture;
		}

		//std::cout << "Loaded cached texture!\n";
		return mit->second;
	}

}
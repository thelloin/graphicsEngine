#pragma once

#include "GLTexture.h"
#include <glm/glm.hpp>

namespace Tengine {

	class TileSheet
	{
	public:
		void init(const GLTexture& texture, const glm::ivec3& tileDims);
	
		glm::vec4 getUVS(int index);
	
		GLTexture texture;
		glm::ivec3 dims;
	};

}
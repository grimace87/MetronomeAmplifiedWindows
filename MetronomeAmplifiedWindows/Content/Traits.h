#pragma once

#include "Content/Resources/Shaders.h"
#include "Content/Resources/Textures.h"
#include "Content/Resources/VertexBuffers.h"

class Renderable {
public:
	virtual void Render() = 0;
	virtual void Update(DX::StepTimer const& timer) = 0;
};

class UsesCachedResources {
public:
	virtual std::vector<shader::ClassId> GetRequiredShaders() = 0;
	virtual std::vector<texture::ClassId> GetRequiredSizeIndependentTextures() = 0;
	virtual std::vector<texture::ClassId> GetRequiredSizeDependentTextures() = 0;
	virtual std::vector<vbo::ClassId> GetRequiredSizeIndependentVertexBuffers() = 0;
	virtual std::vector<vbo::ClassId> GetRequiredSizeDependentVertexBuffers() = 0;
};

class Scene : public Renderable, public UsesCachedResources {};

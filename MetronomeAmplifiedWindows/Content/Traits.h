#pragma once

#include "Content/Components/BaseShader.h"
#include "Content/Components/BaseTexture.h"
#include "Content/Components/BaseVertexBuffer.h"

class StackHost;

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

class Scene : public Renderable, public UsesCachedResources {
public:
	virtual void OnPointerPressed(StackHost* stackHost, float normalisedX, float normalisedY) = 0;
};

class StackHost {
public:
	virtual void pushScene(Scene* newScene) = 0;
	virtual void popScene() = 0;
};

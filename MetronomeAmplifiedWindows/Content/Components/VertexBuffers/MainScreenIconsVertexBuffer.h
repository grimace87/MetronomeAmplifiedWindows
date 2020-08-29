#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class MainScreenIconsVertexBuffer : public BaseVertexBuffer {
	public:
		MainScreenIconsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

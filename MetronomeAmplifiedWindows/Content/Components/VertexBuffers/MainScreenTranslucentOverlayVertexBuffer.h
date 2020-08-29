#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class MainScreenTranslucentOverlayVertexBuffer : public BaseVertexBuffer {
	public:
		MainScreenTranslucentOverlayVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

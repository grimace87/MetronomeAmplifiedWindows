#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class SettingsDetailsTranslucentOverlayVertexBuffer : public BaseVertexBuffer {
	public:
		SettingsDetailsTranslucentOverlayVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

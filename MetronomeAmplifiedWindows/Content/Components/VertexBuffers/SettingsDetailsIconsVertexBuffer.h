#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class SettingsDetailsIconsVertexBuffer : public BaseVertexBuffer {
	public:
		SettingsDetailsIconsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

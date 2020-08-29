#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class SettingsNavigatingTextsVertexBuffer : public BaseVertexBuffer {
	public:
		SettingsNavigatingTextsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

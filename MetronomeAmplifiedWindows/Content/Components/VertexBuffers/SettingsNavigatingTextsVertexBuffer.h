#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class SettingsNavigatingTextsVertexBuffer : public BaseVertexBuffer {
	public:
		SettingsNavigatingTextsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

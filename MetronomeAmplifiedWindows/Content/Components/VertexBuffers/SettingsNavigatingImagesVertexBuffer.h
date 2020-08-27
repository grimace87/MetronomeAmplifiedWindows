#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class SettingsNavigatingImagesVertexBuffer : public BaseVertexBuffer {
	public:
		SettingsNavigatingImagesVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

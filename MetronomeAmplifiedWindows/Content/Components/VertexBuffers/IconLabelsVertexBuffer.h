#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class IconLabelsVertexBuffer : public BaseVertexBuffer {
	public:
		IconLabelsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

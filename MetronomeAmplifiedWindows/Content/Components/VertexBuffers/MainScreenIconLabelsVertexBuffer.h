#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class MainScreenIconLabelsVertexBuffer : public BaseVertexBuffer {
	public:
		MainScreenIconLabelsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

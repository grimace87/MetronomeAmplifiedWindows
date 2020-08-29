#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class BackgroundVertexBuffer : public BaseVertexBuffer {
	public:
		BackgroundVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

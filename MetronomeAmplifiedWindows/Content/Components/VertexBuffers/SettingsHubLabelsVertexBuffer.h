#pragma once

#include "../BaseVertexBuffer.h"

namespace vbo
{
	class SettingsHubLabelsVertexBuffer : public BaseVertexBuffer {
	public:
		SettingsHubLabelsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual void Initialise(DX::DeviceResources* resources) override;
	};
}

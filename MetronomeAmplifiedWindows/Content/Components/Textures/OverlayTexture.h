#pragma once
#include "../BaseTexture.h"

namespace texture
{
	class OverlayTexture : public BaseTexture {
	public:
		OverlayTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

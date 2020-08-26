#pragma once
#include "../BaseTexture.h"

namespace texture
{
	class WoodTexture : public BaseTexture {
	public:
		WoodTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

#pragma once
#include "../BaseTexture.h"

namespace texture
{
	class FontTexture : public BaseTexture {
	public:
		FontTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

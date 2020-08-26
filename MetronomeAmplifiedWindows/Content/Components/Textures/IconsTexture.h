#pragma once
#include "../BaseTexture.h"

namespace texture
{
	class IconsTexture : public BaseTexture {
	public:
		IconsTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

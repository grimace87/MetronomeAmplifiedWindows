#pragma once
#include "../BaseTexture.h"

namespace texture
{
	class SampleImageTexture : public BaseTexture {
	public:
		SampleImageTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

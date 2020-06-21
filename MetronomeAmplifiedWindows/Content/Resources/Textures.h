#pragma once

#include <string>

namespace DX {
	class DeviceResources;
}

namespace texture {

	enum class ClassId {
		WOOD_TEXTURE,
		OVERLAY_TEXTURE
	};

	class BaseTexture {
	private:
		Microsoft::WRL::ComPtr<ID3D11Resource>            m_textureResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_textureView;

	protected:
		BaseTexture();

		Concurrency::task<void> MakeTextureFromFileTask(DX::DeviceResources* resources, std::wstring fileName);
		void MakeTextureFromMemory(DX::DeviceResources* resources, std::vector<byte>& pixelData, int width, int height);

	public:
		static BaseTexture* NewFromClassId(ClassId id);
		virtual bool GetIsSizeDependent() = 0;
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) = 0;
		void Activate(ID3D11DeviceContext3* context);
		void Reset();
	};

	class WoodTexture : public BaseTexture {
	public:
		WoodTexture();
		virtual bool GetIsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};

	class OverlayTexture : public BaseTexture {
	public:
		OverlayTexture();
		virtual bool GetIsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

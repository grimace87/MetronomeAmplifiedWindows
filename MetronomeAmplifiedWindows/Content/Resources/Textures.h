#pragma once

#include <string>

namespace DX {
	class DeviceResources;
}

namespace texture {

	enum class ClassId {
		WOOD_TEXTURE,
		OVERLAY_TEXTURE,
		FONT_TEXTURE
	};

	class BaseTexture {
	private:
		Microsoft::WRL::ComPtr<ID3D11Resource>            m_textureResource;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_textureView;

	protected:
		bool m_isValid;

		BaseTexture();
		Concurrency::task<void> MakeTextureFromFileTask(DX::DeviceResources* resources, std::wstring fileName);
		void MakeTextureFromMemory(DX::DeviceResources* resources, std::vector<byte>& pixelData, int width, int height);

	public:
		static BaseTexture* NewFromClassId(ClassId id);
		virtual bool IsSizeDependent() = 0;
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) = 0;
		void Activate(ID3D11DeviceContext3* context);
		void Reset();
		inline bool IsValid() { return m_isValid; }
	};

	class WoodTexture : public BaseTexture {
	public:
		WoodTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};

	class OverlayTexture : public BaseTexture {
	public:
		OverlayTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};

	class FontTexture : public BaseTexture {
	public:
		FontTexture();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}

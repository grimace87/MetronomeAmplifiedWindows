#include "pch.h"
#include "SettingsNavigatingTextsVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::SettingsNavigatingTextsVertexBuffer::SettingsNavigatingTextsVertexBuffer()
{
}

bool vbo::SettingsNavigatingTextsVertexBuffer::IsSizeDependent()
{
	return true;
}

Concurrency::task<void> vbo::SettingsNavigatingTextsVertexBuffer::MakeInitTask(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	return Concurrency::create_task([this, resources]() -> void {

		font::Font* orkney = resources->GetOrkneyFont();
		if (orkney == nullptr) {
			return;
		}

		// Basic margins and the like
		winrt::Windows::Foundation::Size size = resources->GetOutputSize();
		const float marginLogicalInches = 0.25f;
		const float dpi = resources->GetDpi();
		const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
		const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;
		
		const float w1 = -1.0f + marginUnitsW;
		const float w2 = w1 + marginUnitsW;
		const float w4 = 1.0f - marginUnitsW;
		const float w3 = w4 - marginUnitsW;
		const float h1 = -1.0f + 2.0f * marginUnitsH;
		const float h2 = -0.125f - marginUnitsH;
		const float h4 = 1.0f - marginUnitsH;
		const float h3 = h4 - 2.0f * marginUnitsH;

		std::vector<structures::VertexTexCoord> vboData;
		std::vector<std::string> labels = {
			"Navigating the App",
			"The pattern of percussive beats you'll play along with are displayed here. The time signature is shown, along with the timing of each note, in case you're familiar with musical notation. A song consists of one or more of these sections, each with its own note pattern, and therefore can be very simple or very complex.",
			"Playback is controlled with the buttons at the bottom. Fast-forward and rewind have a use only with songs that have multiple sections. Pausing will halt the current position in the current section, while stopping will reset the playback position to the beginning.",
			"A section of a song has a default tempo, although the tempo it is being played at can vary. Tapping the beat-per-minute count will reset to the default tempo, while the slider allows free manual adjustments.",
			"Training modes exist to alter the tempo automatically while you play, or set a limit to how long you'd like to play. Pressing the One-touch Tempo Lift button will begin tempo control, and this feature can be customised through the settings screen.",
			"The timer will allow you to set a timespan before the metronome stops playing.",
			"There is more than one set of sounds that the metronome can play - they can be loaded by pressing the Tone button.",
			"The song and its sections can be fully customised by tapping the Song button.",
			"Various settings are accessible by tapping the Settings button. These settings control the Tempo Lift behaviour, as well as visual cues to coincide with the sounds you here."
		};
		int totalStructCount = 0;
		for (auto& label : labels) {
			totalStructCount += 6 * label.length();
		}
		vboData.resize(totalStructCount);

		int bufferIndex = 0;
		const float headingTextHeightPixels = 1.2f * marginLogicalInches * dpi;
		const float bodyTextHeightPixels = 0.9f * marginLogicalInches * dpi;

		// Put heading
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[0], w1, h4, w4 - w1, h4 - h3, headingTextHeightPixels, size, font::Gravity::START, font::Gravity::CENTER);
		bufferIndex += 6 * labels[0].length();

		// Put content texts
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[1], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[1].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[2], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[2].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[3], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[3].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[4], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[4].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[5], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[5].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[6], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[6].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[7], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[7].length();
		orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[8], w2, h2, w3 - w2, h2 - h1, bodyTextHeightPixels, size, font::Gravity::START, font::Gravity::START);
		bufferIndex += 6 * labels[8].length();

		m_vertexCount = vboData.size();

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBufferData.pSysMem = vboData.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(CD3D11_BUFFER_DESC));
		vertexBufferDesc.ByteWidth = vboData.size() * sizeof(structures::VertexTexCoord);
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		winrt::check_hresult(
			resources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				m_vertexBuffer.put()
			)
		);

		m_isValid = true;
		});
}

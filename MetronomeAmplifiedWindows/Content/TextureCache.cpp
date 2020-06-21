#include "pch.h"
#include "TextureCache.h"

#include "../Common/DeviceResources.h"
#include "../Common/DirectXHelper.h"

cache::TextureCache::TextureCache() : m_textures(), m_sizeIndependentTexturesAreFulfilled(true), m_sizeDependentTexturesAreFulfilled(true), m_samplerAndBlendStateFulfilled(false)
{
}

bool cache::TextureCache::ContainsAll(std::vector<texture::ClassId>& textureClasses)
{
    bool containsAll = true;
    for (auto classId : textureClasses) {
        bool exists = (m_textures.count(classId) == 1);
        if (exists) {
            containsAll &= m_textures[classId]->IsValid();
        }
        else {
            return false;
        }
    }
    return containsAll && m_samplerAndBlendStateFulfilled;
}

void cache::TextureCache::RequireSamplerAndBlendState(DX::DeviceResources* resources)
{
    if (m_samplerAndBlendStateFulfilled) {
        return;
    }

    // Create the linear sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    DX::ThrowIfFailed(
        resources->GetD3DDevice()->CreateSamplerState(
            &samplerDesc,
            &m_samplerStateLinear)
    );

    // Create the point sampler state, as a slight variation of the previous
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    DX::ThrowIfFailed(
        resources->GetD3DDevice()->CreateSamplerState(
            &samplerDesc,
            &m_samplerStatePoint)
    );

    // Create the blend state
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DX::ThrowIfFailed(
        resources->GetD3DDevice()->CreateBlendState(
            &blendDesc,
            &m_blendState)
    );

    // Signal job done
    m_samplerAndBlendStateFulfilled = true;
}

void cache::TextureCache::RequireSizeIndependentTextures(DX::DeviceResources* resources, std::vector<texture::ClassId>& textureClasses)
{
    m_sizeIndependentTexturesAreFulfilled = false;
    Concurrency::task<void> awaitAllTask = Concurrency::create_task([this, resources]() -> void {
        RequireSamplerAndBlendState(resources);
        });

    for (auto classId : textureClasses) {
        texture::BaseTexture* texture;
        if (m_textures.count(classId) == 1) {
            texture = m_textures[classId];
            if (texture->IsValid()) {
                continue;
            }
        } else {
            texture = texture::BaseTexture::NewFromClassId(classId);
        }
        if (texture->IsSizeDependent()) {
            continue;
        }
        awaitAllTask = awaitAllTask && texture->MakeInitTask(resources);
        m_textures[classId] = texture;
    }

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_sizeIndependentTexturesAreFulfilled = true;
        }
        catch (Platform::COMException^ e) {
            OutputDebugString(L"Failed to create a size-independent texture");
            throw e;
        }
        });
}

void cache::TextureCache::RequireSizeDependentTextures(DX::DeviceResources* resources, std::vector<texture::ClassId>& textureClasses)
{
    m_sizeDependentTexturesAreFulfilled = false;
    Concurrency::task<void> awaitAllTask = Concurrency::create_task([this, resources]() -> void {
        RequireSamplerAndBlendState(resources);
        });
    for (auto classId : textureClasses) {
        texture::BaseTexture* texture;
        if (m_textures.count(classId) == 1) {
            texture = m_textures[classId];
            if (texture->IsValid()) {
                continue;
            }
        }
        texture = texture::BaseTexture::NewFromClassId(classId);
        if (!texture->IsSizeDependent()) {
            continue;
        }
        awaitAllTask = awaitAllTask && texture->MakeInitTask(resources);
        m_textures[classId] = texture;
    }

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_sizeDependentTexturesAreFulfilled = true;
        }
        catch (Platform::COMException^ e) {
            OutputDebugString(L"Failed to create a size-dependent texture");
            throw e;
        }
        });
}

texture::BaseTexture* cache::TextureCache::GetTexture(texture::ClassId textureClass)
{
    return m_textures[textureClass];
}

void cache::TextureCache::Clear()
{
    for (auto texture : m_textures) {
        texture.second->Reset();
    }
    m_textures.clear();
    m_samplerStateLinear.Reset();
    m_samplerStatePoint.Reset();
    m_blendState.Reset();
    m_samplerAndBlendStateFulfilled = false;
}

void cache::TextureCache::InvalidateSizeDependentTextures()
{
    for (auto texture : m_textures) {
        if (texture.second->IsSizeDependent()) {
            texture.second->Reset();
        }
    }
}

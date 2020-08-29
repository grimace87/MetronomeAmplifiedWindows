#include "pch.h"
#include "VertexBufferCache.h"

#include "Common/DirectXHelper.h"

cache::VertexBufferCache::VertexBufferCache() : m_vertexBuffers(),
    m_sizeIndependentBuffersAreFulfilled(true),
    m_sizeDependentBuffersAreFulfilled(true),
    m_orkneyFont(nullptr)
{
}

bool cache::VertexBufferCache::ContainsAll(std::vector<vbo::ClassId>& vertexBufferClasses)
{
    bool containsAll = true;
    for (auto classId : vertexBufferClasses) {
        bool exists = (m_vertexBuffers.count(classId) == 1);
        if (exists) {
            containsAll &= m_vertexBuffers[classId]->IsValid();
        }
        else {
            return false;
        }
    }
    return containsAll;
}

void cache::VertexBufferCache::RequireSizeIndependentVertexBuffers(DX::DeviceResources* resources, std::vector<vbo::ClassId>& vertexBufferClasses)
{
    m_sizeIndependentBuffersAreFulfilled = false;

    // Require font object
    Concurrency::task<void> awaitFontTask = m_orkneyFont ?
        Concurrency::create_task([]() -> void {}) :
        DX::ReadDataAsync(L"Assets\\Definitions\\Orkney.fnt").then([this, resources](const std::vector<byte>& fileData) -> void {
            m_orkneyFont = font::Font::MakeFromFileContents(fileData);
            });

    // Await the font object, then load required vertex buffers in sequence
    Concurrency::task<void> awaitAllTask = awaitFontTask.then([this, resources, vertexBufferClasses]() {
        BuildVertexBuffers(resources, vertexBufferClasses);
        });

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_sizeIndependentBuffersAreFulfilled = true;
        }
        catch (const std::exception& e) {
            OutputDebugString(L"Failed to create a size-independent VBO");
            throw e;
        }
        });
}

void cache::VertexBufferCache::RequireSizeDependentVertexBuffers(DX::DeviceResources* resources, std::vector<vbo::ClassId>& vertexBufferClasses)
{
    m_sizeDependentBuffersAreFulfilled = false;

    // Require font object
    Concurrency::task<void> awaitFontTask = m_orkneyFont ?
        Concurrency::create_task([]() -> void {}) :
        DX::ReadDataAsync(L"Assets\\Definitions\\Orkney.fnt").then([this, resources](const std::vector<byte>& fileData) -> void {
        m_orkneyFont = font::Font::MakeFromFileContents(fileData);
            });

    // Await the font object, then load required vertex buffers in sequence
    Concurrency::task<void> awaitAllTask = awaitFontTask.then([this, resources, vertexBufferClasses]() {
        BuildVertexBuffers(resources, vertexBufferClasses);
        });

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_sizeDependentBuffersAreFulfilled = true;
        }
        catch (const std::exception& e) {
            OutputDebugString(L"Failed to create a size-independent VBO");
            throw e;
        }
        });
}

void cache::VertexBufferCache::BuildVertexBuffers(DX::DeviceResources* resources, std::vector<vbo::ClassId> vertexBufferClasses)
{
    for (auto classId : vertexBufferClasses) {
        vbo::BaseVertexBuffer* vertexBuffer;
        if (m_vertexBuffers.count(classId) == 1) {
            vertexBuffer = m_vertexBuffers[classId];
            if (vertexBuffer->IsValid()) {
                continue;
            }
        }
        vertexBuffer = vbo::BaseVertexBuffer::NewFromClassId(classId);
        vertexBuffer->Initialise(resources);
        m_vertexBuffers[classId] = vertexBuffer;
    }
}

vbo::BaseVertexBuffer* cache::VertexBufferCache::GetVertexBuffer(vbo::ClassId vertexBufferClass)
{
    return m_vertexBuffers[vertexBufferClass];
}

void cache::VertexBufferCache::Clear()
{
    for (auto vertexBuffer : m_vertexBuffers) {
        vertexBuffer.second->Reset();
    }
    m_vertexBuffers.clear();
    if (m_orkneyFont) {
        delete m_orkneyFont;
    }
    m_orkneyFont = nullptr;
}

void cache::VertexBufferCache::InvalidateSizeDependentVertexBuffers()
{
    for (auto vertexBuffer : m_vertexBuffers) {
        if (vertexBuffer.second->IsSizeDependent()) {
            vertexBuffer.second->Reset();
        }
    }
}

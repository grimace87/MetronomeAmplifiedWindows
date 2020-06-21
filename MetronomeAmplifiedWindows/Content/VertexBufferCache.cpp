#include "pch.h"
#include "VertexBufferCache.h"

cache::VertexBufferCache::VertexBufferCache() : m_vertexBuffers(), m_sizeIndependentBuffersAreFulfilled(true), m_sizeDependentBuffersAreFulfilled(true)
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
    Concurrency::task<void> awaitAllTask = Concurrency::create_task([]() -> void {});
    for (auto classId : vertexBufferClasses) {
        vbo::BaseVertexBuffer* vertexBuffer;
        if (m_vertexBuffers.count(classId) == 1) {
            vertexBuffer = m_vertexBuffers[classId];
            if (vertexBuffer->IsValid()) {
                continue;
            }
        } else {
            vertexBuffer = vbo::BaseVertexBuffer::NewFromClassId(classId);
        }
        if (vertexBuffer->IsSizeDependent()) {
            continue;
        }
        awaitAllTask = awaitAllTask && vertexBuffer->MakeInitTask(resources);
        m_vertexBuffers[classId] = vertexBuffer;
    }

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_sizeIndependentBuffersAreFulfilled = true;
        }
        catch (Platform::COMException^ e) {
            OutputDebugString(L"Failed to create a size-independent VBO");
            throw e;
        }
        });
}

void cache::VertexBufferCache::RequireSizeDependentVertexBuffers(DX::DeviceResources* resources, std::vector<vbo::ClassId>& vertexBufferClasses)
{
    m_sizeDependentBuffersAreFulfilled = false;
    Concurrency::task<void> awaitAllTask = Concurrency::create_task([]() -> void {});
    for (auto classId : vertexBufferClasses) {
        vbo::BaseVertexBuffer* vertexBuffer;
        if (m_vertexBuffers.count(classId) == 1) {
            vertexBuffer = m_vertexBuffers[classId];
            if (vertexBuffer->IsValid()) {
                continue;
            }
        }
        vertexBuffer = vbo::BaseVertexBuffer::NewFromClassId(classId);
        if (!vertexBuffer->IsSizeDependent()) {
            continue;
        }
        awaitAllTask = awaitAllTask && vertexBuffer->MakeInitTask(resources);
        m_vertexBuffers[classId] = vertexBuffer;
    }

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_sizeDependentBuffersAreFulfilled = true;
        }
        catch (Platform::COMException^ e) {
            OutputDebugString(L"Failed to create a size-dependent VBO");
            throw e;
        }
        });
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
}

void cache::VertexBufferCache::InvalidateSizeDependentVertexBuffers()
{
    for (auto vertexBuffer : m_vertexBuffers) {
        if (vertexBuffer.second->IsSizeDependent()) {
            vertexBuffer.second->Reset();
        }
    }
}

#include "pch.h"
#include "ShaderCache.h"

cache::ShaderCache::ShaderCache() : m_shaders(), m_shadersAreFulfilled(false)
{
}

bool cache::ShaderCache::ContainsAll(std::vector<shader::ClassId>& shaderClasses)
{
    bool containsAll = true;
    for (auto classId : shaderClasses) {
        containsAll &= (m_shaders.count(classId) == 1);
    }
    return containsAll;
}

void cache::ShaderCache::RequireShaders(ID3D11Device3* device, ID3D11DeviceContext3* context, std::vector<shader::ClassId>& shaderClasses)
{
    m_shadersAreFulfilled = false;
    Concurrency::task<void> awaitAllTask = Concurrency::create_task([]() -> void {});
	for (auto classId : shaderClasses) {
        if (m_shaders.count(classId) == 1) {
            continue;
        }
		shader::BaseShader* shader = shader::BaseShader::NewFromClassId(classId);
        awaitAllTask = awaitAllTask && shader->MakeCompileTask(device);
        m_shaders[classId] = shader;
	}

    // Check if exceptions occurred, if not then signal this stuff loaded okay.
    awaitAllTask.then([this](Concurrency::task<void> t) {
        try {
            t.get();
            m_shadersAreFulfilled = true;
        }
        catch (Platform::COMException^ e) {
            OutputDebugString(L"Failed to create a shader");
            throw e;
        }
        });
}

shader::BaseShader* cache::ShaderCache::GetShader(shader::ClassId shaderId)
{
    return m_shaders[shaderId];
}

void cache::ShaderCache::Clear()
{
    for (auto shader : m_shaders) {
        shader.second->Reset();
    }
    m_shaders.clear();
}

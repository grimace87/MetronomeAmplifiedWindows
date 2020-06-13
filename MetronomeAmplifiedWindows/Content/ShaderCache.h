
#include "Resources/Shaders.h"

#include <map>

namespace cache {

	class ShaderCache {
	private:
		std::map<shader::ClassId, shader::BaseShader*> m_shaders;
		bool m_shadersAreFulfilled;

	public:
	    ShaderCache();
	    bool ContainsAll(std::vector<shader::ClassId>& shaderClasses);
		void RequireShaders(ID3D11Device3* device, ID3D11DeviceContext3* context, std::vector<shader::ClassId>& shaderClasses);
		inline bool AreShadersFulfilled() { return m_shadersAreFulfilled; }
		shader::BaseShader* GetShader(shader::ClassId shaderId);
        void Clear();
	};
}

#pragma once

#include "../Content/ShaderStructures.h"

#define FONT_TEXTURE_GLYPH_COUNT 128
#define FONT_TEXTURE_SIZE 512.0f

namespace font {

    class Glyph {
    public:
        float textureS;
        float textureT;
        float offsetX;
        float offsetY;
        float width;
        float height;
        float advanceX;
        Glyph();
    };

    class QuadPT {
        structures::VertexTexCoord vertices[6];
    };

    class Font {
    private:
        Font(float baseHeight, float lineHeight, std::vector<Glyph>&& glyphs);

    public:
        float m_baseHeight;
        float m_lineHeight;
        std::vector<Glyph> m_glyphs;

        Font();
        static Font* MakeFromFileContents(const std::vector<byte>& fileData);
        void PrintIntoVbo(std::vector<structures::VertexTexCoord>& vboData, int startIndex, std::string& textToRender, float left, float top, float boxWidth, float boxHeight, float lines, float scale);
    };
}

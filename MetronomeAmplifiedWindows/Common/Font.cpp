#include "pch.h"
#include "Font.h"

#include <sstream>

font::Glyph::Glyph() {
    textureS = 0.0f;
    textureT = 0.0f;
    offsetX = 0.0f;
    offsetY = 0.0f;
    width = 0.0f;
    height = 0.0f;
    advanceX = 0.0f;
}

font::Font::Font(float baseHeight, float lineHeight, std::vector<Glyph>&& glyphs) :
        m_baseHeight(baseHeight), m_lineHeight(lineHeight), m_glyphs(glyphs) { }

font::Font::Font() :
        m_baseHeight(1.0f), m_lineHeight(1.0f), m_glyphs() { }

font::Font* font::Font::MakeFromFileContents(const std::vector<byte>& fileData) {
    // Create the buffer
    auto glyphSet = std::vector<Glyph>();
    glyphSet.resize(FONT_TEXTURE_GLYPH_COUNT);

    // Create a stream around this resource
    auto inputString = std::string((char*)fileData.data(), fileData.size());
    std::istringstream stream(inputString);

    // Search for "base=XX" and "lineHeight=XX" components before "chars count=XX"
    std::string keyBase = "base";
    std::string keyLineHeight = "lineHeight";
    std::string keyCharCount = "count";
    int valBase = 0, valLineHeight = 0;
    while (!stream.eof()) {
        // Read each component of the format "key=xx"
        std::string item;
        stream >> item;

        // Look for an equals sign ignore items not containing it
        auto equalsPos = item.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        std::string key = item.substr(0, equalsPos);

        // Check for needed items
        if (key == keyBase) {
            valBase = std::stoi(item.substr(equalsPos + 1));
        } else if (key == keyLineHeight) {
            valLineHeight = std::stoi(item.substr(equalsPos + 1));
        } else if (key == keyCharCount) {
            break;
        }
    }

    // Parse each character line
    std::string keyNewChar = "char";
    std::string keyId = "id";
    std::string keyX = "x";
    std::string keyY = "y";
    std::string keyWidth = "width";
    std::string keyHeight = "height";
    std::string keyOffsetX = "xoffset";
    std::string keyOffsetY = "yoffset";
    std::string keyAdvance = "xadvance";
    int valId = -1;
    int valTextureS = 0, valTextureT = 0, valWidth = 0, valHeight = 0, valOffsetX = 0, valOffsetY = 0, valAdvance = 0;
    while (!stream.eof()) {
        // Read each component of the format "key=xx"
        std::string item;
        stream >> item;

        // Save where a new "char" identifier is encountered
        if (item == keyNewChar) {
            if (valId >= 0 && valId < FONT_TEXTURE_GLYPH_COUNT) {
                Glyph glyph;
                glyph.textureS = (float)valTextureS;
                glyph.textureT = (float)valTextureT;
                glyph.width = (float)valWidth;
                glyph.height = (float)valHeight;
                glyph.offsetX = (float)valOffsetX;
                glyph.offsetY = (float)valOffsetY;
                glyph.advanceX = (float)valAdvance;
                glyphSet[valId] = glyph;
            }
            continue;
        }

        // Read the value of any other item
        auto equalsPos = item.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        std::string key = item.substr(0, equalsPos);
        int value = std::stoi(item.substr(equalsPos + 1));
        if (key == keyId) {
            valId = value;
        } else if (key == keyX) {
            valTextureS = value;
        } else if (key == keyY) {
            valTextureT = value;
        } else if (key == keyWidth) {
            valWidth = value;
        } else if (key == keyHeight) {
            valHeight = value;
        } else if (key == keyOffsetX) {
            valOffsetX = value;
        } else if (key == keyOffsetY) {
            valOffsetY = value;
        } else if (key == keyAdvance) {
            valAdvance = value;
        }
    }

    // Return set
    return new Font((float)valBase, (float)valLineHeight, std::move(glyphSet));
}

std::vector<structures::VertexTexCoord> font::Font::GenerateTextVbo(std::string& textToRender, float left, float top, float boxWidth, float boxHeight, float lines, float scale)
{
    // Assign buffer, with 6 vertices per character and 5 or 8 floats per vertex
    const size_t floatsPerVertex = 6U;
    size_t vertexCount = textToRender.length() * floatsPerVertex;
    std::vector<structures::VertexTexCoord> vertexData;
    vertexData.resize(vertexCount);

    // Find scaling factor
    const float lineHeightUnits = boxHeight / lines;
    const float unitsPerPixel = scale * lineHeightUnits / m_lineHeight;

    // Start building the buffer
    int charsRendered = 0;
    float penX = left;
    float penY = top - (float)m_baseHeight * unitsPerPixel;
    float xMin, xMax, yMin, yMax;
    float sMin, sMax, tMin, tMax;
    structures::VertexTexCoord quad[6];
    for (char c : textToRender) {
        Glyph& glyph = m_glyphs.at(c);

        xMin = penX + (float)glyph.offsetX * unitsPerPixel;
        xMax = xMin + (float)glyph.width * unitsPerPixel;
        yMax = penY + (float)(m_baseHeight - glyph.offsetY) * unitsPerPixel;
        yMin = yMax - (float)glyph.height * unitsPerPixel;

        sMin = glyph.textureS / FONT_TEXTURE_SIZE;
        sMax = sMin + glyph.width / FONT_TEXTURE_SIZE;
        tMin = glyph.textureT / FONT_TEXTURE_SIZE;
        tMax = tMin + glyph.height / FONT_TEXTURE_SIZE;

        quad[0].pos = { xMin, yMax, 0.0f };
        quad[0].tex = { sMin, tMin, 0.0f };

        quad[1].pos = { xMax, yMax, 0.0f };
        quad[1].tex = { sMax, tMin, 0.0f };

        quad[2].pos = { xMax, yMin, 0.0f };
        quad[2].tex = { sMax, tMax, 0.0f };

        quad[3].pos = { xMax, yMin, 0.0f };
        quad[3].tex = { sMax, tMax, 0.0f };

        quad[4].pos = { xMin, yMin, 0.0f };
        quad[4].tex = { sMin, tMax, 0.0f };

        quad[5].pos = { xMin, yMax, 0.0f };
        quad[5].tex = { sMin, tMin, 0.0f };

        structures::VertexTexCoord* copyDest = vertexData.data() + charsRendered * 6;
        memcpy((void*)copyDest, (void*)&quad, 6 * sizeof(structures::VertexTexCoord));

        penX += (float)glyph.advanceX * unitsPerPixel;
        if ((penX + (float)m_lineHeight * unitsPerPixel) > boxWidth) {
            penX = left;
            penY -= (float)m_lineHeight * unitsPerPixel;
        }
        charsRendered++;

    }

    return vertexData;
}

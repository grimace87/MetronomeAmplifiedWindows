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

/// <summary>
/// Generate VBO data to render supplied text, writing into the provided vector of VertexTexCoord structs.
/// Required space in the vector, starting at the startIndex offset, is 6 VertexTexCoord structs per character.
/// </summary>
void font::Font::PrintTextIntoVbo(
    std::vector<structures::VertexTexCoord>& vboData,
    int startIndex,
    std::string& textToRender,
    float left,
    float top,
    float boxWidth,
    float boxHeight,
    float maxHeightPixels,
    winrt::Windows::Foundation::Size size,
    Gravity horizontalGravity,
    Gravity verticalGravity)
{
    // Assign buffer, with 6 vertices per character and 5 or 8 floats per vertex
    const size_t floatsPerVertex = 6U;

    // Find scaling factors
    const float pixelsPerUnitWidth = size.Width / 2.0f;
    const float pixelsPerUnitHeight = size.Height / 2.0f;

    // Convert target area to pixel sizes and coordinates
    const float targetWidthPixels = pixelsPerUnitWidth * boxWidth;
    const float targetHeightPixels = pixelsPerUnitHeight * boxHeight;
    const float lineHeightPixels = min(targetHeightPixels, maxHeightPixels);
    const float screenPixelsPerFontPixel = lineHeightPixels / m_lineHeight;

    // Do an initial pass to determine how many lines need to be rendered, and how many
    // characters will be on each of those lines
    std::vector<int> charactersPerLine;
    std::vector<float> pixelWidthOfLine;
    float pixelsAcrossThisLine = 0.0f;
    int currentWordBegunAt = 0;
    float pixelsIntoThisWord = 0.0f;
    int charsForThisLine = 0;
    for (int index = 0; index < textToRender.length(); index++) {
        const char c = textToRender[index];
        Glyph& glyph = m_glyphs.at(c);
        const float advance = glyph.advanceX * screenPixelsPerFontPixel;
        pixelsAcrossThisLine += advance;
        pixelsIntoThisWord += advance;
        charsForThisLine++;
        if (c == ' ') {
            currentWordBegunAt = index + 1;
            pixelsIntoThisWord = 0.0f;
        } else if (pixelsAcrossThisLine > targetWidthPixels) {
            if (index - currentWordBegunAt + 1 == charsForThisLine) {
                charactersPerLine.push_back(index - currentWordBegunAt);
                currentWordBegunAt = index;
                charsForThisLine = 1;
                pixelWidthOfLine.push_back(pixelsAcrossThisLine - advance);
                pixelsAcrossThisLine = advance;
                pixelsIntoThisWord = advance;
            }
            else {
                const int charactersForNextLine = index + 1 - currentWordBegunAt;
                charactersPerLine.push_back(charsForThisLine - charactersForNextLine);
                charsForThisLine = charactersForNextLine;
                pixelWidthOfLine.push_back(pixelsAcrossThisLine - pixelsIntoThisWord);
                pixelsAcrossThisLine = pixelsIntoThisWord;
            }
        }
    }
    if (charsForThisLine > 0) {
        charactersPerLine.push_back(charsForThisLine);
        pixelWidthOfLine.push_back(pixelsAcrossThisLine);
    }

    // Set side margin, horizontal margin depends on supplied gravity
    const float totalTextHeightPixels = (float)charactersPerLine.size() * lineHeightPixels;
    float marginYPixels;
    switch (verticalGravity) {
    case Gravity::START:
        marginYPixels = targetHeightPixels - totalTextHeightPixels;
        break;
    case Gravity::END:
        marginYPixels = 0.0f;
        break;
    default:
        marginYPixels = 0.5f * (targetHeightPixels - totalTextHeightPixels);
    }

    // Start building the buffer
    int charsRendered = 0;
    const float widthUnitsPerFontPixel = screenPixelsPerFontPixel / pixelsPerUnitWidth;
    const float heightUnitsPerFontPixel = screenPixelsPerFontPixel / pixelsPerUnitHeight;
    float penY = top - boxHeight + marginYPixels / pixelsPerUnitHeight + (float)(charactersPerLine.size() - 1) * lineHeightPixels / pixelsPerUnitHeight;
    int textIndex = 0;
    for (int index = 0; index < charactersPerLine.size(); index++) {
        int charsOnLine = charactersPerLine[index];
        const float lineWidthPixels = pixelWidthOfLine[index];
        float marginXPixels;
        switch (horizontalGravity) {
        case Gravity::START:
            marginXPixels = 0.0f;
            break;
        case Gravity::END:
            marginXPixels = targetWidthPixels - lineWidthPixels;
            break;
        default:
            marginXPixels = 0.5f * (targetWidthPixels - lineWidthPixels);
        }
        float penX = left + marginXPixels / pixelsPerUnitWidth;
        structures::VertexTexCoord quad[6];
        for (int i = 0; i < charsOnLine; i++) {
            const char c = textToRender[textIndex];
            textIndex++;
            Glyph & glyph = m_glyphs[c];

            const float xMin = penX + glyph.offsetX * widthUnitsPerFontPixel;
            const float xMax = xMin + glyph.width * widthUnitsPerFontPixel;
            const float yMax = penY + (m_baseHeight - glyph.offsetY) * heightUnitsPerFontPixel;
            const float yMin = yMax - glyph.height * heightUnitsPerFontPixel;

            const float sMin = glyph.textureS / FONT_TEXTURE_SIZE;
            const float sMax = sMin + glyph.width / FONT_TEXTURE_SIZE;
            const float tMin = glyph.textureT / FONT_TEXTURE_SIZE;
            const float tMax = tMin + glyph.height / FONT_TEXTURE_SIZE;

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

            structures::VertexTexCoord* copyDest = vboData.data() + startIndex + charsRendered * 6;
            memcpy((void*)copyDest, (void*)&quad, 6 * sizeof(structures::VertexTexCoord));

            penX += (float)glyph.advanceX * widthUnitsPerFontPixel;
            charsRendered++;
        }
        penY -= lineHeightPixels / pixelsPerUnitHeight;
    }
}

#include "engine.h"

#include <GL/glew.h>
#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>

//#include "lib/parson.h"

#define MAX_DEBUG_LINES 1024

// TODO: get rid of malloc

void setupDebug(DebugState* dstate)
{
    dstate->lines = 0;
    dstate->lineMem = malloc(Megabytes(5));

    glGenBuffers(1, &dstate->lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, dstate->lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_LINES*sizeof(Vec4), dstate->lineMem, GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &dstate->lineVAO);
    glBindVertexArray(dstate->lineVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawLine(DebugState* dstate, Vec3 start, Vec3 end)
{
    Vec4* lineData = (Vec4*)dstate->lineMem;
    lineData[dstate->lines*2] = vec4FromVec3AndW(start,1.0f);
    lineData[dstate->lines*2+1] = vec4FromVec3AndW(end,1.0f);
    dstate->lines++;
}

void drawDebugLines(DebugState* dstate, Shader lineShader, Camera* cam)
{
    glUseProgram(lineShader.program);
    glUniformMatrix4fv(lineShader.surface.transformMatrixUnif, 1, GL_FALSE, (GLfloat*)&cam->transformMatrix);
    glUniformMatrix4fv(lineShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (GLfloat*)&cam->perspectiveMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, dstate->lineBuffer);
    glBindVertexArray(dstate->lineVAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dstate->lines*2*sizeof(Vec4), dstate->lineMem);

    glEnableVertexAttribArray(0); //?
    // TODO: why the fuck this cause some objects to not render??
    glDrawArrays(GL_LINES, 0, dstate->lines*2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    dstate->lines= 0;
    //printf("where the fuck are my lines? \n");
}

// TODO: remove thse globals
//std::map<GLchar, Character> Characters;
GLuint VAO, VBO;
void InitText()
{
    /*FT_Library ft;
    if (FT_Init_FreeType(&ft))
        printf("ERROR::FREETYPE: Could not load freetype\n");
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
        printf("ERROR::FREETYPE: Failed to load font\n");

    FT_Set_Pixel_Sizes(face, 0, 48);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        printf("ERROR::FREETYTPE: Failed to load Glyph\n");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("ERROR::FREETYTPE: Failed to load Glyph\n");
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);*/
}

void RenderText(char* text, GLfloat x, GLfloat y, GLfloat scale)
{
    /*glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);*/
}

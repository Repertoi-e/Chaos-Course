#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glad/glad.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "shader.h"

[[nodiscard("Leak")]] char *read_entire_file(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to read file: %s\n", filePath);
        return null;
    }
    defer(fclose(file));

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto *content = (char *)malloc(fileSize + 1);
    size_t bytesRead = fread(content, 1, fileSize, file);
    if (bytesRead != fileSize)
    {
        fprintf(stderr, "Failed to read file: %s\n", filePath);
        free(content);
        return null;
    }

    content[fileSize] = '\0';
    return content;
}

void free_shader_segment(shader_segment *s) { free(s->Code); }

// Returns array of shader segments. Looks in filePath and splits the contents,
// looking for #vertex, #fragment, #compute special lines which separate the shaders.
// If no such line is found, then type of the returned segment is SHADER_UNKNOWN.
// Probably the caller knows what to do with it.
shader_segment *read_shader_file(const char *filePath)
{
    char *contents = read_entire_file(filePath); // leak
    if (!contents)
        return null;

    size_t contentsLen = strlen(contents);

    char *codeStart = contents;
    char *p = contents;

    shader_segment_type currentSegmentType = SHADER_UNKNOWN;
    shader_segment *segments = null;
    auto addSegment = [&]()
    {
        shader_segment segment;

        size_t n = p - codeStart;
        segment.Code = (char *)malloc(n + 1);
        strncpy(segment.Code, codeStart, n);
        segment.Code[n] = 0;

        segment.Type = currentSegmentType;
        arrput(segments, segment);
    };

    while (p < contents + contentsLen)
    {
        if (*p == '#' && *(p + 1) != '\n')
        {
            const char *headerMap[SHADER_COUNT] = {null, "#vertex", "#fragment", "#compute"};
            for (int i = 1; i < SHADER_COUNT; i++)
            {
                const char *header = headerMap[i];
                if (!header)
                    continue;

                if (strncmp(p, header, strlen(header)) == 0)
                {
                    if (currentSegmentType != SHADER_UNKNOWN)
                    {
                        addSegment();
                    }

                    currentSegmentType = (shader_segment_type)i;
                    codeStart = p + strlen(header);
                    p = codeStart;
                    break;
                }
            }
        }
        ++p;
    }

    if (currentSegmentType != SHADER_UNKNOWN)
    {
        addSegment();
    }

    return segments;
}

GLuint create_shader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        fprintf(stderr, "Failed to compile shader: %s\n", info);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

[[nodiscard]] uint create_shader(shader_segment *segments)
{
    if (!arrlen(segments))
        return 0;

    GLuint *shaders = null;

    for (int i = 0; i < arrlen(segments); i++)
    {
        shader_segment *s = segments + i;
        if (s->Type == SHADER_UNKNOWN)
            continue;

        GLenum map[SHADER_COUNT] = {0, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER};

        GLuint shader = create_shader(map[s->Type], s->Code);
        if (shader)
        {
            arrput(shaders, shader);
        }
        else
        {
            for (int i = 0; i < arrlen(shaders); i++)
            {
                glDeleteShader(shaders[i]);
            }
            return 0;
        }
    }

    GLuint program = glCreateProgram();
    for (int i = 0; i < arrlen(shaders); i++)
    {
        glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar info[512];
        glGetProgramInfoLog(program, 512, nullptr, info);
        fprintf(stderr, "Failed to link shader program: %s\n", info);
        for (int i = 0; i < arrlen(shaders); i++)
        {
            glDeleteShader(shaders[i]);
        }
        glDeleteProgram(program);
        return 0;
    }

    for (int i = 0; i < arrlen(shaders); i++)
    {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }

    return program;
}

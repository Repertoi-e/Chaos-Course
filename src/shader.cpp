#include "shader.h"

#include <glad/glad.h>
#include <lstd/lstd.h>

void free(shader_segment ref s) { free(s.Code); }

// Returns array of shader segments. Looks in filePath and splits the contents,
// looking for #vertex, #fragment, #compute special lines which separate the
// shaders. If no such line is found, then type of the returned segment is
// SHADER_UNKNOWN. Probably the caller knows what to do with it.
array<shader_segment> read_shader_file(string filePath) {
  array<shader_segment> segments;

  auto file = os_read_entire_file(filePath);
  file.visit(match{[ref](string content) {
                     reserve(segments);

                     char *codeStart = content.Data;
                     char *p = content.Data;

                     shader_segment_type currentSegmentType = SHADER_UNKNOWN;
                     auto addSegment = [ref]() {
                       shader_segment segment;

                       size_t n = p - codeStart;
                       segment.Code = string(codeStart, n);
                       segment.Type = currentSegmentType;

                       add(segments, segment);
                     };

                     while (p < content.Data + content.Count) {
                       if (*p == '#' && *(p + 1) != '\n') {
                         string headerMap[SHADER_COUNT] = {
                             "", "#vertex", "#fragment", "#compute"};
                         for (int i = 1; i < SHADER_COUNT; i++) {
                           string header = headerMap[i];
                           if (!header.Count)
                             continue;

                           if (strings_match(header, string(p, header.Count))) {
                             if (currentSegmentType != SHADER_UNKNOWN) {
                               addSegment();
                             }

                             currentSegmentType = (shader_segment_type)i;
                             codeStart = p + header.Count;
                             p = codeStart;
                             break;
                           }
                         }
                       }
                       ++p;
                     }

                     if (currentSegmentType != SHADER_UNKNOWN) {
                       addSegment();
                     }
                   },
                   [](auto) { return; }});
  return segments;
}

GLuint create_shader(GLenum type, string source) {
  char *sourceTemp = to_c_string_temp(source);

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, ref sourceTemp, null);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, ref success);
  if (!success) {
    GLchar info[512];
    glGetShaderInfoLog(shader, 512, nullptr, info);
    fprintf(stderr, "Failed to compile shader: %s\n", info);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

[[nodiscard]] u32 create_shader(array<shader_segment> segments) {
  if (!segments.Count)
    return 0;

  array<GLuint> shaders;
  reserve(shaders);
  defer(free(shaders));

  For(segments) {
    if (it.Type == SHADER_UNKNOWN)
      continue;
    GLenum map[SHADER_COUNT] = {0, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,
                                GL_COMPUTE_SHADER};
    GLuint shader = create_shader(map[it.Type], it.Code);
    if (shader) {
      add(shaders, shader);
    } else {
      For(shaders) { glDeleteShader(it); }
      return 0;
    }
  }

  GLuint program = glCreateProgram();
  For(shaders) { glAttachShader(program, it); }
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, ref success);
  if (!success) {
    GLchar info[512];
    glGetProgramInfoLog(program, 512, nullptr, info);
    fprintf(stderr, "Failed to link shader program: %s\n", info);
    For(shaders) { glDeleteShader(it); }
    glDeleteProgram(program);
    return 0;
  }

  For(shaders) {
    glDetachShader(program, it);
    glDeleteShader(it);
  }

  return program;
}

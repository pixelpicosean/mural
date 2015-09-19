#ifndef Mural_MuCanvasShaders_h
#define Mural_MuCanvasShaders_h

namespace mural {

  const char * const MuShaderVertex = " \
    #version 330 core                   \
    in vec2 pos;                        \
    in vec2 uv;                         \
    in vec4 color;                      \
    \
    out lowp vec4 vColor;               \
    out highp vec2 vUv;                 \
    \
    uniform highp vec2 screen;          \
    \
    void main() {                       \
      vColor = color;                   \
      vUv = uv;                         \
    \
      gl_Position = vec4(pos * (vec2(2,2)/screen) - clamp(screen,-1.0,1.0), 0.0, 1.0); \
    }                                   \
    ";

  const char * const MuShaderAlphaTexture = " \
    #version 330 core                         \
    in lowp vec4 vColor;                      \
    in highp vec2 vUv;                        \
    \
    out vec4 color;                           \
    \
    uniform sampler2D image;                  \
    \
    void main() {                             \
      color = texture(image, vUv).aaaa * vColor; \
    }                                         \
    ";

  const char * const MuShaderFlat = " \
    #version 330 core                 \
    in lowp vec4 vColor;              \
    in highp vec2 vUv;                \
    \
    out vec4 color;                   \
    \
    void main() {                     \
      color = vColor;                 \
    }                                 \
    ";

  const char * const MuShaderPattern = "  \
    #version 330 core                     \
    in lowp vec4 vColor;                  \
    in highp vec2 vUv;                    \
    \
    out vec4 color;                       \
    \
    uniform sampler2D image;              \
    \
    void main() {                         \
      color = texture(image, mod(vUv, vec2(1.0, 1.0)) ) * vColor; \
    }                                     \
    ";

  const char * const MuShaderTexture = "  \
    #version 330 core                     \
    in lowp vec4 vColor;                  \
    in highp vec2 vUv;                    \
    \
    out vec4 color;                       \
    \
    uniform sampler2D image;              \
    \
    void main() {                         \
      color = texture(image, vUv) * vColor; \
    }                                     \
    ";

  const char * const MuShaderRadialGradient = " \
    #version 330 core                           \
    precision highp float;                      \
    \
    in highp vec2 vUv;                          \
    in lowp vec4 vColor;                        \
    \
    out vec4 color;                             \
    \
    uniform mediump vec3 inner; // x, y, z=radius \
    uniform mediump vec3 diff; // x, y, z=radius  \
    \
    uniform sampler2D image;                    \
    \
    void main() {                               \
      vec2 p2 = vUv - inner.xy;                 \
    \
      float A = dot(diff.xy, diff.xy) - diff.z * diff.z;  \
      float B = dot(p2.xy, diff.xy) + inner.z * diff.z;   \
      float C = dot(p2, p2) - (inner.z * inner.z);        \
      float D = (B * B) - (A * C);                        \
    \
      float DA = sqrt(D) / A;                   \
      float BA = B / A;                         \
    \
      float t = max(BA+DA, BA-DA);              \
    \
      lowp float keep = sign(diff.z * t + inner.z); // discard if < 0.0 \
      color = texture(image, vec2(t, 0.0)) * vColor * keep; \
    }                                           \
    ";

}

#endif

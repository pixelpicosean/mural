#ifndef Mural_MuCanvasShaders_h
#define Mural_MuCanvasShaders_h

#define Mu_SHADER_SOURCE(NAME, ...) const char * const NAME = #__VA_ARGS__;

namespace mural {

  Mu_SHADER_SOURCE(MuShaderVertex,
    attribute vec2 pos;
    attribute vec2 uv;
    attribute vec4 color;

    varying lowp vec4 vColor;
    varying highp vec2 vUv;

    uniform highp vec2 screen;

    void main() {
      vColor = color;
      vUv = uv;

      gl_Position = vec4(pos * (vec2(2,2)/screen) - clamp(screen,-1.0,1.0), 0.0, 1.0);
    }
  );

  Mu_SHADER_SOURCE(MuShaderAlphaTexture,
    varying lowp vec4 vColor;
    varying highp vec2 vUv;

    uniform sampler2D texture;

    void main() {
      gl_FragColor = texture2D(texture, vUv).aaaa * vColor;
    }
  );

  Mu_SHADER_SOURCE(MuShaderFlat,
    varying lowp vec4 vColor;
    varying highp vec2 vUv;

    void main() {
      gl_FragColor = vColor;
    }
  );

  Mu_SHADER_SOURCE(MuShaderPattern,
    varying lowp vec4 vColor;
    varying highp vec2 vUv;

    uniform sampler2D texture;

    void main() {
      gl_FragColor = texture2D(texture, mod(vUv, vec2(1.0, 1.0)) ) * vColor;
    }
  );

  Mu_SHADER_SOURCE(MuShaderTexture,
    varying lowp vec4 vColor;
    varying highp vec2 vUv;

    uniform sampler2D texture;

    void main() {
      gl_FragColor = texture2D(texture, vUv) * vColor;
    }
  );

  Mu_SHADER_SOURCE(MuShaderRadialGradient,
    precision highp float;

    varying highp vec2 vUv;
    varying lowp vec4 vColor;

    uniform mediump vec3 inner; // x, y, z=radius
    uniform mediump vec3 diff; // x, y, z=radius

    uniform sampler2D texture;

    void main() {
      vec2 p2 = vUv - inner.xy;

      float A = dot(diff.xy, diff.xy) - diff.z * diff.z;
      float B = dot(p2.xy, diff.xy) + inner.z * diff.z;
      float C = dot(p2, p2) - (inner.z * inner.z);
      float D = (B * B) - (A * C);

      float DA = sqrt(D) / A;
      float BA = B / A;

      float t = max(BA+DA, BA-DA);

      lowp float keep = sign(diff.z * t + inner.z); // discard if < 0.0
      gl_FragColor = texture2D(texture, vec2(t, 0.0)) * vColor * keep;
    }
  );

}

#endif

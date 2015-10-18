//
//  MuCanvas2DTypes.hpp
//  ciEjecta
//
//  Created by Sean on 10/3/15.
//
//

#pragma once

namespace mural {

  using ci::vec2;

  struct MuAffineTransform {
    float a  = 1.0f, b  = 0.0f;
    float c  = 0.0f, d  = 1.0f;
    float tx = 0.0f, ty = 0.0f;

    MuAffineTransform(float a = 1.0f, float b = 0.0f, float c = 0.0f, float d = 1.0f, float tx = 0.0f, float ty = 0.0f):
      a(a), b(b),
      c(c), d(d),
      tx(tx), ty(ty)
    {}

    void translate(float x, float y) {
      tx = tx + a * x + c * y;
      ty = ty + b * x + d * y;
    }

    void rotate(float angle) {
      float fSin = sin(angle);
      float fCos = cos(angle);

      float res_a = a * fCos + c * fSin;
      float res_b = b * fCos + d * fSin;
      float res_c = c * fCos - a * fSin;
      float res_d = d * fCos - b * fSin;

      a = res_a;
      b = res_b;
      c = res_c;
      d = res_d;
    }

    void scale(float sx, float sy) {
      a *= sx;
      b *= sx;
      c *= sy;
      d *= sy;
    }

    void concat(const MuAffineTransform &t) {
      float res_a = a * t.a + b * t.c;
      float res_b = a * t.b + b * t.d;
      float res_c = c * t.a + d * t.c;
      float res_d = c * t.b + d * t.d;
      float res_tx = tx * t.a + ty * t.c + t.tx;
      float res_ty = tx * t.b + ty * t.d + t.ty;

      a = res_a;
      b = res_b;
      c = res_c;
      d = res_d;
      tx = res_tx;
      ty = res_ty;
    }

    void invert() {
      float determinant = 1.0f / (a * d - b * c);

      float res_a = determinant * d;
      float res_b = -determinant * b;
      float res_c = -determinant * c;
      float res_d = determinant * a;
      float res_tx = determinant * (c * ty - d * tx);
      float res_ty = determinant * (b * tx - a * ty);

      a = res_a;
      b = res_b;
      c = res_c;
      d = res_d;
      tx = res_tx;
      ty = res_ty;
    }

    float getScale() const {
      return sqrtf(a * a + c * c);
    }

    bool isIdentity() const {
      // FIXME: float compare available in C++?
      return (a == 1.0f && b == 0.0f && c == 0.0f && d == 1.0f && tx == 0.0f && ty == 0.0f);
    }

    vec2 &applyTo(vec2 &p) const {
      float res_x = a * p.x + c * p.y + tx;
      float res_y = b * p.x + d * p.y + ty;

      p.x = res_x;
      p.y = res_y;

      return p;
    }

    static MuAffineTransform identity;
  };

  static inline float distanceToLineSegmentSquared(const vec2 &p, const vec2 &v, const vec2 &w) {
    float l2 = glm::distance2(v, w);
    if (l2 == 0) return glm::distance2(p, v);
    float t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    if (t < 0) return glm::distance2(p, v);
    if (t > 1) return glm::distance2(p, w);
    return glm::distance2(p, vec2(v.x + t * (w.x - v.x), v.y + t * (w.y - v.y)));
  }

  static inline float distanceToLineSegment(const vec2 &p, const vec2 &v, const vec2 &w) {
    return sqrtf(distanceToLineSegmentSquared(p, v, w));
  }

}

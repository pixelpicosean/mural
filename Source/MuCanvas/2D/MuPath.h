#ifndef Mural_MuPath_h
#define Mural_MuPath_h

#include "MuOpenGL.h"
#include "MuCanvas2DTypes.h"

#include <vector>

#define MU_PATH_RECURSION_LIMIT 8
#define MU_PATH_DISTANCE_EPSILON 1.0f
#define MU_PATH_COLLINEARITY_EPSILON FLT_EPSILON
#define MU_PATH_MIN_STEPS_FOR_CIRCLE 20.0f
#define MU_PATH_MAX_STEPS_FOR_CIRCLE 64.0f

namespace mural {

  enum MuPathPolygonTarget {
    kMuPathPolygonTargetColor,
    kMuPathPolygonTargetDepth
  };

  enum MuPathFillRule {
    kMuPathFillRuleNonZero,
    kMuPathFillRuleEvenOdd
  };

  // We're using the C++ std::vector here to store our points.
  typedef std::vector<glm::vec2> points_t;
  typedef struct {
    points_t points;
    bool isClosed;
  } subpath_t;
  typedef std::vector<subpath_t> path_t;

  class MuCanvasContext2D;
  class MuPath {
    public:
      glm::mat3 transform;
      MuPathFillRule fillRule;

      MuPath();
      ~MuPath() {}

      void push(glm::vec2 v);
      void reset();
      void close();
      void endSubPath();
      void moveTo(float x, float y);
      void lineTo(float x, float y);
      void bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y, float scale);
      void recursiveBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level);
      void quadraticCurveTo(float cpx, float cpy, float x, float y, float scale);
      void recursiveQuadratic(float x1, float y1, float x2, float y2, float x3, float y3, int level);
      void arcTo(float x1, float y1, float x2, float y2, float radius);
      void arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise);

      void drawPolygonsToContext(MuCanvasContext2D *context, MuPathFillRule fillRule, MuPathPolygonTarget target);
      void drawLinesToContext(MuCanvasContext2D *context);

    private:
      glm::vec2 currentPos, lastPushed;
      glm::vec2 minPos, maxPos;
      unsigned int longestSubpath = 0;

      float distanceTolerance = 0.0f;

      subpath_t currentPath;
      path_t paths;

      void drawArcTo(MuCanvasContext2D *context, glm::vec2 point, glm::vec2 p1, glm::vec2 p2, MuColorRGBA color);
  };

}

#endif

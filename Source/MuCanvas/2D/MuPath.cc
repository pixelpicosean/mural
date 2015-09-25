#include "MuPath.h"
#include "MuCanvasContext2D.h"

#include <algorithm>

namespace mural {

  MuPath::MuPath() {
    reset();
  }

  void MuPath::push(MuVector2 v) {
    // Ignore this point if it's identical to the last
    if ((v.x == lastPushed.x && v.y == lastPushed.y) && !currentPath.points.empty()) {
      return;
    }
    lastPushed = v;

    minPos.x = std::min(minPos.x, v.x);
    minPos.y = std::min(minPos.y, v.y);
    maxPos.x = std::max(maxPos.x, v.x);
    maxPos.y = std::max(maxPos.y, v.y);
    currentPath.points.push_back(v);
  }

  void MuPath::reset() {
    longestSubpath = 0;
    paths.clear();
    currentPath.isClosed = false;
    currentPath.points.clear();

    currentPos = MuVector2(0.0f, 0.0f);

    minPos = MuVector2(INFINITY, INFINITY);
    maxPos = MuVector2(-INFINITY, -INFINITY);
  }

  void MuPath::close() {
    currentPath.isClosed = true;
    if (currentPath.points.size()) {
        currentPos = currentPath.points.front();
        push(currentPos);
    }
    endSubPath();
  }

  void MuPath::endSubPath() {
    if (currentPath.points.size() > 1) {
      paths.push_back(currentPath);
      longestSubpath = std::max(longestSubpath, (unsigned int)currentPath.points.size());
    }
    currentPath.points.clear();
    currentPath.isClosed = false;
  }

  void MuPath::moveTo(float x, float y) {
    endSubPath();
    currentPos = MuVector2ApplyTransform(MuVector2(x, y), transform);
    push(currentPos);
  }

  void MuPath::lineTo(float x, float y) {
    currentPos = MuVector2ApplyTransform(MuVector2(x, y), transform);
    push(currentPos);
  }

  void MuPath::bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y, float scale) {
    distanceTolerance = MU_PATH_DISTANCE_EPSILON / scale;
    distanceTolerance *= distanceTolerance;

    MuVector2 cp1 = MuVector2ApplyTransform(MuVector2(cpx1, cpy1), transform);
    MuVector2 cp2 = MuVector2ApplyTransform(MuVector2(cpx2, cpy2), transform);
    MuVector2 p = MuVector2ApplyTransform(MuVector2(x, y), transform);

    recursiveBezier(currentPos.x, currentPos.y, cp1.x, cp1.y, cp2.x, cp2.y, p.x, p.y, 0);
    currentPos = p;
    push(currentPos);
  }

  void MuPath::recursiveBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level) {
    // Based on http://www.antigrain.com/research/adaptive_bezier/index.html

    // Calculate all the mid-points of the line segments
    float x12 = (x1 + x2) / 2;
    float y12 = (y1 + y2) / 2;
    float x23 = (x2 + x3) / 2;
    float y23 = (y2 + y3) / 2;
    float x34 = (x3 + x4) / 2;
    float y34 = (y3 + y4) / 2;
    float x123 = (x12 + x23) / 2;
    float y123 = (y12 + y23) / 2;
    float x234 = (x23 + x34) / 2;
    float y234 = (y23 + y34) / 2;
    float x1234 = (x123 + x234) / 2;
    float y1234 = (y123 + y234) / 2;

    if (level > 0) {
      // Enforce subdivision first time
      // Try to approximate the full cubic curve by a single straight line
      float dx = x4-x1;
      float dy = y4-y1;

      float d2 = fabsf(((x2 - x4) * dy - (y2 - y4) * dx));
      float d3 = fabsf(((x3 - x4) * dy - (y3 - y4) * dx));

      if (d2 > MU_PATH_COLLINEARITY_EPSILON && d3 > MU_PATH_COLLINEARITY_EPSILON) {
        // Regular care
        if ((d2 + d3)*(d2 + d3) <= distanceTolerance * (dx*dx + dy*dy)) {
          // If the curvature doesn't exceed the distance_tolerance value
          // we tend to finish subdivisions.
          push(MuVector2(x1234, y1234));
          return;
        }
      }
      else {
        if (d2 > MU_PATH_COLLINEARITY_EPSILON) {
          // p1,p3,p4 are collinear, p2 is considerable
          if (d2 * d2 <= distanceTolerance * (dx*dx + dy*dy)) {
            push(MuVector2(x1234, y1234));
            return;
          }
        }
        else if (d3 > MU_PATH_COLLINEARITY_EPSILON) {
          // p1,p2,p4 are collinear, p3 is considerable
          if (d3 * d3 <= distanceTolerance * (dx*dx + dy*dy)) {
            push(MuVector2(x1234, y1234));
            return;
          }
        }
        else {
          // Collinear case
          dx = x1234 - (x1 + x4) / 2;
          dy = y1234 - (y1 + y4) / 2;
          if (dx*dx + dy*dy <= distanceTolerance) {
            push(MuVector2(x1234, y1234));
            return;
          }
        }
      }
    }

    if (level <= MU_PATH_RECURSION_LIMIT) {
      // Continue subdivision
      recursiveBezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
      recursiveBezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
    }
  }

  void MuPath::quadraticCurveTo(float cpx, float cpy, float x, float y, float scale) {
    distanceTolerance = MU_PATH_DISTANCE_EPSILON / scale;
    distanceTolerance *= distanceTolerance;

    MuVector2 cp = MuVector2ApplyTransform(MuVector2(cpx, cpy), transform);
    MuVector2 p = MuVector2ApplyTransform(MuVector2(x, y), transform);

    recursiveQuadratic(currentPos.x, currentPos.y, cp.x, cp.y, p.x, p.y, 0);
    currentPos = p;
    push(currentPos);
  }

  void MuPath::recursiveQuadratic(float x1, float y1, float x2, float y2, float x3, float y3, int level) {
    // Based on http://www.antigrain.com/research/adaptive_bezier/index.html

    // Calculate all the mid-points of the line segments
    float x12 = (x1 + x2) / 2;
    float y12 = (y1 + y2) / 2;
    float x23 = (x2 + x3) / 2;
    float y23 = (y2 + y3) / 2;
    float x123 = (x12 + x23) / 2;
    float y123 = (y12 + y23) / 2;

    float dx = x3-x1;
    float dy = y3-y1;
    float d = fabsf(((x2 - x3) * dy - (y2 - y3) * dx));

    if (d > MU_PATH_COLLINEARITY_EPSILON) {
      // Regular care
      if (d * d <= distanceTolerance * (dx*dx + dy*dy)) {
        push(MuVector2(x123, y123));
        return;
      }
    }
    else {
      // Collinear case
      dx = x123 - (x1 + x3) / 2;
      dy = y123 - (y1 + y3) / 2;
      if (dx*dx + dy*dy <= distanceTolerance) {
        push(MuVector2(x123, y123));
        return;
      }
    }

    if (level <= MU_PATH_RECURSION_LIMIT) {
      // Continue subdivision
      recursiveQuadratic(x1, y1, x12, y12, x123, y123, level + 1);
      recursiveQuadratic(x123, y123, x23, y23, x3, y3, level + 1);
    }
  }

  void MuPath::arcTo(float x1, float y1, float x2, float y2, float radius) {
    // Lifted from http://code.google.com/p/fxcanvas/
    // I have no idea what this code is doing, but it seems to work.

    // get untransformed currentPos
    MuVector2 cp = MuVector2ApplyTransform(currentPos, MuAffineTransformInvert(transform));

    float a1 = cp.y - y1;
    float b1 = cp.x - x1;
    float a2 = y2 - y1;
    float b2 = x2 - x1;
    float mm = fabsf(a1 * b2 - b1 * a2);

    if (mm < 1.0e-8 || radius == 0) {
      lineTo(x1, y1);
    }
    else {
      float dd = a1 * a1 + b1 * b1;
      float cc = a2 * a2 + b2 * b2;
      float tt = a1 * a2 + b1 * b2;
      float k1 = radius * sqrtf(dd) / mm;
      float k2 = radius * sqrtf(cc) / mm;
      float j1 = k1 * tt / dd;
      float j2 = k2 * tt / cc;
      float cx = k1 * b2 + k2 * b1;
      float cy = k1 * a2 + k2 * a1;
      float px = b1 * (k2 + j1);
      float py = a1 * (k2 + j1);
      float qx = b2 * (k1 + j2);
      float qy = a2 * (k1 + j2);
      float startAngle = atan2f(py - cy, px - cx);
      float endAngle = atan2f(qy - cy, qx - cx);

      arc(cx + x1, cy + y1, radius, startAngle, endAngle, (b1 * a2 > b2 * a1));
    }
  }

  void MuPath::arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {
    startAngle = fmodf(startAngle, 2 * M_PI);
    endAngle = fmodf(endAngle, 2 * M_PI);

    if (!antiClockwise && endAngle <= startAngle) {
      endAngle += 2 * M_PI;
    }
    else if (antiClockwise && startAngle <= endAngle) {
      startAngle += 2 * M_PI;
    }

    float span = antiClockwise ? (startAngle - endAngle) * -1 : (endAngle - startAngle);

    // Calculate the number of steps, based on the radius, scaling and the span
    float size = radius * MuAffineTransformGetScale(transform) * 5;
    float maxSteps = MU_PATH_MAX_STEPS_FOR_CIRCLE * fabsf(span)/(2 * M_PI);
    int steps = std::max(MU_PATH_MIN_STEPS_FOR_CIRCLE, (size / (200+size)) * maxSteps);

    float stepSize = span / (float)steps;
    float angle = startAngle;
    for( int i = 0; i < steps; i++, angle += stepSize ) {
      currentPos = MuVector2ApplyTransform(MuVector2(x + cosf(angle) * radius, y + sinf(angle) * radius), transform);
      push(currentPos);
    }

    // Add the final step or close to the first one if it's a full circle
    float lastAngle = (fabsf(span) < 2 * M_PI - FLT_EPSILON) ? angle : startAngle;
    currentPos = MuVector2ApplyTransform(MuVector2(x + cosf(lastAngle) * radius, y + sinf(lastAngle) * radius), transform);
    push(currentPos);
  }

  void MuPath::drawPolygonsToContext(MuCanvasContext2D *context, MuPathFillRule rule, MuPathPolygonTarget target) {
    fillRule = rule;
    if (longestSubpath < 3 && currentPath.points.size() < 3) { return; }

    MuCanvasState *state = context->state;
    MuColorRGBA white = { .hex = 0xffffffff };

    // For potentially concave polygons (those with more than 3 unique vertices), we
    // need to draw to the context twice: first to create a stencil mask, and then again
    // to fill the created mask with the polygons color.
    // TODO: add a fast path for polygons that only have 3 vertices

    context->flushBuffers();
    context->createStencilBufferOnce();


    // Disable drawing to the color buffer, enable the stencil buffer
    glDisableVertexAttribArray(kMuGLProgram2DAttributeUV);
    glDisableVertexAttribArray(kMuGLProgram2DAttributeColor);

    glDisable(GL_BLEND);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xff);

    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


    // Clear the needed area in the stencil buffer

    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    glm::mat3 identity;
    context->pushRect(minPos.x, minPos.y, maxPos.x-minPos.x, maxPos.y-minPos.y, white, MuAffineTransform::identity);
    context->flushBuffers();


    if (fillRule == kMuPathFillRuleNonZero) {
      // If we use the Non-Zero fill rule, draw to the stencil buffer twice
      // for each sub path:
      // 1) for all back-facing polygons, increase the stencil value
      // 2) for all front-facing polygons, decrease the stencil value

      // We need to enable face culling for this and alternate the stencil
      // ops in the draw loop.
      glEnable(GL_CULL_FACE);
    }
    else if (fillRule == kMuPathFillRuleEvenOdd) {
      // For the Even-Odd fill rule, simply invert the stencil buffer with
      // each overdraw/
      glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
    }

    for (path_t::iterator sp = paths.begin(); ; ++sp) {
      subpath_t &path = (sp == paths.end()) ? currentPath : *sp;

      glVertexAttribPointer(kMuGLProgram2DAttributePos, 2, GL_FLOAT, GL_FALSE, 0, &(path.points).front());

      if (fillRule == kMuPathFillRuleNonZero) {
        glCullFace(GL_BACK);
        glStencilOp(GL_INCR_WRAP, GL_KEEP, GL_INCR_WRAP);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (int)path.points.size());

        glCullFace(GL_FRONT);
        glStencilOp(GL_DECR_WRAP, GL_KEEP, GL_DECR_WRAP);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (int)path.points.size());
      }
      else if (fillRule == kMuPathFillRuleEvenOdd) {
        glDrawArrays(GL_TRIANGLE_FAN, 0, (int)path.points.size());
      }

      if (sp==paths.end()) break;
    }

    if (fillRule == kMuPathFillRuleNonZero) {
      glDisable(GL_CULL_FACE);
    }

    context->bindVertexBuffer();


    // Enable drawing to the color or depth buffer and push a rect with the correct
    // size and color to the context. This rect will also clear the stencil buffer
    // again.

    if (target == kMuPathPolygonTargetDepth) {
      glDepthFunc(GL_ALWAYS);
      glDepthMask(GL_TRUE);
      glClear(GL_DEPTH_BUFFER_BIT);
    }
    else if (target == kMuPathPolygonTargetColor) {
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glEnable(GL_BLEND);
    }

    glStencilFunc(GL_NOTEQUAL, 0x00, 0xff);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

    if (state->fillObject && target == kMuPathPolygonTargetColor) {
      // If we have a fill pattern or gradient, we have to do some extra work to unproject the
      // Quad we're drawing, so we can then project it _with_ the pattern/gradient again

      MuAffineTransform inverse = MuAffineTransformInvert(transform);
      MuVector2 p1 = MuVector2ApplyTransform(minPos, inverse);
      MuVector2 p2 = MuVector2ApplyTransform(MuVector2(maxPos.x, minPos.y), inverse);
      MuVector2 p3 = MuVector2ApplyTransform(MuVector2(minPos.x, maxPos.y), inverse);
      MuVector2 p4 = MuVector2ApplyTransform(maxPos, inverse);

      // Find the unprojected min/max
      MuVector2 tmin(std::min(p1.x, std::min(p2.x,std::min(p3.x, p4.x))), std::min(p1.y, std::min(p2.y,std::min(p3.y, p4.y))));
      MuVector2 tmax(std::max(p1.x, std::max(p2.x,std::max(p3.x, p4.x))), std::max(p1.y, std::max(p2.y,std::max(p3.y, p4.y))));

      context->pushFilledRect(tmin.x, tmin.y, tmax.x-tmin.x, tmax.y-tmin.y, state->fillObject, MuCanvasBlendWhiteColor(state), transform);
    }
    else {
        context->pushRect(minPos.x, minPos.y, maxPos.x-minPos.x, maxPos.y-minPos.y, MuCanvasBlendFillColor(state), MuAffineTransform::identity);
    }

    context->flushBuffers();
    glDisable(GL_STENCIL_TEST);

    if (target == kMuPathPolygonTargetDepth) {
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_EQUAL);

      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glEnable(GL_BLEND);
    }
  }

  void MuPath::drawArcTo(MuCanvasContext2D *context, MuVector2 point, MuVector2 p1, MuVector2 p2, MuColorRGBA color) {
    MuCanvasState *state = context->state;
    float width2 = state->lineWidth / 2;

    MuVector2
      v1 = MuVector2Normalize(MuVector2Sub(p1, point)),
      v2 = MuVector2Normalize(MuVector2Sub(p2, point));

    // calculate starting angle for arc
    float angle1 = atan2(1, 0) - atan2(v1.x, -v1.y);

    // calculate smallest angle between both vectors
    // colinear vectors (for caps) need to be handled seperately
    float angle2;
    if (v1.x == -v2.x && v1.y == -v2.y) {
      angle2 = 3.14;
    }
    else {
      angle2 = acosf(v1.x * v2.x + v1.y * v2.y);
    }

    // 1 step per 5 pixel
    float pxScale = MuAffineTransformGetScale(state->transform);
    int numSteps = ceilf((angle2 * width2 * pxScale) / 5.0f);

    if (numSteps == 1) {
      context->pushTri(p1.x, p1.y, point.x, point.y, p2.x, p2.y, color, transform);
      return;
    }
    // avoid MuPath::"triangular" look
    else if (numSteps == 3 && fabsf(angle2) > M_PI_2) {
      numSteps = 4;
    }

    // calculate direction
    float direction = (v2.x*v1.y - v2.y*v1.x < 0) ? -1 : 1;

    // calculate angle step
    float step = (angle2/numSteps) * direction;

    // starting point
    float angle = angle1;

    MuVector2 arcP1(point.x + cosf(angle) * width2, point.y - sinf(angle) * width2);
    MuVector2 arcP2;

    for (int i = 0; i < numSteps; i++) {
      angle += step;
      arcP2 = MuVector2(point.x + cosf(angle) * width2, point.y - sinf(angle) * width2);

      context->pushTri(arcP1.x, arcP1.y, point.x, point.y, arcP2.x, arcP2.y, color, transform);

      arcP1 = arcP2;
    }
  }

  void MuPath::drawLinesToContext(MuCanvasContext2D *context) {
    MuCanvasState *state = context->state;
    GLubyte stencilMask;

    // Find the width of the line as it is projected onto the screen.
    float projectedLineWidth = MuAffineTransformGetScale(state->transform) * state->lineWidth;

    // Figure out if we need to add line caps and set the cap texture coord for square or round caps.
    // For thin lines we disable texturing and line caps.
    float width2 = state->lineWidth/2;
    bool addCaps = (projectedLineWidth > 2 && (state->lineCap == kMuLineCapRound || state->lineCap == kMuLineCapSquare));

    // The miter limit is the maximum allowed ratio of the miter length to half the line width.
    bool addMiter = (state->lineJoin == kMuLineJoinMiter);
    float miterLimit = (state->miterLimit * width2);

    MuColorRGBA color = MuCanvasBlendStrokeColor(state);

    // Enable stencil test when drawing transparent lines or if we have stroke pattern or gradient.
    // Cycle through all bits, so that the stencil buffer only has to be cleared after eight
    // stroke operations
    bool useStencil = (
      color.rgba.a < 0xff ||
      state->globalCompositeOperation != kMuCompositeOperationSourceOver ||
      state->strokeObject
    );
    bool fillStencil = (useStencil && state->strokeObject);

    if (useStencil) {
      context->flushBuffers();
      context->createStencilBufferOnce();
      stencilMask = context->stencilMask;

      glEnable(GL_STENCIL_TEST);

      glStencilMask(stencilMask);

      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glStencilFunc(GL_NOTEQUAL, stencilMask, stencilMask);

      // If we have a stroke object, we also have to disable drawing to the color buffer,
      // so we can later fill the stencil mask with the pattern or gradient
      if (fillStencil) {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      }
    }


    // To draw the line correctly with transformations, we need to construct the line
    // vertices from the untransformed points and only apply the transformation in
    // the last step (pushQuad) again.
    MuAffineTransform inverseTransform = MuAffineTransformInvert(transform);


    // Oh god, I'm so sorry... This code sucks quite a bit. I'd be surprised if I
    // will understand what I've written in 3 days :/
    // Calculating line miters for potentially closed paths is serious business!
    // And it doesn't even handle all the edge cases.

    MuVector2
      *transCurrent, *transNext,  // Pointers to current and next vertices on the line
      current, next,        // Untransformed current and next points
      firstMiter1, firstMiter2, // First miter vertices (left, right) needed for closed paths
      miter11, miter12,     // Current miter vertices (left, right)
      miter21, miter22,     // Next miter vertices (left, right)
      currentEdge, currentExt,  // Current edge and its normal * width/2
      nextEdge, nextExt;      // Next edge and its normal * width/2

    // Keep track of min and max drawing points if we have fill object, so we can later
    // draw a quad with the minimum required size
    MuVector2 drawMin = minPos, drawMax = maxPos;

    for (path_t::iterator sp = paths.begin(); ; ++sp) {
      subpath_t &path = (sp == paths.end()) ? currentPath : *sp;
      if (sp == paths.end() && currentPath.points.size() <= 1) break;

      bool subPathIsClosed = path.isClosed;
      bool ignoreFirstSegment = addMiter && subPathIsClosed;
      bool firstInSubPath = true;
      bool miterLimitExceeded = false, firstMiterLimitExceeded = false;

      transCurrent = transNext = nullptr;

      // If this subpath is closed, initialize the first vertex for the loop ("next")
      // to the last vertex in the subpath. This way, the miter between the last and
      // the first segment will be computed and used to draw the first segment's first
      // miter, as well as the last segment's last miter outside the loop.
      if (addMiter && subPathIsClosed) {
        transNext = &path.points.at(path.points.size()-2);
        next = MuVector2ApplyTransform(*transNext, inverseTransform);
      }

      for (points_t::iterator vertex = path.points.begin(); vertex != path.points.end(); ++vertex) {
        transCurrent = transNext;
        transNext = &(*vertex);

        current = next;
        next = MuVector2ApplyTransform(*transNext, inverseTransform);

        if (!transCurrent) { continue; }

        currentEdge = nextEdge;
        currentExt = nextExt;
        nextEdge = MuVector2Normalize(MuVector2Sub(next, current));
        nextExt = MuVector2(-nextEdge.y * width2, nextEdge.x * width2);

        if (firstInSubPath) {
          firstMiter1 = miter21 = MuVector2Add(current, nextExt);
          firstMiter2 = miter22 = MuVector2Sub(current, nextExt);
          firstInSubPath = false;

          // Start cap
          if (addCaps && !subPathIsClosed) {
            MuVector2 capExt(-nextExt.y, nextExt.x);
            MuVector2 cap11 = MuVector2Add(miter21, capExt);
            MuVector2 cap12 = MuVector2Add(miter22, capExt);

            if (state->lineCap == kMuLineCapSquare) {
              context->pushQuad(cap11, cap12, miter21, miter22, color, transform);
            }
            else {
              drawArcTo(context, current, miter22, miter21, color);
            }
          }

          continue;
        }


        miter11 = miter21;
        miter12 = miter22;

        bool miterAdded = false;
        if (addMiter) {
          MuVector2 miterEdge = MuVector2Add(currentEdge, nextEdge);
          float miterExt = (1 / MuVector2Dot(miterEdge, miterEdge)) * state->lineWidth;

          if (miterExt < miterLimit) {
            miterEdge.x *= miterExt;
            miterEdge.y *= miterExt;
            miter21 = MuVector2(current.x - miterEdge.y, current.y + miterEdge.x);
            miter22 = MuVector2(current.x + miterEdge.y, current.y - miterEdge.x);

            miterAdded = true;
            miterLimitExceeded = false;

            // If we have to fill the stroke later, we need to adjust the min and max
            // points for our bounding box - the miter may be outside of it.
            if (fillStencil) {
              drawMin.x = std::min(drawMin.x, std::min(miter21.x, miter22.x));
              drawMin.y = std::min(drawMin.y, std::min(miter21.y, miter22.y));
              drawMax.x = std::max(drawMax.x, std::max(miter21.x, miter22.x));
              drawMax.y = std::max(drawMax.y, std::max(miter21.y, miter22.y));
            }
          }
          else {
            miterLimitExceeded = true;
          }
        }

        // No miter added? Calculate the butt for the current segment
        if (!miterAdded) {
          miter21 = MuVector2Add(current, currentExt);
          miter22 = MuVector2Sub(current, currentExt);
        }

        if (ignoreFirstSegment) {
          // True when starting from the back vertex of a closed path. This run was just
          // to calculate the first miter.
          firstMiter1 = miter21;
          firstMiter2 = miter22;
          if (!miterAdded) {
            // Flip miter21 <> miter22 if it's the butt for the first segment
            miter21 = firstMiter2;
            miter22 = firstMiter1;
          }
          firstMiterLimitExceeded = miterLimitExceeded;
          ignoreFirstSegment = false;
          continue;
        }

        if (!addMiter || miterLimitExceeded) {
          // previous point can be approximated, good enough for distance comparison
          MuVector2 prev = MuVector2Sub(current, currentEdge);
          MuVector2 p1, p2;
          float d1, d2;

          // calculate points to use for bevel
          // two points are possible for each edge - the one farthest away from the other line has to be used

          // calculate point for current edge
          d1 = MuDistanceToLineSegmentSquared(miter21, current, next);
          d2 = MuDistanceToLineSegmentSquared(miter22, current, next);
          p1 = (d1 > d2) ? miter21 : miter22;

          // calculate point for next edge
          d1 = MuDistanceToLineSegmentSquared(MuVector2Add(current, nextExt), current, prev);
          d2 = MuDistanceToLineSegmentSquared(MuVector2Sub(current, nextExt), current, prev);
          p2 = (d1 > d2) ? MuVector2Add(current, nextExt) : MuVector2Sub(current, nextExt);



          if (state->lineJoin == kMuLineJoinRound) {
            drawArcTo(context, current, p1, p2, color);
          }
          else {
            context->pushTri(p1.x, p1.y, current.x, current.y, p2.x, p2.y, color, transform);
          }
        }

        context->pushQuad(miter11, miter12, miter21, miter22, color, transform);

        // No miter added? The "miter" for the next segment needs to be the butt for the next segment,
        // not the butt for the current one.
        if (!miterAdded) {
          miter21 = MuVector2Add(current, nextExt);
          miter22 = MuVector2Sub(current, nextExt);
        }
      } // for each subpath


      // The last segment, not handled in the loop
      if (!firstMiterLimitExceeded && addMiter && subPathIsClosed) {
        miter11 = firstMiter1;
        miter12 = firstMiter2;
      }
      else {
        MuVector2 untransformedBack = MuVector2ApplyTransform(path.points.back(), inverseTransform);
        miter11 = MuVector2Add(untransformedBack, nextExt);
        miter12 = MuVector2Sub(untransformedBack, nextExt);
      }

      if ((!addMiter || firstMiterLimitExceeded) && subPathIsClosed) {
        float d1, d2;
        MuVector2 p1, p2,
          firstNormal = MuVector2Sub(firstMiter1, firstMiter2), // unnormalized line normal for first edge
          second = MuVector2Add(next, MuVector2(firstNormal.y, -firstNormal.x)); // approximated second point

        // calculate points to use for bevel
        // two points are possible for each edge - the one farthest away from the other line has to be used

        // calculate point for current edge
        d1 = MuDistanceToLineSegmentSquared(miter12, next, second);
        d2 = MuDistanceToLineSegmentSquared(miter11, next, second);
        p2 = (d1 > d2) ? miter12 : miter11;

        // calculate point for next edge
        d1 = MuDistanceToLineSegmentSquared(firstMiter1, current, next);
        d2 = MuDistanceToLineSegmentSquared(firstMiter2, current, next);
        p1 = (d1 > d2) ? firstMiter1 : firstMiter2;

        if (state->lineJoin == kMuLineJoinRound) {
          drawArcTo(context, next, p1, p2, color);
        }
        else {
          context->pushTri(p1.x, p1.y, next.x, next.y, p2.x, p2.y, color, transform);
        }
      }

      context->pushQuad(miter11, miter12, miter21, miter22, color, transform);

      // End cap
      if (addCaps && !subPathIsClosed) {
        if (state->lineCap == kMuLineCapSquare) {
          MuVector2 capExt(nextExt.y, -nextExt.x);
          MuVector2 cap11 = MuVector2Add(miter11, capExt);
          MuVector2 cap12 = MuVector2Add(miter12, capExt);

          context->pushQuad(cap11, cap12, miter11, miter12, color, transform);
        }
        else {
          drawArcTo(context, next, miter11, miter12, color);
        }
      }

      if (sp == paths.end()) break;
    } // for each path

    // disable stencil test when drawing transparent lines
    if (useStencil) {
      context->flushBuffers();

      if (fillStencil) {
        // Fill the stencil mask with the strokeObject
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_EQUAL, stencilMask, stencilMask);

        // Add a bit of padding to the fill rect, so all line caps are included
        float padding = width2 * M_SQRT2;
        context->pushFilledRect(
          drawMin.x-padding, drawMin.y-padding, drawMax.x-drawMin.x+padding*2, drawMax.y-drawMin.y+padding*2,
          state->strokeObject, MuCanvasBlendWhiteColor(state),
          transform
        );

        context->flushBuffers();
      }

      glDisable(GL_STENCIL_TEST);

      if (stencilMask == (1<<7)) {
        context->stencilMask = (1 << 0);

        glStencilMask(0xff);
        glClearStencil(0x0);
        glClear(GL_STENCIL_BUFFER_BIT);
      }
      else {
        context->stencilMask = (stencilMask << 1);
      }
    }
  }

}

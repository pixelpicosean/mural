//
//  MuPath.cpp
//  ciEjecta
//
//  Created by Sean on 10/3/15.
//
//

#include "MuPath.hpp"
#include "MuCanvasContext2D.hpp"

namespace mural {

  MuPath::MuPath() {
    reset();
  }

  void MuPath::push(vec2 v) {
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

    currentPos = vec2(0.0f, 0.0f);

    minPos = vec2(INFINITY, INFINITY);
    maxPos = vec2(-INFINITY, -INFINITY);
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
    currentPos = vec2(x, y);
    transform.applyTo(currentPos);
    push(currentPos);
  }

  void MuPath::lineTo(float x, float y) {
    currentPos = vec2(x, y);
    transform.applyTo(currentPos);
    push(currentPos);
  }

  void MuPath::bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y, float scale) {
    distanceTolerance = MU_PATH_DISTANCE_EPSILON / scale;
    distanceTolerance *= distanceTolerance;

    vec2 cp1(cpx1, cpy1);
    transform.applyTo(cp1);
    vec2 cp2(cpx2, cpy2);
    transform.applyTo(cp2);
    vec2 p(x, y);
    transform.applyTo(p);

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
          push(vec2(x1234, y1234));
          return;
        }
      }
      else {
        if (d2 > MU_PATH_COLLINEARITY_EPSILON) {
          // p1,p3,p4 are collinear, p2 is considerable
          if (d2 * d2 <= distanceTolerance * (dx*dx + dy*dy)) {
            push(vec2(x1234, y1234));
            return;
          }
        }
        else if (d3 > MU_PATH_COLLINEARITY_EPSILON) {
          // p1,p2,p4 are collinear, p3 is considerable
          if (d3 * d3 <= distanceTolerance * (dx*dx + dy*dy)) {
            push(vec2(x1234, y1234));
            return;
          }
        }
        else {
          // Collinear case
          dx = x1234 - (x1 + x4) / 2;
          dy = y1234 - (y1 + y4) / 2;
          if (dx*dx + dy*dy <= distanceTolerance) {
            push(vec2(x1234, y1234));
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

    vec2 cp(cpx, cpy);
    transform.applyTo(cp);
    vec2 p(x, y);
    transform.applyTo(p);

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

    float dx = x3 - x1;
    float dy = y3 - y1;
    float d = fabsf(((x2 - x3) * dy - (y2 - y3) * dx));

    if (d > MU_PATH_COLLINEARITY_EPSILON) {
      // Regular care
      if (d * d <= distanceTolerance * (dx * dx + dy * dy)) {
        push(vec2(x123, y123));
        return;
      }
    }
    else {
      // Collinear case
      dx = x123 - (x1 + x3) / 2;
      dy = y123 - (y1 + y3) / 2;
      if (dx * dx + dy * dy <= distanceTolerance) {
        push(vec2(x123, y123));
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
    vec2 cp(currentPos);
    MuAffineTransform inv = transform;
    inv.invert();
    inv.applyTo(cp);

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
    float size = radius * transform.getScale() * 5;
    float maxSteps = MU_PATH_MAX_STEPS_FOR_CIRCLE * fabsf(span)/(2 * M_PI);
    int steps = std::max(MU_PATH_MIN_STEPS_FOR_CIRCLE, (size / (200+size)) * maxSteps);

    float stepSize = span / (float)steps;
    float angle = startAngle;
    for( int i = 0; i < steps; i++, angle += stepSize ) {
      vec2 pos(x + cosf(angle) * radius, y + sinf(angle) * radius);
      currentPos = transform.applyTo(pos);
      push(currentPos);
    }

    // Add the final step or close to the first one if it's a full circle
    float lastAngle = (fabsf(span) < 2 * M_PI - FLT_EPSILON) ? angle : startAngle;
    vec2 pos(x + cosf(lastAngle) * radius, y + sinf(lastAngle) * radius);
    currentPos = transform.applyTo(pos);
    push(currentPos);
  }

  void MuPath::drawPolygonsToContext(MuCanvasContext2D *context, MuPathPolygonTarget target) {
    if (longestSubpath < 3 && currentPath.points.size() < 3) { return; }

    MuCanvasState *state = context->state;

    if (state->fillObject && target == kMuPathPolygonTargetColor) {
      // If we have a fill pattern or gradient, we have to do some extra work to unproject the
      // Quad we're drawing, so we can then project it _with_ the pattern/gradient again

      MuAffineTransform inverse = transform;
      inverse.invert();

      vec2 p1(minPos);
      inverse.applyTo(p1);
      vec2 p2(maxPos.x, minPos.y);
      inverse.applyTo(p2);
      vec2 p3(minPos.x, maxPos.y);
      inverse.applyTo(p3);
      vec2 p4(maxPos);
      inverse.applyTo(p4);

      // Find the unprojected min/max
      vec2 tmin(std::min(p1.x, std::min(p2.x,std::min(p3.x, p4.x))), std::min(p1.y, std::min(p2.y,std::min(p3.y, p4.y))));
      vec2 tmax(std::max(p1.x, std::max(p2.x,std::max(p3.x, p4.x))), std::max(p1.y, std::max(p2.y,std::max(p3.y, p4.y))));

      // context->pushFilledRect(tmin.x, tmin.y, tmax.x-tmin.x, tmax.y-tmin.y, state->fillObject, blendWhiteColor(state), transform);
      console() << "Filled rect is not supported yet!" << std::endl;
    }
    else {
      context->pushRect(minPos.x, minPos.y, maxPos.x-minPos.x, maxPos.y-minPos.y, blendFillColor(state), MuAffineTransform::identity);
    }

     context->flushBuffers();
  }

  void MuPath::drawArcTo(MuCanvasContext2D *context, const vec2 &point, const vec2 &p1, const vec2 &p2, const ColorAf &color) {
    MuCanvasState *state = context->state;
    float width2 = state->lineWidth / 2;

    vec2 v1 = glm::normalize(p1 - point);
    vec2 v2 = glm::normalize(p2 - point);

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
    float pxScale = state->transform.getScale();
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

    vec2 arcP1(point.x + cosf(angle) * width2, point.y - sinf(angle) * width2);
    vec2 arcP2;

    for (int i = 0; i < numSteps; i++) {
      angle += step;
      arcP2 = vec2(point.x + cosf(angle) * width2, point.y - sinf(angle) * width2);

      context->pushTri(arcP1.x, arcP1.y, point.x, point.y, arcP2.x, arcP2.y, color, transform);

      arcP1 = arcP2;
    }
  }

  void MuPath::drawLinesToContext(MuCanvasContext2D *context) {
    MuCanvasState *state = context->state;

    // Find the width of the line as it is projected onto the screen.
    float projectedLineWidth = state->transform.getScale() * state->lineWidth;

    // Figure out if we need to add line caps and set the cap texture coord for square or round caps.
    // For thin lines we disable texturing and line caps.
    float width2 = state->lineWidth / 2;
    bool addCaps = (projectedLineWidth > 2 && (state->lineCap == kMuLineCapRound || state->lineCap == kMuLineCapSquare));

    // The miter limit is the maximum allowed ratio of the miter length to half the line width.
    bool addMiter = (state->lineJoin == kMuLineJoinMiter);
    float miterLimit = (state->miterLimit * width2);

    ColorAf color = blendStrokeColor(state);


    // To draw the line correctly with transformations, we need to construct the line
    // vertices from the untransformed points and only apply the transformation in
    // the last step (pushQuad) again.
    MuAffineTransform inverseTransform = transform;
    inverseTransform.invert();


    // Oh god, I'm so sorry... This code sucks quite a bit. I'd be surprised if I
    // will understand what I've written in 3 days :/
    // Calculating line miters for potentially closed paths is serious business!
    // And it doesn't even handle all the edge cases.

    vec2
      *transCurrent, *transNext,  // Pointers to current and next vertices on the line
      current, next,        // Untransformed current and next points
      firstMiter1, firstMiter2, // First miter vertices (left, right) needed for closed paths
      miter11, miter12,     // Current miter vertices (left, right)
      miter21, miter22,     // Next miter vertices (left, right)
      currentEdge, currentExt,  // Current edge and its normal * width/2
      nextEdge, nextExt;      // Next edge and its normal * width/2

    // Keep track of min and max drawing points if we have fill object, so we can later
    // draw a quad with the minimum required size
    vec2 drawMin = minPos, drawMax = maxPos;

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
        next = *transNext;
        inverseTransform.applyTo(next);
      }

      for (points_t::iterator vertex = path.points.begin(); vertex != path.points.end(); ++vertex) {
        transCurrent = transNext;
        transNext = &(*vertex);

        current = next;
        next = *transNext;
        inverseTransform.applyTo(next);

        if (!transCurrent) { continue; }

        currentEdge = nextEdge;
        currentExt = nextExt;
        nextEdge = glm::normalize(next - current);
        nextExt = vec2(-nextEdge.y * width2, nextEdge.x * width2);

        if (firstInSubPath) {
          firstMiter1 = miter21 = current + nextExt;
          firstMiter2 = miter22 = current - nextExt;
          firstInSubPath = false;

          // Start cap
          if (addCaps && !subPathIsClosed) {
            vec2 capExt(-nextExt.y, nextExt.x);
            vec2 cap11 = miter21 + capExt;
            vec2 cap12 = miter22 + capExt;

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
          vec2 miterEdge = currentEdge + nextEdge;
          float miterExt = (1 / glm::dot(miterEdge, miterEdge)) * state->lineWidth;

          if (miterExt < miterLimit) {
            miterEdge.x *= miterExt;
            miterEdge.y *= miterExt;
            miter21 = vec2(current.x - miterEdge.y, current.y + miterEdge.x);
            miter22 = vec2(current.x + miterEdge.y, current.y - miterEdge.x);

            miterAdded = true;
            miterLimitExceeded = false;
          }
          else {
            miterLimitExceeded = true;
          }
        }

        // No miter added? Calculate the butt for the current segment
        if (!miterAdded) {
          miter21 = current + currentExt;
          miter22 = current - currentExt;
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
          vec2 prev = current - currentEdge;
          vec2 p1, p2;
          float d1, d2;

          // calculate points to use for bevel
          // two points are possible for each edge - the one farthest away from the other line has to be used

          // calculate point for current edge
          d1 = distanceToLineSegmentSquared(miter21, current, next);
          d2 = distanceToLineSegmentSquared(miter22, current, next);
          p1 = (d1 > d2) ? miter21 : miter22;

          // calculate point for next edge
          d1 = distanceToLineSegmentSquared(current + nextExt, current, prev);
          d2 = distanceToLineSegmentSquared(current - nextExt, current, prev);
          p2 = (d1 > d2) ? (current + nextExt) : (current - nextExt);


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
          miter21 = current + nextExt;
          miter22 = current - nextExt;
        }
      } // for each subpath


      // The last segment, not handled in the loop
      if (!firstMiterLimitExceeded && addMiter && subPathIsClosed) {
        miter11 = firstMiter1;
        miter12 = firstMiter2;
      }
      else {
        vec2 untransformedBack(path.points.back());
        inverseTransform.applyTo(untransformedBack);
        miter11 = untransformedBack + nextExt;
        miter12 = untransformedBack - nextExt;
      }

      if ((!addMiter || firstMiterLimitExceeded) && subPathIsClosed) {
        float d1, d2;
        vec2 p1, p2,
        firstNormal = firstMiter1 - firstMiter2, // unnormalized line normal for first edge
        second = next + vec2(firstNormal.y, -firstNormal.x); // approximated second point

        // calculate points to use for bevel
        // two points are possible for each edge - the one farthest away from the other line has to be used

        // calculate point for current edge
        d1 = distanceToLineSegmentSquared(miter12, next, second);
        d2 = distanceToLineSegmentSquared(miter11, next, second);
        p2 = (d1 > d2) ? miter12 : miter11;

        // calculate point for next edge
        d1 = distanceToLineSegmentSquared(firstMiter1, current, next);
        d2 = distanceToLineSegmentSquared(firstMiter2, current, next);
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
          vec2 capExt(nextExt.y, -nextExt.x);
          vec2 cap11 = miter11 + capExt;
          vec2 cap12 = miter12 + capExt;

          context->pushQuad(cap11, cap12, miter11, miter12, color, transform);
        }
        else {
          drawArcTo(context, next, miter11, miter12, color);
        }
      }

      if (sp == paths.end()) break;
    } // for each path
  }

}

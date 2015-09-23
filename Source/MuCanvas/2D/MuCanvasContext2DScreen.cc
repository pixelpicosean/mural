#include "MuOpenGL.h"

#include "MuCanvasContext2DScreen.h"
#include "AppController.h"

namespace mural {

  const GLfloat quad[] = {
    // Positions   // TexCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };

  // Generates a texture that is suited for attachments to a framebuffer
  GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil, int width, int height) {
    // What enum to use?
    GLenum attachment_type;
    if (!depth && !stencil)
      attachment_type = GL_RGB;
    else if (depth && !stencil)
      attachment_type = GL_DEPTH_COMPONENT;
    else if (!depth && stencil)
      attachment_type = GL_STENCIL_INDEX;

    // Generate texture ID and load texture data
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    if (!depth && !stencil)
      glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, width, height, 0, attachment_type, GL_UNSIGNED_BYTE, nullptr);
    else // Using both a stencil and depth test, needs special format arguments
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
  }

  MuCanvasContext2DScreen::MuCanvasContext2DScreen(short width, short height):
    MuCanvasContext2D(width, height),
    style(0.0f, 0.0f, width, height)
  {
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);

    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

    // // Position
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    // // Color
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    // glBindVertexArray(0);
  }

  MuCanvasContext2DScreen::~MuCanvasContext2DScreen() {
    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(1, &vbo);
  }

  void MuCanvasContext2DScreen::setStyle(MuRect newStyle) {
    if ((style.size.x ? style.size.x : width) != newStyle.size.x ||
      (style.size.y ? style.size.y : height) != newStyle.size.y) {
      // Must resize
      style = newStyle;

      // Only resize if we already have a viewFrameBuffer. Otherwise the style
      // will be honored in the 'create' call.
      // if (viewFrameBuffer) {
      //   resizeTo(width, height);
      // }
    }
    else {
      // Just reposition
      style = newStyle;
    }
  }

  MuRect MuCanvasContext2DScreen::getFrame() {
    return MuRect(
      style.origin.x, style.origin.y,
      (style.size.x ? style.size.x : width),
      (style.size.y ? style.size.y : height)
    );
  }

  void MuCanvasContext2DScreen::resizeTo(short newWidth, short newHeight) {
    flushBuffers();

    width = newWidth;
    height = newHeight;

    MuRect frame = getFrame();

    float contentScale = useRetinaResolution ? app.devicePixelRatio : 1;
    backingStoreRatio = (frame.size.x / (float)width) * contentScale;

    bufferWidth = frame.size.x * contentScale;
    bufferHeight = frame.size.y * contentScale;

    printf(
      "Creating ScreenCanvas (2D):\n"
        "  size:    %dx%d\n"
        "  style:   %.0fx%.0f\n"
        "  retina:  %s (%.0fx%.0f)\n\n",
      width, height,
      frame.size.x, frame.size.y,
      (useRetinaResolution ? "true" : "false"),
      frame.size.x * contentScale, frame.size.y * contentScale
    );

    // Create a color attachment texture
    // textureId = generateAttachmentTexture(false, false, bufferWidth, bufferHeight);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    // Flip the screen - OpenGL has the origin in the bottom left corner. We want the top left.
    upsideDown = true;
  }

  void MuCanvasContext2DScreen::present() {}

}

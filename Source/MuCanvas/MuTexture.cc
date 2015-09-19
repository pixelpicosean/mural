#include "MuTexture.h"
#include "MuSharedTextureCache.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace mural {

  GLuint getBytesPerPixel(GLenum format) {
    switch (format) {
      case GL_LUMINANCE:
      case GL_ALPHA:
        return 1;
      case GL_LUMINANCE_ALPHA:
        return 2;
      case GL_RGB:
        return 3;
      case GL_RGBA:
        return 4;
    }
    return 0;
  }

  std::string normalizePath(const GLchar *file) {
    std::string path("../assets");
    path += file;
    return path;
  }

  unsigned char *MuTexture::getPixels() {
    GLint boundFrameBuffer;
    GLuint tempFramebuffer;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &boundFrameBuffer );

    // If this texture doesn't have an FBO (i.e. its not used as the backing store
    // for an offscreen canvas2d), we have to create a new, temporary framebuffer
    // containing the texture. We can then read the pixel data using glReadPixels
    // as usual
    if (!fbo) {
      glGenFramebuffers(1, &tempFramebuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, tempFramebuffer);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureStorage->textureId, 0);
    }
    else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    int size = width * height * getBytesPerPixel(format);
    unsigned char *data = new unsigned char[size];
    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

    glBindFramebuffer(GL_FRAMEBUFFER, boundFrameBuffer);


    if (!fbo) {
      glDeleteFramebuffers(1, &tempFramebuffer);
    }

    return data;
  }

  double MuTexture::getLastUsed() {
    return textureStorage->lastBound;
  }
  GLuint MuTexture::getTextureId() {
    return textureStorage->textureId;
  }

  bool MuTexture::isDynamic() {
    return (bool)fbo;
  }

  GLint MuTexture::getParam(GLenum pname) {
    if (pname == GL_TEXTURE_MIN_FILTER) return params[kMuTextureParamMinFilter];
    if (pname == GL_TEXTURE_MAG_FILTER) return params[kMuTextureParamMagFilter];
    if (pname == GL_TEXTURE_WRAP_S) return params[kMuTextureParamWrapS];
    if (pname == GL_TEXTURE_WRAP_T) return params[kMuTextureParamWrapT];
    return 0;
  }

  void MuTexture::setParam(GLenum pname, GLenum param) {
    if (pname == GL_TEXTURE_MIN_FILTER) params[kMuTextureParamMinFilter] = (MuTextureParam)param;
    else if (pname == GL_TEXTURE_MAG_FILTER) params[kMuTextureParamMagFilter] = (MuTextureParam)param;
    else if (pname == GL_TEXTURE_WRAP_S) params[kMuTextureParamWrapS] = (MuTextureParam)param;
    else if (pname == GL_TEXTURE_WRAP_T) params[kMuTextureParamWrapT] = (MuTextureParam)param;
  }

  MuTexture::MuTexture(const char *path):
    contentScale(1.0f),
    fullPath(path)
  {
    unsigned char *data = loadPixelsFromPath(path);

    if (data) {
      createWithPixels(data, GL_RGBA);
    }
  }

  MuTexture::MuTexture(short width, short height, GLenum format):
    width(width),
    height(height),
    contentScale(1.0f),
    dimensionsKnown(true)
  {
    createWithPixels(nullptr, format);
  }

  MuTexture::MuTexture(short width, short height, unsigned char *pixels):
    width(width),
    height(height),
    contentScale(1.0f),
    dimensionsKnown(true)
  {
    createWithPixels(pixels, GL_RGBA);
  }

  MuTexture::MuTexture(short width, short height, GLuint fbo, float contentScale):
    width(width),
    height(height),
    contentScale(contentScale),
    dimensionsKnown(true),
    fbo(fbo)
  {
    createWithPixels(nullptr, GL_RGBA);
  }

  MuTexture::MuTexture(MuTexture const& other) {
    format = other.format;
    contentScale = other.contentScale;
    fullPath = other.fullPath;

    width = other.width;
    height = other.height;
    dimensionsKnown = other.dimensionsKnown;

    textureStorage = other.textureStorage;
  }

  MuTexture::~MuTexture() {
    if (cached) {
      theTextureCache.getTextures().erase(fullPath);
    }
  }

  void MuTexture::createWithPixels(unsigned char *pixels, GLenum format, GLenum target) {
    if (textureStorage) {
      textureStorage = nullptr;
    }

    // Set the default texture params for Canvas2D
    params[kMuTextureParamMinFilter] = (MuTextureParam)GL_LINEAR;
    params[kMuTextureParamMagFilter] = (MuTextureParam)GL_LINEAR;
    params[kMuTextureParamWrapS] = (MuTextureParam)GL_CLAMP_TO_EDGE;
    params[kMuTextureParamWrapT] = (MuTextureParam)GL_CLAMP_TO_EDGE;

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    if (width > maxTextureSize || height > maxTextureSize) {
      printf("Warning: Image %s larger than MAX_TEXTURE_SIZE (%d)", fullPath.empty() ? "[Dynamic]" : fullPath.c_str(), maxTextureSize);
      return;
    }
    this->format = format;

    GLint boundTexture = 0;
    GLenum bindingName = (target == GL_TEXTURE_2D) ?
      GL_TEXTURE_BINDING_2D :
      GL_TEXTURE_BINDING_CUBE_MAP;
    glGetIntegerv(bindingName, &boundTexture);

    textureStorage = new MuTextureStorage(true);
    textureStorage->bindToTarget(target, params);
    glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(target, boundTexture);

    this->pixels = pixels;
  }

  void MuTexture::updateWithPixels(unsigned char *pixels, int sx, int sy, int sw, int sh) {
    int boundTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);

    glBindTexture(GL_TEXTURE_2D, textureStorage->textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, sx, sy, sw, sh, format, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, boundTexture);
  }

  unsigned char *MuTexture::loadPixelsFromPath(const char *path) {
    // TODO: add DateURI support
    // TODO: add @2x texture support
    // TODO: accept images without alpha channel (jpg ...)
    int w, h, n;
    unsigned char *data = stbi_load(normalizePath(path).c_str(), &w, &h, &n, 4);
    if (data) {
      width = w;
      height = h;
      format = GL_RGBA;
      dimensionsKnown = true;
    }

    return data;
  }

  void MuTexture::bindWithFilter(GLenum filter) {
    params[kMuTextureParamMinFilter] = (MuTextureParam)filter;
    params[kMuTextureParamMagFilter] = (MuTextureParam)filter;
    textureStorage->bindToTarget(GL_TEXTURE_2D, params);
  }

  void MuTexture::bindToTarget(GLenum target) {
    textureStorage->bindToTarget(target, params);
  }

  void MuTexture::premultiplyPixels(const GLubyte *inPixels, GLubyte *outPixels, int byteLength, GLenum format) {
    const GLubyte *premultiplyTable = theTextureCache.getPremultiplyTable();

    if (format == GL_RGBA) {
      for (int i = 0; i < byteLength; i += 4) {
        unsigned short a = inPixels[i+3] * 256;
        outPixels[i+0] = premultiplyTable[ a + inPixels[i+0] ];
        outPixels[i+1] = premultiplyTable[ a + inPixels[i+1] ];
        outPixels[i+2] = premultiplyTable[ a + inPixels[i+2] ];
        outPixels[i+3] = inPixels[i+3];
      }
    }
    else if (format == GL_LUMINANCE_ALPHA) {
      for (int i = 0; i < byteLength; i += 2) {
        unsigned short a = inPixels[i+1] * 256;
        outPixels[i+0] = premultiplyTable[ a + inPixels[i+0] ];
        outPixels[i+1] = inPixels[i+1];
      }
    }
  }

  void MuTexture::unPremultiplyPixels(const GLubyte *inPixels, GLubyte *outPixels, int byteLength, GLenum format) {
    const GLubyte *unPremultiplyTable = theTextureCache.getUnPremultiplyTable();

    if (format == GL_RGBA) {
      for (int i = 0; i < byteLength; i += 4) {
        unsigned short a = inPixels[i+3] * 256;
        outPixels[i+0] = unPremultiplyTable[ a + inPixels[i+0] ];
        outPixels[i+1] = unPremultiplyTable[ a + inPixels[i+1] ];
        outPixels[i+2] = unPremultiplyTable[ a + inPixels[i+2] ];
        outPixels[i+3] = inPixels[i+3];
      }
    }
    else if (format == GL_LUMINANCE_ALPHA) {
      for (int i = 0; i < byteLength; i += 2) {
        unsigned short a = inPixels[i+1] * 256;
        outPixels[i+0] = unPremultiplyTable[ a + inPixels[i+0] ];
        outPixels[i+1] = inPixels[i+1];
      }
    }
  }

  void MuTexture::flipPixelsY(GLubyte *pixels, int bytesPerRow, int rows) {
    if (!pixels) { return; }

    GLuint middle = rows/2;
    GLuint intsPerRow = bytesPerRow / sizeof(GLuint);
    GLuint remainingBytes = bytesPerRow - intsPerRow * sizeof(GLuint);

    for (GLuint rowTop = 0, rowBottom = rows-1; rowTop < middle; rowTop++, rowBottom--) {
      // Swap bytes in packs of sizeof(GLuint) bytes
      GLuint *iTop = (GLuint *)(pixels + rowTop * bytesPerRow);
      GLuint *iBottom = (GLuint *)(pixels + rowBottom * bytesPerRow);

      GLuint itmp;
      GLint n = intsPerRow;
      do {
        itmp = *iTop;
        *iTop++ = *iBottom;
        *iBottom++ = itmp;
      } while(--n > 0);

      // Swap the remaining bytes
      GLubyte *bTop = (GLubyte *)iTop;
      GLubyte *bBottom = (GLubyte *)iBottom;

      GLubyte btmp;
      switch (remainingBytes) {
        case 3: btmp = *bTop; *bTop++ = *bBottom; *bBottom++ = btmp;
        case 2: btmp = *bTop; *bTop++ = *bBottom; *bBottom++ = btmp;
        case 1: btmp = *bTop; *bTop = *bBottom; *bBottom = btmp;
      }
    }
  }

}

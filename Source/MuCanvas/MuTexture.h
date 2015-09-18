#ifndef Mural_MuTexture_h
#define Mural_MuTexture_h

#include "MuTextureStorage.h"

#include <string>
#include <functional>

namespace mural {

  class MuTexture {
    public:
      // Properties
      bool drawFlippedY;

      GLenum format;
      short width, height;
      float contentScale;

      unsigned char *getPixels();
      double getLastUsed();
      GLuint getTextureId();

      bool isDynamic();

      GLint getParam(GLenum name);
      void setParam(GLenum name, GLenum param);

      // Ctors
      MuTexture(const char *path);

      MuTexture(short width, short height, GLenum format = GL_RGBA);
      MuTexture(short width, short height, unsigned char *pixels);
      MuTexture(short width, short height, GLuint fbo, float contentScale);

      MuTexture(MuTexture const& other);

      ~MuTexture();

      // Methods
      void createWithPixels(unsigned char *pixels, GLenum format, GLenum target = GL_TEXTURE_2D);
      void updateWithPixels(unsigned char *pixels, int x, int y, int subWidth, int subHeight);

      unsigned char *loadPixelsFromPath(const char *path);

      void bindWithFilter(GLenum filter);
      void bindToTarget(GLenum target);

      static void premultiplyPixels(const GLubyte *inPixels, GLubyte *outPixels, int byteLength, GLenum format);
      static void unPremultiplyPixels(const GLubyte *inPixels, GLubyte *outPixels, int byteLength, GLenum format);
      static void flipPixelsY(GLubyte *pixels, int bytesPerRow, int rows);

    private:
      void operator=(MuTexture const&) {}

      bool cached;
      bool dimensionsKnown;

      std::string fullPath;

      unsigned char *pixels;
      MuTextureStorage *textureStorage;
      GLuint fbo;
      MuTextureParams params;

      std::function<void()> loadCallback;
  };

}

#endif

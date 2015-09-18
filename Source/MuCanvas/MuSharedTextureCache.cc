#include "MuSharedTextureCache.h"

namespace mural {

  std::map<std::string, MuTexture *> MuSharedTextureCache::getTextures() {
    return textures;
  }

  unsigned char *MuSharedTextureCache::getPremultiplyTable() {
    if (!premultiplyTable) {
      premultiplyTable = new unsigned char[256*256];

      for (int a = 0; a <= 255; a++) {
        for (int c = 0; c <= 255; c++) {
          premultiplyTable[a*256+c] = (a * c + 254) / 255;
        }
      }
    }

    return premultiplyTable;
  }

  unsigned char *MuSharedTextureCache::getUnPremultiplyTable() {
    if (!unPremultiplyTable) {
      unPremultiplyTable = new unsigned char[256*256];

      // a == 0 case
      for (int c = 0; c <= 255; c++) {
        unPremultiplyTable[c] = c;
      }

      for (int a = 1; a <= 255; a++) {
        for (int c = 0; c <= 255; c++) {
          unPremultiplyTable[a*256+c] = (c * 255) / a;
        }
      }
    }

    return unPremultiplyTable;
  }

  MuSharedTextureCache::~MuSharedTextureCache() {
    delete[] premultiplyTable;
    delete[] unPremultiplyTable;

    for (auto it = textures.begin(); it != textures.end(); it++) {
      delete ((*it).second);
    }
    textures.clear();
  }

}

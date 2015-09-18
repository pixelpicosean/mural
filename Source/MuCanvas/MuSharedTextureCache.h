#ifndef Mural_MuSharedTextureCache
#define Mural_MuSharedTextureCache

#include <map>
#include <string>

namespace mural {

  class MuTexture;
  class MuSharedTextureCache {
    public:
      std::map<std::string, MuTexture *> getTextures();
      unsigned char *getPremultiplyTable();
      unsigned char *getUnPremultiplyTable();

      static MuSharedTextureCache& instance() {
        static MuSharedTextureCache instance;
        return instance;
      }

    private:
      MuSharedTextureCache() {}
      ~MuSharedTextureCache();
      MuSharedTextureCache(MuSharedTextureCache const&) {}
      void operator=(MuSharedTextureCache const&) {}

      std::map<std::string, MuTexture *> textures;
      unsigned char *premultiplyTable;
      unsigned char *unPremultiplyTable;
  };

}

#define theTextureCache mural::MuSharedTextureCache::instance()

#endif

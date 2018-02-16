// The MIT License (MIT)
//
// Copyright (c) 2014 Leif Erkenbrach
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "glStd.h"
#include <list>

class Texture;

class HeightmapCache {
  struct IDHeightmap {
    unsigned long long int ID;
    Texture *TexturePtr;

    IDHeightmap(const unsigned long long int &id, Texture *texture)
        : ID(id), TexturePtr(texture) {}
    ~IDHeightmap() {}
  };

public:
  HeightmapCache(int cacheSize, int textureResolution);
  ~HeightmapCache();

  void Initialize();

  int getActiveCount() const { return mActiveTextures.size(); }
  int getInactiveCount() const { return mFreeTextures.size(); }

  bool hasActiveTexture(const unsigned long long int &id);
  bool hasInactiveTexture(const unsigned long long int &id);
  bool unfreeInactiveTexture(const unsigned long long int &id);
  bool hasFreeTexture();

  Texture *getFreeTexture(const unsigned long long int &newId);
  Texture *getTexture(const unsigned long long int &id,
                      bool searchFreeTextures = true);

  void clearLastFetched();
  void freeTexture();
  bool freeTexture(const unsigned long long int &id);

private:
  // ID-Texture reference
  std::map<unsigned long long int, Texture *> mActiveTextures;
  std::list<IDHeightmap> mFreeTextures;
  std::list<unsigned long long int> mLastFetched;

  int mCacheSize;
  int mTexResolution;
};
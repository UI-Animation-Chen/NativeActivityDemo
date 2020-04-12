//
// Created by mtdp on 2020-04-11.
//

#ifndef NATIVEACTIVITYDEMO_ANDROIDASSETUTILS_H
#define NATIVEACTIVITYDEMO_ANDROIDASSETUTILS_H

#include <android/asset_manager.h>

class AndroidAssetUtils {
private:
    static AAssetManager *assetManager;
public:
    static void init(AAssetManager *assetMgr);
    static int openFdFromAsset(const char *assetName);
};

#endif //NATIVEACTIVITYDEMO_ANDROIDASSETUTILS_H

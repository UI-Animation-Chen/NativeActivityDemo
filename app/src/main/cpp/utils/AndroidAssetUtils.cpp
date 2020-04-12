//
// Created by mtdp on 2020-04-11.
//

#include <unistd.h>
#include "AndroidAssetUtils.h"

AAssetManager *AndroidAssetUtils::assetManager = NULL;

void AndroidAssetUtils::init(AAssetManager *assetMgr) {
    assetManager = assetMgr;
}

int AndroidAssetUtils::openFdFromAsset(const char *assetName) {
    AAsset *asset = AAssetManager_open(assetManager, assetName, AASSET_MODE_BUFFER);

    off_t start = 0, len = 0;
    int fd = AAsset_openFileDescriptor(asset, &start, &len);

    AAsset_close(asset);

    if (fd > 0) {
        lseek(fd, start, SEEK_CUR);// 通过asset方式打开的文件，流里的[start, start+len)区间属于该文件。
    }

    return fd;
}

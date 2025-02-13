#pragma once

#include <cstdio>
#include <nfd.h>
#include <string>

enum VKPT_FILE_TYPE {
    VKPT_SPHERE,
    VKPT_SCENE,
    VKPT_MESH,
    VKPT_HDRI,
    VKPT_TEXTURE
};

enum VKPT_FILE_ACTION {
    VKPT_SAVE,
    VKPT_LOAD
};

const std::string pickFilePath(VKPT_FILE_TYPE type, VKPT_FILE_ACTION);

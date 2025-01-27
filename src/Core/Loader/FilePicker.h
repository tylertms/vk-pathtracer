#pragma once

#include <nfd.h>
#include <cstdio>
#include <string>

enum VKPT_FILE_TYPE {
    VKPT_SCENE,
    VKPT_MODEL
};

enum VKPT_FILE_ACTION {
    VKPT_SAVE,
    VKPT_LOAD
};

std::string pickFilePath(VKPT_FILE_TYPE type, VKPT_FILE_ACTION);

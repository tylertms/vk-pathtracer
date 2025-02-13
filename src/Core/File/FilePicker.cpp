#include "FilePicker.h"

#include "nfd.h"

#include <stdexcept>

const std::string pickFilePath(VKPT_FILE_TYPE type, VKPT_FILE_ACTION action) {
    nfdu8char_t *outPath = nullptr;

    nfdu8filteritem_t filters[1];
    switch (type) {
    case VKPT_MESH:
        filters[0] = {"glTF 2.0", "gltf,glb"};
        break;
    case VKPT_SCENE:
        filters[0] = {"YAML", "yaml,yml"};
        break;
    case VKPT_HDRI:
        filters[0] = {"HDRI", "hdr"};
        break;
    case VKPT_TEXTURE:
        filters[0] = {"Image", "png,jpg,jpeg"};
        break;
    default:
        break;
    }

    nfdopendialogu8args_t openArgs = {0};
    openArgs.filterList = filters;
    openArgs.filterCount = 1;

    nfdsavedialogu8args_t saveArgs = {0};
    saveArgs.filterList = filters;
    saveArgs.defaultName = "Untitled.yaml";
    saveArgs.filterCount = 1;

    std::string path;

    nfdresult_t result;
    switch (action) {
    case VKPT_LOAD:
        result = NFD_OpenDialogU8_With(&outPath, &openArgs);
        break;
    case VKPT_SAVE:
        result = NFD_SaveDialogU8_With(&outPath, &saveArgs);
        break;
    }

    if (result == NFD_OKAY) {
        path = outPath;
        NFD_FreePathU8(outPath);
    } else if (result == NFD_CANCEL) {

    } else {
        std::string errorMsg = NFD_GetError();
        throw std::runtime_error("NFD Error: " + errorMsg);
    }

    return path;
}

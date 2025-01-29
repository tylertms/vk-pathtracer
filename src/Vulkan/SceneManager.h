#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "SceneUniform.h"

#include "../Core/Types/Scene.h"
#include "../Core/Utils/Matrix.h"
#include "../Core/Loader/GLTFLoader.h"

namespace Vulkan {

class SceneManager {
  public:

    void init(const Device &device);
    void deinit(const VkDevice &device);

    void resetAccumulation();
    bool resetOccurred();

    void addSphere();
    void addMesh(const std::string filename);

    inline const VkBuffer &getVkBuffer() const { return m_Buffer; }

    void submitUpdatesIfNeeded() {
        if (!m_StorageChanged) return;
        printf("SUBMITTING\n");
        m_StorageChanged = false;
        memcpy(m_BufferMapped, &sceneStorage, sizeof(sceneStorage));
    }

    SceneUniform sceneUniform;
    VKPT::StorageBuffer sceneStorage;
    std::vector<std::string> modelPaths;

  private:
    bool m_Reset = false;
    bool m_StorageChanged = false;

    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    void *m_BufferMapped;
};

} // namespace Vulkan

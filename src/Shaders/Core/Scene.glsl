#ifndef CORE_SCENE_GLSL
#define CORE_SCENE_GLSL

#include "../../Core/Types/AABB.h"

HitPayload rayHitScene(Ray ray) {
    HitPayload hit, temp;
    hit.didHit = false;
    hit.distance = 1.#INF;

    for (int i = 0; i < scene.numSpheres; i++) {
        temp = rayHitSphere(ray, spheres[i]);
        if (temp.didHit && temp.distance < hit.distance) {
            hit = temp;
            hit.material = spheres[i].material;
        }
    }

    for (int i = 0; i < scene.numMeshes; i++) {
        float hitAABB = rayHitAABB(ray, meshes[i].bounds);
        if (hitAABB >= hit.distance) continue;

        for (int t = 0; t < meshes[i].triangleCount; t++) {
            uint index = meshes[i].startIndex + t;
            temp = rayHitTriangle(ray, triangles[index]);
            if (temp.didHit && temp.distance < hit.distance)  {
                hit = temp;
                hit.material = meshes[i].material;
            }
        }
    }

    return hit;
}

#endif

#ifndef CORE_SCENE_GLSL
#define CORE_SCENE_GLSL

#include "../../Core/Types/AABB.h"

HitPayload rayHitScene(Ray worldRay) {
    HitPayload hit, temp;
    hit.didHit = false;
    hit.distance = 1.#INF;

    for (int i = 0; i < scene.numSpheres; i++) {
        temp = rayHitSphere(worldRay, scene.spheres[i]);
        if (temp.didHit && temp.distance < hit.distance) {
            hit = temp;
            hit.material = scene.spheres[i].material;
        }
    }

    for (int i = 0; i < scene.numMeshes; i++) {
        Ray localRay;
        localRay.origin = vec3(scene.meshes[i].worldLocalTransform * vec4(worldRay.origin, 1.f));
        localRay.dir = vec3(scene.meshes[i].worldLocalTransform * vec4(worldRay.dir, 0.f));
        localRay.inv = 1 / localRay.dir;

        float hitAABB = rayHitAABB(localRay, scene.meshes[i].bounds);
        if (hitAABB >= hit.distance) continue;

        for (int t = 0; t < scene.meshes[i].triangleCount; t++) {
            uint index = scene.meshes[i].startIndex + t;
            temp = rayHitTriangle(localRay, triangles[index]);
            if (temp.didHit && temp.distance < hit.distance)  {
                hit = temp;
                hit.point = worldRay.origin + worldRay.dir * temp.distance;
                hit.normal = normalize(vec3(scene.meshes[i].localWorldTransform * vec4(temp.normal, 0.f)));
                hit.material = scene.meshes[i].material;
            }
        }
    }

    return hit;
}

#endif

#ifndef CORE_SCENE_GLSL
#define CORE_SCENE_GLSL

#include "Common.glsl"
#include "Sphere.glsl"
#include "Triangle.glsl"
#include "Mesh.glsl"

HitPayload rayHitScene(Ray ray) {
    HitPayload hit, temp;
    hit.didHit = false;
    hit.distance = 1e20;

    for (int i = 0; i < scene.numSpheres; i++) {
        temp = rayHitSphere(ray, scene.spheres[i]);
        if (temp.didHit && temp.distance < hit.distance) {
            hit = temp;
            hit.material = scene.spheres[i].material;
        }
    }

    for (int i = 0; i < scene.numMeshes; i++) {
        for (int t = 0; t < scene.meshes[i].triangleCount; t++) {
            uint index = scene.meshes[i].startIndex + t;
            temp = rayHitTriangle(ray, scene.triangles[index]);
            if (temp.didHit && temp.distance < hit.distance)  {
                hit = temp;
                hit.material = scene.meshes[i].material;
            }
        }
    }

    return hit;
}

#endif

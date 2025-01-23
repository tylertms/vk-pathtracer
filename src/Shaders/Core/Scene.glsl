#include "Common.glsl"
#include "Sphere.glsl"

HitPayload rayHitScene(Ray ray) {
    HitPayload hit, temp;
    hit.didHit = false;
    hit.time = 1e20;

    for (int i = 0; i < scene.numSpheres; i++) {
        temp = rayHitSphere(ray, scene.spheres[i]);
        if (temp.didHit && temp.time < hit.time) {
            hit = temp;
            hit.material = scene.spheres[i].material;
        }
    }

    return hit;
}

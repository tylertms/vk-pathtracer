#ifndef CORE_SCENE_GLSL
#define CORE_SCENE_GLSL

HitPayload rayHitScene(Ray worldRay, inout uint stats[2]) {
    HitPayload hit, temp;
    hit.didHit = false;
    hit.normal = vec3(0);
    hit.distance = 1.#INF;

    for (int i = 0; i < scene.numSpheres; i++) {
        temp = rayHitSphere(worldRay, scene.spheres[i]);
        if (temp.didHit && temp.distance < hit.distance) {
            hit = temp;
            hit.material = scene.materials[scene.spheres[i].materialIndex];
        }
    }

    for (int i = 0; i < scene.numMeshes; i++) {
        Mesh mesh = scene.meshes[i];

        Ray localRay;
        localRay.origin = vec3(mesh.worldLocalTransform * vec4(worldRay.origin, 1.));
        localRay.dir = vec3(mesh.worldLocalTransform * vec4(worldRay.dir, 0.));
        localRay.inv = 1 / localRay.dir;

        uint nodeStack[BVH_MAX_DEPTH];
        uint nodeIndex = 0;

        nodeStack[nodeIndex++] = mesh.rootBVHNode;

        while (nodeIndex > 0) {
            BVH node = bvhs[nodeStack[--nodeIndex]];
            bool isLeaf = node.triangleCount > 0;

            if (isLeaf) {
                for (uint t = 0; t < node.triangleCount; t++) {
                    uint index = node.index + t;
                    temp = rayHitTriangle(localRay, triangles[index]);
                    stats[1]++;

                    if (temp.didHit && temp.distance < hit.distance) {
                        hit.didHit = true;
                        hit.distance = temp.distance;
                        hit.point = worldRay.origin + worldRay.dir * temp.distance;
                        hit.normal = normalize(vec3(mesh.localWorldTransform * vec4(temp.normal, 0.)));
                        hit.material = scene.materials[mesh.materialIndex];
                    }
                }
            } else {
                uint leftChildIndex = node.index;
                uint rightChildIndex = node.index + 1;

                BVH leftChild = bvhs[leftChildIndex];
                BVH rightChild = bvhs[rightChildIndex];

                float leftDist = rayHitAABB(localRay, leftChild.boundsMin, leftChild.boundsMax);
                float rightDist = rayHitAABB(localRay, rightChild.boundsMin, rightChild.boundsMax);

                stats[0] += 2;

                bool isLeftNearest = leftDist <= rightDist;
                float distNear = isLeftNearest ? leftDist : rightDist;
                float distFar = isLeftNearest ? rightDist : leftDist;
                uint nearChildIndex = isLeftNearest ? leftChildIndex : rightChildIndex;
                uint farChildIndex = isLeftNearest ? rightChildIndex : leftChildIndex;

                if (distFar < hit.distance)
                    nodeStack[nodeIndex++] = farChildIndex;
                if (distNear < hit.distance)
                    nodeStack[nodeIndex++] = nearChildIndex;
            }
        }
    }

    return hit;
}

#endif

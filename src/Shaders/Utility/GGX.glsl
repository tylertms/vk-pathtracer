#ifndef UTILITY_GGX
#define UTILITY_GGX

// Sample microfacet normal using the GGX/Trowbridge-Reitz distribution.
vec3 sampleGGX(vec3 N, float roughness, inout uint state) {
    float a = roughness * roughness;
    
    float u1 = rand(state);
    float u2 = rand(state);

    float phi = 2.0 * PI * u2;
    
    float cosTheta = sqrt((1.0 - u1) / (1.0 + (a * a - 1.0) * u1));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
    
    vec3 H_tangent = vec3(
        sinTheta * cos(phi),
        sinTheta * sin(phi),
        cosTheta
    );
    
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 H = tangent * H_tangent.x + bitangent * H_tangent.y + N * H_tangent.z;
    
    return normalize(H);
}


#endif

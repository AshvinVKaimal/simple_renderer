#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render(int spp, char* sample);

    Vector3f uniformSampleHemi(float r1, float r2) {
        float theta = acosf(r1);
        float phi = 2 * M_PI * r2;
        
        return Vector3f(sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta));
    }

    Vector3f cosineSampleHemi(float r1, float r2) {
        float theta = r1;
        float phi = 2 * M_PI * r2;

        return Vector3f(sqrtf(theta) * cosf(phi), sqrtf(theta) * sinf(phi), sqrtf(std::max(0.f, 1.f - theta)));
    }

    Scene scene;
    Texture outputImage;
};
#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int spp, char* sample)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int x = 0; x < this->scene.imageResolution.x; x++) {
        for (int y = 0; y < this->scene.imageResolution.y; y++) {
            Vector3f result(0, 0, 0);

            for (int s = 0; s < spp; s++) {
                float offsetX = next_float() - 0.5f;
                float offsetY = next_float() - 0.5f;
                float sampleX = x + offsetX;
                float sampleY = y + offsetY;

                Ray cameraRay = this->scene.camera.generateRay(sampleX, sampleY);
                Interaction si = this->scene.rayIntersect(cameraRay);

                if (si.didIntersect) {
                    Vector3f radiance;
                    LightSample ls;

                    if (*sample == 'uniform') {                 // Uniform Hemisphere Sampling
                        Vector3f dir_local = uniformSampleHemi(next_float(), next_float());
                        Vector3f dir_world = Normalize(si.toWorld(dir_local));

                        Ray sampleRay(si.p + 1e-3f * si.n, dir_world);
                        Interaction siSample = this->scene.rayEmitterIntersect(sampleRay);
                        Interaction siShadowSample = this->scene.rayIntersect(sampleRay);
                            
                        if (siSample.didIntersect && (!siShadowSample.didIntersect || siShadowSample.t > siSample.t))
                            result += si.bsdf->eval(&si, dir_local) * siSample.emissiveColor * std::abs(Dot(si.n, dir_world)) * (2.f * M_PI);

                        for (Light &light : this->scene.lights) {
                            if (light.type != AREA_LIGHT) {
                                std::tie(radiance, ls) = light.sample(&si);

                                Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                Interaction siShadow = this->scene.rayIntersect(shadowRay);

                                if (!siShadow.didIntersect || siShadow.t > ls.d) {
                                    result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                                }
                            }
                        }
                    }

                    else if (*sample == 'cosine') {             // Cosine Weighted Sampling
                        Vector3f dir_local = cosineSampleHemi(next_float(), next_float());
                        Vector3f dir_world = Normalize(si.toWorld(dir_local));

                        Ray sampleRay(si.p + 1e-3f * si.n, dir_world);
                        Interaction siSample = this->scene.rayEmitterIntersect(sampleRay);
                        Interaction siShadowSample = this->scene.rayIntersect(sampleRay);
                            
                        if (siSample.didIntersect && (!siShadowSample.didIntersect || siShadowSample.t > siSample.t))
                            result += si.bsdf->eval(&si, dir_local) * siSample.emissiveColor * std::abs(Dot(si.n, dir_world)) * (2.f * M_PI);

                        for (Light &light : this->scene.lights) {
                            if (light.type != AREA_LIGHT) {
                                std::tie(radiance, ls) = light.sample(&si);

                                Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                Interaction siShadow = this->scene.rayIntersect(shadowRay);

                                if (!siShadow.didIntersect || siShadow.t > ls.d) {
                                    result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                                }
                            }
                        }
                    }

                    else if (*sample == 'importance') {         // Importance Light Sampling
                        int lightNo = next_float() * this->scene.lights.size();
                        Light &light = this->scene.lights[lightNo];

                        std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        if (!siShadow.didIntersect || siShadow.t > ls.d) {
                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                        }
                    }

                }

                for (Light &light : this->scene.lights) {
                    Interaction siEmissive = light.intersectLight(&cameraRay);
                    if (siEmissive.didIntersect) {
                        result += siEmissive.emissiveColor;
                    }
                }
            }

            result /= spp;
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 5) {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    auto renderTime = rayTracer.render(spp, argv[4]);
    
    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}

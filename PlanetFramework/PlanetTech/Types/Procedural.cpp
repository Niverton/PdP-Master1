#include "Procedural.h"
#include "Simplex.h"
#include "Texture.h"
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

ProceduralPlanet::ProceduralPlanet(Properties *properties) {
  unsigned int width = properties->width;
  unsigned int height = properties->height;
  m_Radius = properties->radius;
  m_MaxHeight = properties->maxHeight;
  Noise n = properties->noise;

  std::function<double(glm::vec3)> noise_maker;
  /* Added NOLINT to prevent clang-tidy from flagging suspicious but
   * legit static_casts
   */
  switch (n) {
  case Noise::SIMPLEX:
  default:
    noise_maker = [](const glm::vec3 &p) { return glm::simplex(p); };
    break;
  case Noise::PERLIN:
    noise_maker = [](const glm::vec3 &p) { return glm::perlin(p); };
    break;
  case Noise::RIDGED_NOISE:
    noise_maker = [](const glm::vec3 &p) { return Simplex::ridgedNoise(p); };
    break;
  case Noise::FLOW_NOISE: {
    auto prop = static_cast<FlowNoiseProperties *>(properties); // NOLINT
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::flowNoise(p, prop->angle);
    };
  } break;
  case Noise::FBM:
    /* Fall Through*/
  case Noise::FBM_VARIATION: {
    auto prop = static_cast<FbmProperties *>(properties); // NOLINT
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::fBm(p, prop->octave, prop->lacunarity, prop->gain);
    };
  } break;
  case Noise::WARPED_FBM: {
    auto prop = static_cast<FbmProperties *>(properties); // NOLINT
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::fBm(
          Simplex::fBm(p, prop->octave, prop->lacunarity, prop->gain),
          prop->octave, prop->lacunarity, prop->gain);
    };
  } break;
  case Noise::DFBM_WARPED_FBM: {
    auto prop = static_cast<FbmProperties *>(properties); // NOLINT
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::fBm(
          Simplex::dfBm(p, prop->octave, prop->lacunarity, prop->gain),
          prop->octave, prop->lacunarity, prop->gain);
    };
  } break;

  case Noise::RIDGED_MULTI_FRACTAL_VARIATION: {
    auto prop = static_cast<RidgedMultiFractalVariationProperties *>( // NOLINT
        properties);
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::ridgedMF(p, prop->ridgeOffset, prop->octave,
                               prop->lacunarity, prop->gain);
    };
  } break;
  case Noise::WARPED_RIDGED_MULTI_FRACTAL: {
    auto prop = static_cast<WarpedRidgedMultiFractalProperties *>( // NOLINT
        properties);
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::ridgedMF(
          Simplex::ridgedMF(p, prop->ridgeOffset, prop->octave,
                            prop->lacunarity, prop->gain),
          prop->ridgeOffset, prop->octave, prop->lacunarity, prop->gain);
    };
  } break;
  case Noise::RIDGED_MULTI_FRACTAL_VARIATION2: {
    auto prop = static_cast<RidgedMultiFractalVariationProperties *>( // NOLINT
        properties);
    noise_maker = [&](const glm::vec3 &p) {
      return Simplex::ridgedMF(p, prop->ridgeOffset, prop->octave,
                               prop->lacunarity, prop->gain);
    };
  } break;
  }

  m_data = new float[height * width];
  for (unsigned int i = 0; i < height * width; i++) {
    double u = std::fmod(double(i), double(width));
    double v = std::floor(double(i) / double(width));
    glm::vec3 p{std::cos(u) * std::cos(v) * m_Radius,
                std::sin(u) * std::cos(v) * m_Radius, std::sin(v) * m_Radius};
    m_data[i] = noise_maker(p);
  }

  m_pHeight = new Texture(m_data, width, height);
  // m_pDiffuse = new Texture(m_data, width, height);
  m_pDiffuse = new Texture("./Textures/moon8k.jpg");
}

ProceduralPlanet::~ProceduralPlanet() {
  delete[] m_data;
}

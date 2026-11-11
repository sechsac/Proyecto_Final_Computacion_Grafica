#pragma once

#include <string>
#include <cmath>
#include <glm/glm.hpp>

struct Hotspot {
    glm::vec3   posicion;
    float       radio;
    std::string titulo;
    std::string texto;
    std::string accion;

    float distancia(const glm::vec3& camara, const glm::vec3& origenZona) const {
        glm::vec3 p = origenZona + posicion;
        float dx = camara.x - p.x;
        float dz = camara.z - p.z;
        return std::sqrt(dx * dx + dz * dz);
    }
};

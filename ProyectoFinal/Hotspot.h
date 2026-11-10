#pragma once
// =====================================================================
//  Hotspot.h  -  Punto de información cultural
// ---------------------------------------------------------------------
//  Un hotspot es un punto invisible en el escenario. Cuando la cámara
//  se acerca a menos de "radio" metros, se muestra un aviso y el usuario
//  puede presionar I para leer la información cultural.
// =====================================================================
#include <string>
#include <cmath>
#include <glm/glm.hpp>

struct Hotspot {
    glm::vec3   posicion;  // relativa al origen de la zona
    float       radio;     // en metros
    std::string titulo;
    std::string texto;
    std::string accion;    // texto extra opcional, ej. "E: abrir el biombo"

    // Distancia en el plano del piso (ignora la altura de la cámara)
    float distancia(const glm::vec3& camara, const glm::vec3& origenZona) const {
        glm::vec3 p = origenZona + posicion;
        float dx = camara.x - p.x;
        float dz = camara.z - p.z;
        return std::sqrt(dx * dx + dz * dz);
    }
};

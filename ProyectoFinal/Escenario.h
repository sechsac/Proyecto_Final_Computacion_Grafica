#pragma once
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"

class Escenario {
public:
    void cargar() {
        for (const char* nombre : { "arq_casa.obj", "arq_patio.obj", "arq_muro.obj", "arq_porton.obj", "arq_jardin.obj" })
            agregar(RUTA_ARQ + nombre, { colocar(glm::vec3(0.0f), 0.0f, 1.0f) });

        agregar(RUTA_ENT + "pagoda.obj",          { colocar(glm::vec3( 6.5f, 0.55f, -6.5f), 0.0f, 1.0f) });
        agregar(RUTA_ENT + "pino.obj",            { colocar(glm::vec3( 4.3f, 0.0f, -7.8f),   0.0f, 1.0f),
                                                    colocar(glm::vec3( 8.9f, 0.0f, -5.0f),  70.0f, 0.85f),
                                                    colocar(glm::vec3(-9.4f, 0.0f, -7.5f), 140.0f, 1.1f),
                                                    colocar(glm::vec3(-4.0f, 0.0f, -7.8f), 200.0f, 0.9f) });
        agregar(RUTA_ENT + "roca.obj",            { colocar(glm::vec3( 5.2f, 0.0f, -5.0f),   0.0f, 1.0f),
                                                    colocar(glm::vec3( 8.0f, 0.0f, -7.9f),  95.0f, 0.7f),
                                                    colocar(glm::vec3( 4.6f, 0.0f, -6.4f), 210.0f, 0.6f),
                                                    colocar(glm::vec3( 8.6f, 0.0f, -4.6f), 300.0f, 0.5f) });
        agregar(RUTA_ENT + "linterna_piedra.obj", { colocar(glm::vec3( 5.0f, 0.0f, -4.4f),  20.0f, 1.0f),
                                                    colocar(glm::vec3(-0.4f, 0.0f,  4.6f),   0.0f, 1.0f) });
        agregar(RUTA_ENT + "maceta.obj",          { colocar(glm::vec3(-2.2f, 0.0f,  2.7f),   0.0f, 1.0f),
                                                    colocar(glm::vec3( 2.2f, 0.0f,  2.7f),  45.0f, 1.0f),
                                                    colocar(glm::vec3(-3.8f, 0.0f,  2.7f),  90.0f, 1.0f),
                                                    colocar(glm::vec3(-8.2f, 0.0f,  2.7f),  10.0f, 1.0f) });
    }

    void dibujar(Shader& shader, GLint modelLoc) {
        for (Elemento& e : elementos) {
            for (const glm::mat4& m : e.matrices) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
                e.modelo->Draw(shader);
            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    struct Elemento {
        std::unique_ptr<Model> modelo;
        std::vector<glm::mat4> matrices;
    };
    const std::string RUTA_ARQ = "Models/arq/";
    const std::string RUTA_ENT = "Models/entorno/";
    std::vector<Elemento> elementos;

    static glm::mat4 colocar(const glm::vec3& pos, float rotY, float escala) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), pos);
        m = glm::rotate(m, glm::radians(rotY), glm::vec3(0, 1, 0));
        return glm::scale(m, glm::vec3(escala));
    }

    void agregar(const std::string& ruta, const std::vector<glm::mat4>& matrices) {
        if (!std::ifstream(ruta).good()) { std::cout << "[Escenario] Aun no existe: " << ruta << " (se omite)\n"; return; }
        Elemento e;
        e.modelo = std::make_unique<Model>((GLchar*)ruta.c_str());
        e.matrices = matrices;
        elementos.push_back(std::move(e));
    }
};

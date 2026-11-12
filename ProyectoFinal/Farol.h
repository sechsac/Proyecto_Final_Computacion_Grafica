#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"

class Farol {
public:
    glm::vec3 posicion = glm::vec3(0.0f);
    float rotacionY = 0.0f;

    void cargar() {
        poste = cargar1("farol_poste.obj");
        cuerpo = cargar1("farol_cuerpo.obj");
        vela = cargar1("farol_vela.obj");
    }

    void actualizar(float dt, const glm::vec3& camara, const glm::mat4& baseZona) {
        tiempo += dt;
        tImpulso += dt;
        glm::mat4 mGancho = matrizGancho(baseZona);
        glm::vec3 g = glm::vec3(mGancho[3]);
        float d = glm::length(glm::vec2(camara.x - g.x, camara.z - g.z));
        bool ahoraCerca = d < RADIO_PASO;
        if (ahoraCerca && !cerca) impulso();
        cerca = ahoraCerca;
        amplitud *= std::exp(-dt / AMORTIGUAMIENTO);
        float w = 2.0f * 3.14159265f / PERIODO;
        anguloZ = amplitud * std::sin(w * tImpulso) + 2.0f * std::sin(0.7f * tiempo);
        anguloX = 0.35f * amplitud * std::sin(w * tImpulso + 1.2f) + 1.5f * std::sin(0.53f * tiempo + 0.8f);
        brillo = 0.85f + 0.10f * std::sin(17.0f * tiempo) * std::sin(7.3f * tiempo) + 0.05f * std::sin(29.0f * tiempo);
        mVelaCache = matrizVela(baseZona);
    }

    void impulso() { amplitud = 24.0f; tImpulso = 0.0f; }

    glm::vec3 posLuz() const { return glm::vec3(mVelaCache * glm::vec4(0.0f, 0.115f, 0.0f, 1.0f)); }
    float intensidad() const { return brillo; }

    void dibujar(Shader& shader, GLint modelLoc, const glm::mat4& baseZona) {
        auto pintar = [&](const std::unique_ptr<Model>& m, const glm::mat4& matriz) {
            if (!m) return;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matriz));
            m->Draw(shader);
        };
        glm::mat4 mPoste = matrizPoste(baseZona);
        pintar(poste, mPoste);
        glm::mat4 mCuerpo = matrizCuerpo(baseZona);
        pintar(cuerpo, mCuerpo);
        float llama = 1.0f + 0.08f * std::sin(23.0f * tiempo) * std::sin(9.0f * tiempo);
        pintar(vela, glm::scale(matrizVela(baseZona), glm::vec3(1.0f, llama, 1.0f)));
    }

private:
    const std::string RUTA = "Models/farol/";
    const glm::vec3 GANCHO = glm::vec3(0.45f, 1.93f, 0.0f);
    const float ALTURA_VELA = -0.62f;
    const float RADIO_PASO = 1.6f;
    const float PERIODO = 1.9f;
    const float AMORTIGUAMIENTO = 4.0f;

    std::unique_ptr<Model> poste, cuerpo, vela;
    float tiempo = 0.0f, tImpulso = 100.0f, amplitud = 0.0f;
    float anguloZ = 0.0f, anguloX = 0.0f, brillo = 1.0f;
    bool cerca = false;
    glm::mat4 mVelaCache = glm::mat4(1.0f);

    glm::mat4 matrizPoste(const glm::mat4& base) const {
        glm::mat4 m = glm::translate(base, posicion);
        return glm::rotate(m, glm::radians(rotacionY), glm::vec3(0, 1, 0));
    }
    glm::mat4 matrizGancho(const glm::mat4& base) const {
        return glm::translate(matrizPoste(base), GANCHO);
    }
    glm::mat4 matrizCuerpo(const glm::mat4& base) const {
        glm::mat4 m = matrizGancho(base);
        m = glm::rotate(m, glm::radians(anguloZ), glm::vec3(0, 0, 1));
        return glm::rotate(m, glm::radians(anguloX), glm::vec3(1, 0, 0));
    }
    glm::mat4 matrizVela(const glm::mat4& base) const {
        glm::mat4 m = glm::translate(matrizCuerpo(base), glm::vec3(0.0f, ALTURA_VELA, 0.0f));
        m = glm::rotate(m, glm::radians(-0.6f * anguloZ), glm::vec3(0, 0, 1));
        return glm::rotate(m, glm::radians(-0.6f * anguloX), glm::vec3(1, 0, 0));
    }
    std::unique_ptr<Model> cargar1(const std::string& archivo) {
        std::string ruta = RUTA + archivo;
        if (!std::ifstream(ruta).good()) { std::cout << "[Farol] Falta " << ruta << "\n"; return nullptr; }
        return std::make_unique<Model>((GLchar*)ruta.c_str());
    }
};

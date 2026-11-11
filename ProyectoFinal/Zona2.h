#pragma once

#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"
#include "Animator.h"
#include "Hotspot.h"
#include "imgui/imgui.h"

namespace z2 {
const std::string RUTA      = "Models/z2/";
const std::string RUTA_PERS = "Models/z3/";

const glm::vec3 POS_MESA(0.0f, 0.0f, -0.75f);
const float     ALTURA_MESA = 0.35f;
const float     CUENCO_X[] = { -0.30f, 0.0f, 0.30f };
const glm::vec3 POS_ESCOBA(0.0f, ALTURA_MESA, -0.93f);
const glm::vec3 POS_YEONGDON(-1.05f, 0.0f, -0.75f);
const glm::vec3 POS_JIJEON(0.0f, 0.0f, -1.60f);
const glm::vec3 POS_MUDANG(0.0f, 0.0f, 0.25f);

const float     ALTURA_CADERA = 0.95f;
const glm::vec3 CINTURA   ( 0.00f,  0.10f, 0.0f);
const glm::vec3 CUELLO    ( 0.00f,  0.50f, 0.0f);
const glm::vec3 HOMBRO_IZQ(-0.20f,  0.45f, 0.0f);
const glm::vec3 HOMBRO_DER( 0.20f,  0.45f, 0.0f);
const glm::vec3 CODO      ( 0.00f, -0.30f, 0.0f);
const glm::vec3 MANO      ( 0.00f, -0.28f, 0.0f);
const glm::vec3 CADERA_IZQ(-0.10f, -0.05f, 0.0f);
const glm::vec3 CADERA_DER( 0.10f, -0.05f, 0.0f);
const glm::vec3 RODILLA   ( 0.00f, -0.45f, 0.0f);

const int   SEGMENTOS = 8;
const float LARGO_SEGMENTO = 0.22f;

enum { BAJADA_Y = 0, V_TORSO, V_HOMBRO, V_CODO, V_MUSLO, V_RODILLA, V_TELA };

const float DIST_RITUAL = 3.0f;

inline glm::mat4 rotX(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(1, 0, 0)); }
inline glm::mat4 rotY(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(0, 1, 0)); }
inline float distanciaPiso(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(glm::vec2(a.x - b.x, a.z - b.z));
}
}

class Zona2 {
public:
    glm::vec3 origen    = glm::vec3(6.0f, 0.0f, 0.0f);
    float     rotacionY = 0.0f;

    void cargar() {
        using namespace z2;
        mesa       = cargarModelo(RUTA, "z2_mesa_baja.obj");
        cuencos[0] = cargarModelo(RUTA, "z2_cuenco_artemisa.obj");
        cuencos[1] = cargarModelo(RUTA, "z2_cuenco_fragante.obj");
        cuencos[2] = cargarModelo(RUTA, "z2_cuenco_clara.obj");
        escoba     = cargarModelo(RUTA, "z2_escoba.obj");
        yeongdon   = cargarModelo(RUTA, "z2_yeongdon.obj");
        jijeon     = cargarModelo(RUTA, "z2_jijeon.obj");
        nobi       = cargarModelo(RUTA, "z2_nobi_segmento.obj");

        cadera       = cargarModelo(RUTA_PERS, "z3_doliente_cadera.obj");
        torso        = cargarModelo(RUTA_PERS, "z3_doliente_torso.obj");
        cabeza       = cargarModelo(RUTA_PERS, "z3_doliente_cabeza.obj");
        brazoIzq     = cargarModelo(RUTA_PERS, "z3_doliente_brazo_izq.obj");
        brazoDer     = cargarModelo(RUTA_PERS, "z3_doliente_brazo_der.obj");
        antebrazoIzq = cargarModelo(RUTA_PERS, "z3_doliente_antebrazo_izq.obj");
        antebrazoDer = cargarModelo(RUTA_PERS, "z3_doliente_antebrazo_der.obj");
        musloIzq     = cargarModelo(RUTA_PERS, "z3_doliente_muslo_izq.obj");
        musloDer     = cargarModelo(RUTA_PERS, "z3_doliente_muslo_der.obj");
        piernaIzq    = cargarModelo(RUTA_PERS, "z3_doliente_pierna_izq.obj");
        piernaDer    = cargarModelo(RUTA_PERS, "z3_doliente_pierna_der.obj");

        texTraje = cargarTextura(RUTA, "z2_traje_mudang.png");
        texCara  = cargarTextura(RUTA, "z2_cara_mudang.png");

        animGilkkut.agregar(  0, { 0.00f,  5,   20,   40,   0,    0,           0,   0,   0,   0, -70, -20,   0,   0 });
        animGilkkut.agregar( 45, { 0.00f,  0,  165,    0,   0,    0,           0,   0,   0,   0,   0,   0,   0,   0 });
        animGilkkut.agregar( 90, {-0.02f, 25,   75,   15,  10,   25,         -75,  25, -25,  25, -25,  25, -25,  25 });
        animGilkkut.agregar(110, {-0.02f, 25,   85,   10,  10,   25,         -85, -25,  25, -25,  25, -25,  25, -25 });
        animGilkkut.agregar(130, {-0.02f, 25,   75,   15,  10,   25,         -75,  25, -25,  25, -25,  25, -25,  25 });
        animGilkkut.agregar(150, {-0.08f, 35,   55,    0,  25,   50,         -45, -40,   0,   0,   0,   0,   0,   0 });
        animGilkkut.agregar(200, { 0.00f,  5,   20,   40,   0,    0,           0,   0,   0,   0, -70, -20,   0,   0 });

        hotspots.push_back({ glm::vec3(0.0f, 0.0f, 1.6f), 1.4f,
            "Ssitkim-gut: la purificación del alma",
            "El Ssitkim-gut es un ritual chamánico (Musok), tradicional en regiones como la isla "
            "de Jindo. La chamana (mudang) lava simbólicamente el alma del difunto para disolver "
            "sus rencores y pesares (Han), de modo que pueda partir en paz hacia el otro mundo. "
            "La figura enrollada en una estera (yeongdon) representa al difunto y contiene sus "
            "ropas; sobre ella se coloca un cuenco que guarda su alma.",
            "E: ver el Gilkkut" });

        hotspots.push_back({ glm::vec3(-0.9f, 0.0f, 0.3f), 0.8f,
            "Las tres aguas y la escobilla",
            "La mudang purifica al difunto con tres aguas: agua de artemisa, agua perfumada y "
            "agua clara. Las aplica con una escobilla, en un gesto que simboliza lavar las "
            "impurezas y los resentimientos que el alma pudo acumular en vida.",
            "" });

        hotspots.push_back({ glm::vec3(0.9f, 0.0f, 0.3f), 0.8f,
            "La tela Nobi y las tiras Jijeon",
            "La larga tela blanca de algodón simboliza el camino que el alma recorre hacia el "
            "otro mundo. En el Gilkkut, la mudang la despliega y la ondea; con el Gordipuli "
            "deshace los nudos que representan los pesares del difunto. Las tiras de papel "
            "blanco (Jijeon) que cuelgan detrás adornan y consagran el espacio ritual.",
            "E: ver el Gilkkut" });
    }

    void actualizar(float deltaTime, const glm::vec3& posCamara) {
        camara = posCamara;
        animGilkkut.actualizar(deltaTime);
        int anterior = hotspotCercano;
        hotspotCercano = -1;
        float mejor = 1e9f;
        for (int i = 0; i < (int)hotspots.size(); i++) {
            float d = hotspots[i].distancia(camara, origen);
            if (d < hotspots[i].radio && d < mejor) { mejor = d; hotspotCercano = i; }
        }
        if (hotspotCercano != anterior) infoAbierta = false;
    }

    void tecla(int key) {
        using namespace z2;
        bool cerca = distanciaPiso(camara, origen) < DIST_RITUAL;
        if (key == GLFW_KEY_I && hotspotCercano >= 0) infoAbierta = !infoAbierta;
        if (key == GLFW_KEY_E && cerca && !animGilkkut.reproduciendo()) animGilkkut.reproducir();
    }

    void dibujar(Shader& shader, GLint modelLoc) {
        using namespace z2;
        auto pintar = [&](const ModeloPtr& m, const glm::mat4& matriz, GLuint tex) {
            if (!m) return;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matriz));
            m->Draw(shader);
        };
        glm::mat4 base = rotY(glm::translate(glm::mat4(1.0f), origen), rotacionY);

        glm::mat4 mMesa = glm::translate(base, POS_MESA);
        pintar(mesa, mMesa, 0);
        for (int i = 0; i < 3; i++)
            pintar(cuencos[i], glm::translate(mMesa, glm::vec3(CUENCO_X[i], ALTURA_MESA, 0.03f)), 0);
        pintar(escoba, glm::translate(base, POS_ESCOBA), 0);
        pintar(yeongdon, glm::translate(base, POS_YEONGDON), 0);
        pintar(jijeon, glm::translate(base, POS_JIJEON), 0);

        float torsoA = animGilkkut.valor(V_TORSO);
        float hombro = animGilkkut.valor(V_HOMBRO);
        float codo   = animGilkkut.valor(V_CODO);
        float muslo  = animGilkkut.valor(V_MUSLO);
        float rodilla = animGilkkut.valor(V_RODILLA);

        glm::mat4 mCadera = glm::translate(base,
            POS_MUDANG + glm::vec3(0.0f, ALTURA_CADERA + animGilkkut.valor(BAJADA_Y), 0.0f));
        mCadera = rotY(mCadera, 180.0f);
        pintar(cadera, mCadera, texTraje);
        glm::mat4 mTorso = rotX(glm::translate(mCadera, CINTURA), torsoA);
        pintar(torso, mTorso, texTraje);
        pintar(cabeza, glm::translate(mTorso, CUELLO), texCara);

        glm::mat4 mBI = rotX(glm::translate(mTorso, HOMBRO_IZQ), -hombro);
        pintar(brazoIzq, mBI, texTraje);
        pintar(antebrazoIzq, rotX(glm::translate(mBI, CODO), -codo), texTraje);
        glm::mat4 mBD = rotX(glm::translate(mTorso, HOMBRO_DER), -hombro);
        pintar(brazoDer, mBD, texTraje);
        glm::mat4 mABD = rotX(glm::translate(mBD, CODO), -codo);
        pintar(antebrazoDer, mABD, texTraje);

        glm::mat4 mMI = rotX(glm::translate(mCadera, CADERA_IZQ), -muslo);
        pintar(musloIzq, mMI, texTraje);
        pintar(piernaIzq, rotX(glm::translate(mMI, RODILLA), rodilla), texTraje);
        glm::mat4 mMD = rotX(glm::translate(mCadera, CADERA_DER), -muslo);
        pintar(musloDer, mMD, texTraje);
        pintar(piernaDer, rotX(glm::translate(mMD, RODILLA), rodilla), texTraje);

        float anguloBrazo = torsoA - hombro - codo;
        glm::mat4 mSeg = glm::translate(mABD, MANO);
        for (int i = 0; i < SEGMENTOS; i++) {
            float a = animGilkkut.valor(V_TELA + i);
            if (i == 0) a -= anguloBrazo;
            mSeg = rotX(mSeg, a);
            pintar(nobi, mSeg, 0);
            mSeg = glm::translate(mSeg, glm::vec3(0.0f, -LARGO_SEGMENTO, 0.0f));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void dibujarUI() {
        if (hotspotCercano < 0) return;
        const Hotspot& h = hotspots[hotspotCercano];
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y - 30.0f),
                                ImGuiCond_Always, ImVec2(0.5f, 1.0f));
        ImGui::SetNextWindowSize(ImVec2(infoAbierta ? 640.0f : 420.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.82f);
        ImGui::Begin("##hotspotZona2", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                     ImGuiWindowFlags_NoInputs);
        ImGui::TextColored(ImVec4(0.95f, 0.80f, 0.55f, 1.0f), "%s", h.titulo.c_str());
        ImGui::Separator();
        if (infoAbierta) {
            ImGui::TextWrapped("%s", h.texto.c_str());
            ImGui::Spacing();
            ImGui::TextDisabled("I: cerrar");
        } else {
            ImGui::TextDisabled("I: leer más");
        }
        if (!h.accion.empty()) ImGui::TextDisabled("%s", h.accion.c_str());
        ImGui::End();
    }

private:
    using ModeloPtr = std::unique_ptr<Model>;
    ModeloPtr mesa, cuencos[3], escoba, yeongdon, jijeon, nobi;
    ModeloPtr cadera, torso, cabeza, brazoIzq, brazoDer, antebrazoIzq, antebrazoDer;
    ModeloPtr musloIzq, musloDer, piernaIzq, piernaDer;
    GLuint texTraje = 0, texCara = 0;

    Animator animGilkkut;
    std::vector<Hotspot> hotspots;
    int  hotspotCercano = -1;
    bool infoAbierta = false;
    glm::vec3 camara = glm::vec3(0.0f);

    static bool existe(const std::string& ruta) { return std::ifstream(ruta).good(); }
    static ModeloPtr cargarModelo(const std::string& carpeta, const std::string& archivo) {
        std::string ruta = carpeta + archivo;
        if (!existe(ruta)) { std::cout << "[Zona2] Aun no existe: " << ruta << " (se omite)\n"; return nullptr; }
        return std::make_unique<Model>((GLchar*)ruta.c_str());
    }
    static GLuint cargarTextura(const std::string& carpeta, const std::string& archivo) {
        if (!existe(carpeta + archivo)) { std::cout << "[Zona2] Falta la textura " << archivo << "\n"; return 0; }
        return (GLuint)TextureFromFile(archivo.c_str(), carpeta.substr(0, carpeta.size() - 1));
    }
};

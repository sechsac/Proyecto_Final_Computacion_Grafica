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
#include "Farol.h"
#include "imgui/imgui.h"

namespace z1 {
const std::string RUTA      = "Models/z1/";
const std::string RUTA_PERS = "Models/z3/";

const float     ALTURA_MESA_ENTRADA = 0.75f;
const float     ALTURA_MESA_ALTAR   = 0.80f;
const glm::vec3 POS_MESA_ENTRADA(0.0f, 0.0f, 0.0f);
const glm::vec3 POS_LIBRO(-0.20f, ALTURA_MESA_ENTRADA, 0.08f);
const glm::vec3 POS_PLUMA(-0.36f, ALTURA_MESA_ENTRADA, 0.10f);
const glm::vec3 POS_PLATO(0.09f, ALTURA_MESA_ENTRADA, 0.07f);
const glm::vec3 POS_VISITANTE(0.0f, 0.0f, 0.55f);

const glm::vec3 POS_MESA_ALTAR(0.0f, 0.0f, -2.0f);
const glm::vec3 POS_FOTO(0.0f, ALTURA_MESA_ALTAR, -2.12f);
const glm::vec3 POS_CRISANTEMO[] = { glm::vec3(-0.38f, ALTURA_MESA_ALTAR, -2.05f), glm::vec3(0.38f, ALTURA_MESA_ALTAR, -2.05f) };
const glm::vec3 POS_VELA[]       = { glm::vec3(-0.58f, ALTURA_MESA_ALTAR, -1.95f), glm::vec3(0.58f, ALTURA_MESA_ALTAR, -1.95f) };
const glm::vec3 POS_VASIJA(0.0f, ALTURA_MESA_ALTAR, -1.85f);
const glm::vec3 POS_VASO_INCIENSO(0.20f, ALTURA_MESA_ALTAR, -1.85f);
const glm::vec3 POS_INCIENSO(-0.20f, ALTURA_MESA_ALTAR, -1.85f);
const glm::vec3 POS_BOUQUET[] = { glm::vec3(-1.15f, 0.0f, -2.0f), glm::vec3(1.15f, 0.0f, -2.0f) };
const glm::vec3 POS_TAPETE(0.0f, 0.0f, -1.05f);
const glm::vec3 POS_CEREZO(-3.40f, 0.0f, 3.40f);
const glm::vec3 POS_LINTERNA(1.90f, 1.80f, -2.35f);
const glm::vec3 POS_FAROL(2.00f, 0.0f, 1.20f);

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

enum { V_GIRO = 0, V_TORSO, V_CABEZA, V_HOMBRO, V_CODO };

const float FRAME_TOMA_SOBRE  = 95.0f;
const float FRAME_DEJA_SOBRE  = 120.0f;
const float DIST_REGISTRO = 3.0f;

inline glm::mat4 rotX(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(1, 0, 0)); }
inline glm::mat4 rotY(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(0, 1, 0)); }
inline float distanciaPiso(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(glm::vec2(a.x - b.x, a.z - b.z));
}
}

class Zona1 {
public:
    glm::vec3 origen    = glm::vec3(-6.0f, 0.0f, 0.0f);
    float     rotacionY = 0.0f;
    Farol     farol;

    void cargar() {
        using namespace z1;
        mesaEntrada = cargarModelo(RUTA, "z1_mesa_entrada.obj");
        libro       = cargarModelo(RUTA, "z1_libro.obj");
        pluma       = cargarModelo(RUTA, "z1_pluma.obj");
        plato       = cargarModelo(RUTA, "z1_plato_bronce.obj");
        sobre       = cargarModelo(RUTA, "z1_sobre.obj");
        mesaAltar   = cargarModelo(RUTA, "z1_mesa_altar.obj");
        foto        = cargarModelo(RUTA, "z1_fotografia.obj");
        crisantemo  = cargarModelo(RUTA, "z1_crisantemo.obj");
        velaAltar   = cargarModelo(RUTA, "z1_vela.obj");
        vasija      = cargarModelo(RUTA, "z1_vasija.obj");
        vasoIncienso= cargarModelo(RUTA, "z1_vaso_incienso.obj");
        incienso    = cargarModelo(RUTA, "z1_incienso.obj");
        bouquet     = cargarModelo(RUTA, "z1_bouquet.obj");
        tapete      = cargarModelo(RUTA, "z1_tapete.obj");
        cerezo      = cargarModelo(RUTA, "z1_cerezo.obj");
        linterna    = cargarModelo(RUTA, "z1_linterna.obj");

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

        texTraje  = cargarTextura(RUTA, "z1_traje_negro.png");
        texCara   = cargarTextura(RUTA_PERS, "cara_doliente.png");
        texMadera = cargarTextura(RUTA, "z1_madera.png");
        texClaro  = cargarTextura(RUTA, "z1_claro.png");

        farol.posicion = POS_FAROL;
        farol.cargar();

        animRegistro.agregar(  0, {   0,  0,  0,   0,   0 });
        animRegistro.agregar( 25, {   0, 45, 10,  58,   0 });
        animRegistro.agregar( 40, {   0, 45, 15,  55,   0 });
        animRegistro.agregar( 50, {   0, 45, 15,  61,  12 });
        animRegistro.agregar( 60, {   0, 45, 15,  55,   0 });
        animRegistro.agregar( 70, {   0, 45, 15,  61,  12 });
        animRegistro.agregar( 80, {   0, 45, 15,  55,   0 });
        animRegistro.agregar( 95, {   0,  5,  5, -24,  98 });
        animRegistro.agregar(115, { -35, 45, 10,  58,   0 });
        animRegistro.agregar(125, { -35, 40, 10,  50,   0 });
        animRegistro.agregar(145, {   0, 15, 25,   0,   0 });
        animRegistro.agregar(170, {   0,  0,  0,   0,   0 });

        hotspots.push_back({ glm::vec3(0.0f, 0.0f, 1.6f), 1.4f,
            "Samyang-jang: el registro de condolencias",
            "Al llegar al velatorio, los visitantes escriben su nombre en el libro de registro "
            "(Bangmyeongnok) para que la familia sepa quién acompañó al difunto. Después entregan "
            "el Bujo-geum: dinero en un sobre blanco que ayuda a la familia con los gastos del "
            "funeral, como muestra de solidaridad. Los visitantes visten de negro o de colores "
            "oscuros en señal de respeto.",
            "E: ver el registro" });

        hotspots.push_back({ POS_TAPETE + glm::vec3(0.0f, 0.0f, 0.4f), 0.9f,
            "El altar memorial",
            "En la sala memorial se coloca el retrato del difunto, rodeado de crisantemos blancos, "
            "la flor del luto en Corea. Los visitantes ofrecen una flor (Heonhwa) o encienden "
            "incienso (Bunhyang) y hacen una reverencia antes de dar el pésame a la familia.",
            "" });

        hotspots.push_back({ POS_FAROL + glm::vec3(-0.3f, 0.0f, 0.5f), 0.9f,
            "Farol Cheongsa chorong",
            "Farol tradicional de seda, azul en la parte superior y rojo en la inferior, con una "
            "vela en su interior. Se usaba para iluminar el camino de los visitantes durante la "
            "noche. Oscila suavemente cuando alguien pasa junto a él.",
            "" });
    }

    void actualizar(float deltaTime, const glm::vec3& posCamara) {
        camara = posCamara;
        animRegistro.actualizar(deltaTime);
        farol.actualizar(deltaTime, posCamara, base());
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
        using namespace z1;
        if (key == GLFW_KEY_I && hotspotCercano >= 0) infoAbierta = !infoAbierta;
        if (key == GLFW_KEY_E && distanciaPiso(camara, origen) < DIST_REGISTRO && !animRegistro.reproduciendo())
            animRegistro.reproducir();
    }

    void dibujar(Shader& shader, GLint modelLoc) {
        using namespace z1;
        auto pintar = [&](const ModeloPtr& m, const glm::mat4& matriz, GLuint tex) {
            if (!m) return;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matriz));
            m->Draw(shader);
        };
        glm::mat4 b = base();
        auto en = [&](const glm::vec3& p) { return glm::translate(b, p); };

        pintar(mesaEntrada, glm::scale(en(POS_MESA_ENTRADA), glm::vec3(0.8f, 1.0f, 0.8f)), texMadera);
        pintar(libro, en(POS_LIBRO), texClaro);
        pintar(pluma, en(POS_PLUMA), texMadera);
        pintar(plato, en(POS_PLATO), 0);
        pintar(mesaAltar, en(POS_MESA_ALTAR), texMadera);
        pintar(foto, en(POS_FOTO), texClaro);
        for (const glm::vec3& p : POS_CRISANTEMO) pintar(crisantemo, en(p), texClaro);
        for (const glm::vec3& p : POS_VELA) pintar(velaAltar, en(p), texClaro);
        pintar(vasija, en(POS_VASIJA), texMadera);
        pintar(vasoIncienso, en(POS_VASO_INCIENSO), texMadera);
        pintar(incienso, en(POS_INCIENSO), texMadera);
        for (const glm::vec3& p : POS_BOUQUET) pintar(bouquet, en(p), texClaro);
        pintar(tapete, en(POS_TAPETE), texClaro);
        pintar(cerezo, en(POS_CEREZO), texMadera);
        pintar(linterna, en(POS_LINTERNA), texMadera);

        float giro   = animRegistro.valor(V_GIRO);
        float torsoA = animRegistro.valor(V_TORSO);
        float cab    = animRegistro.valor(V_CABEZA);
        float hombro = animRegistro.valor(V_HOMBRO);
        float codo   = animRegistro.valor(V_CODO);

        glm::mat4 mCadera = rotY(en(POS_VISITANTE + glm::vec3(0.0f, ALTURA_CADERA, 0.0f)), 180.0f + giro);
        pintar(cadera, mCadera, texTraje);
        glm::mat4 mTorso = rotX(glm::translate(mCadera, CINTURA), torsoA);
        pintar(torso, mTorso, texTraje);
        pintar(cabeza, rotX(glm::translate(mTorso, CUELLO), cab), texCara);

        glm::mat4 mBI = glm::translate(mTorso, HOMBRO_IZQ);
        pintar(brazoIzq, mBI, texTraje);
        pintar(antebrazoIzq, glm::translate(mBI, CODO), texTraje);
        glm::mat4 mBD = rotX(glm::translate(mTorso, HOMBRO_DER), -hombro);
        pintar(brazoDer, mBD, texTraje);
        glm::mat4 mABD = rotX(glm::translate(mBD, CODO), -codo);
        pintar(antebrazoDer, mABD, texTraje);

        glm::mat4 mMI = glm::translate(mCadera, CADERA_IZQ);
        pintar(musloIzq, mMI, texTraje);
        pintar(piernaIzq, glm::translate(mMI, RODILLA), texTraje);
        glm::mat4 mMD = glm::translate(mCadera, CADERA_DER);
        pintar(musloDer, mMD, texTraje);
        pintar(piernaDer, glm::translate(mMD, RODILLA), texTraje);

        float f = animRegistro.frame();
        if (f >= FRAME_TOMA_SOBRE && f < FRAME_DEJA_SOBRE)
            pintar(sobre, rotX(glm::translate(mABD, MANO), 90.0f), 0);
        else if (f >= FRAME_DEJA_SOBRE)
            pintar(sobre, en(POS_PLATO + glm::vec3(0.0f, 0.006f, 0.0f)), 0);

        farol.dibujar(shader, modelLoc, b);
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
        ImGui::Begin("##hotspotZona1", nullptr,
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

    glm::mat4 base() const {
        return z1::rotY(glm::translate(glm::mat4(1.0f), origen), rotacionY);
    }

private:
    using ModeloPtr = std::unique_ptr<Model>;
    ModeloPtr mesaEntrada, libro, pluma, plato, sobre, mesaAltar, foto, crisantemo, velaAltar;
    ModeloPtr vasija, vasoIncienso, incienso, bouquet, tapete, cerezo, linterna;
    ModeloPtr cadera, torso, cabeza, brazoIzq, brazoDer, antebrazoIzq, antebrazoDer;
    ModeloPtr musloIzq, musloDer, piernaIzq, piernaDer;
    GLuint texTraje = 0, texCara = 0, texMadera = 0, texClaro = 0;

    Animator animRegistro;
    std::vector<Hotspot> hotspots;
    int  hotspotCercano = -1;
    bool infoAbierta = false;
    glm::vec3 camara = glm::vec3(0.0f);

    static bool existe(const std::string& ruta) { return std::ifstream(ruta).good(); }
    static ModeloPtr cargarModelo(const std::string& carpeta, const std::string& archivo) {
        std::string ruta = carpeta + archivo;
        if (!existe(ruta)) { std::cout << "[Zona1] Aun no existe: " << ruta << " (se omite)\n"; return nullptr; }
        return std::make_unique<Model>((GLchar*)ruta.c_str());
    }
    static GLuint cargarTextura(const std::string& carpeta, const std::string& archivo) {
        if (!existe(carpeta + archivo)) { std::cout << "[Zona1] Falta la textura " << archivo << "\n"; return 0; }
        return (GLuint)TextureFromFile(archivo.c_str(), carpeta.substr(0, carpeta.size() - 1));
    }
};

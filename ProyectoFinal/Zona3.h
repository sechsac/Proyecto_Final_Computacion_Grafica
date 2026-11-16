#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
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

namespace z3 {

const std::string RUTA = "Models/z3/";

const float ALTURA_MESA = 0.70f;

const float JEGI_Z = 0.15f;
const float JEGI_X[] = { -0.40f, -0.20f, 0.0f, 0.20f, 0.40f };
const int   NUM_JEGI = 5;
const float ALTURA_PLATO_JEGI = 0.084f;
const float ESCALA_MANZANA = 0.7f;

const glm::vec3 POS_COFRE(0.0f, ALTURA_MESA, -0.25f);
const glm::vec3 BISAGRA_IZQ(-0.08f, 0.0f, 0.06f);
const glm::vec3 BISAGRA_DER( 0.08f, 0.0f, 0.06f);
const float APERTURA_PUERTAS = 110.0f;

const glm::vec3 POS_BIOMBO(-1.20f, 0.0f, -0.90f);
const float ANCHO_PANEL = 0.45f;
const int   NUM_PANELES = 6;
const float PLEGADO = 175.0f;
const float ABIERTO = 30.0f;

const glm::vec3 POS_DOLIENTE(0.0f, 0.0f, 1.10f);
const float     ALTURA_CADERA = 0.95f;

const glm::vec3 CINTURA   ( 0.00f,  0.10f, 0.0f);
const glm::vec3 CUELLO    ( 0.00f,  0.50f, 0.0f);
const glm::vec3 HOMBRO_IZQ(-0.20f,  0.45f, 0.0f);
const glm::vec3 HOMBRO_DER( 0.20f,  0.45f, 0.0f);
const glm::vec3 CODO      ( 0.00f, -0.30f, 0.0f);
const glm::vec3 CADERA_IZQ(-0.10f, -0.05f, 0.0f);
const glm::vec3 CADERA_DER( 0.10f, -0.05f, 0.0f);
const glm::vec3 RODILLA   ( 0.00f, -0.45f, 0.0f);

enum { PUERTA_IZQ = 5, PUERTA_DER = 6 };
enum { BAJADA_Y = 0, V_TORSO, V_CABEZA, V_HOMBRO, V_CODO, V_MUSLO, V_RODILLA };

const float DIST_ALTAR = 2.5f;

// ---------- Modo misión ----------
const float FIN_ALTAR = 120.0f;           // último keyframe de animAltar
enum PasoMision { PASO_REVELAR = 0, PASO_MESA, PASO_JEOL, PASO_FIN };
enum TipoAviso  { AVISO_INFO = 0, AVISO_BIEN, AVISO_ERROR };

struct Pregunta {
    const char* texto;
    const char* opciones[3];
    int         correcta;
    const char* explicacion;
    const char* pista;
};

const Pregunta PREGUNTAS[] = {
    { "Según la regla Hongdong-baekseo, ¿de qué lado de la mesa van las frutas rojas?",
      { "Al este", "Al oeste", "Al centro" }, 0,
      "Correcto: frutas rojas al este (hong-dong) y blancas al oeste (baek-seo).",
      "Pista: hong = rojo, dong = este." },
    { "Según Banseo-gaengdong, ¿dónde se coloca el tazón de arroz?",
      { "Al este", "Al oeste", "Junto al pescado" }, 1,
      "Correcto: el arroz (ban) va al oeste (seo) y la sopa (gaeng) al este (dong).",
      "Pista: ban = arroz, seo = oeste." },
    { "¿Cuál de estos alimentos se evita en la mesa del Jesa?",
      { "La castaña", "La pera", "El durazno" }, 2,
      "Correcto: se evitan el durazno, el ajo y el chile, porque se cree que ahuyentan a los espíritus.",
      "Pista: la castaña y la pera sí aparecen en la regla Jo-yul-i-si." },
};
const int NUM_PREGUNTAS = 3;
const float DIST_COJIN = 2.0f;

inline glm::mat4 rotX(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(1, 0, 0)); }
inline glm::mat4 rotY(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(0, 1, 0)); }
inline float distanciaPiso(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(glm::vec2(a.x - b.x, a.z - b.z));
}
}

class Zona3 {
public:

    glm::vec3 origen    = glm::vec3(0.0f);
    float     rotacionY = 0.0f;

    void cargar() {
        using namespace z3;

        mesa      = cargarModelo("z3_jesasang.obj");
        jegi      = cargarModelo("z3_jegi.obj");
        manzana   = cargarModelo("z3_manzana.obj");

        pera       = cargarModelo("z3_pera.obj");
        caqui      = cargarModelo("z3_caqui.obj");
        castana    = cargarModelo("z3_castana.obj");
        azufaifa   = cargarModelo("z3_azufaifa.obj");
        pescado    = cargarModelo("z3_pescado.obj");
        platoLargo = cargarModelo("z3_plato_largo.obj");
        tteok      = cargarModelo("z3_tteok.obj");
        tazonArroz = cargarModelo("z3_tazon_arroz.obj");
        tazonSopa  = cargarModelo("z3_tazon_sopa.obj");
        copa       = cargarModelo("z3_copa.obj");
        cojin     = cargarModelo("z3_cojin.obj");
        incensario= cargarModelo("z3_incensario.obj");
        panel     = cargarModelo("z3_biombo_panel.obj");
        for (int i = 0; i < NUM_PANELES; i++)
            paneles[i] = cargarModelo("z3_biombo_panel_" + std::to_string(i + 1) + ".obj");
        cofre     = cargarModelo("z3_cofre.obj");
        puertaIzq = cargarModelo("z3_cofre_puerta_izq.obj");
        puertaDer = cargarModelo("z3_cofre_puerta_der.obj");
        tableta   = cargarModelo("z3_tableta.obj");

        cadera       = cargarModelo("z3_doliente_cadera.obj");
        torso        = cargarModelo("z3_doliente_torso.obj");
        cabeza       = cargarModelo("z3_doliente_cabeza.obj");
        brazoIzq     = cargarModelo("z3_doliente_brazo_izq.obj");
        brazoDer     = cargarModelo("z3_doliente_brazo_der.obj");
        antebrazoIzq = cargarModelo("z3_doliente_antebrazo_izq.obj");
        antebrazoDer = cargarModelo("z3_doliente_antebrazo_der.obj");
        musloIzq     = cargarModelo("z3_doliente_muslo_izq.obj");
        musloDer     = cargarModelo("z3_doliente_muslo_der.obj");
        piernaIzq    = cargarModelo("z3_doliente_pierna_izq.obj");
        piernaDer    = cargarModelo("z3_doliente_pierna_der.obj");

        texMaderaOscura = cargarTextura("madera_oscura.png");
        texMaderaMesa   = cargarTextura("madera_mesa.png");
        texMaderaClara  = cargarTextura("madera_clara.png");
        texLaca         = cargarTextura("laca_roja.png");
        texPapel        = cargarTextura("papel_hanji.png");
        texTela         = cargarTextura("tela_canamo.png");
        texCara         = cargarTextura("cara_doliente.png");
        texEstera       = cargarTextura("estera_paja.png");
        texCastana      = cargarTextura("castana.png");
        texAzufaifa     = cargarTextura("azufaifa.png");
        texPescado      = cargarTextura("pescado.png");
        texLaton        = cargarTextura("laton.png");
        texCaqui        = cargarTextura("caqui_piel.png");
        texTazon        = cargarTextura("porcelana_celadon.png");
        texTteok        = cargarTextura("garaetteok.png");

        const float P = PLEGADO, A = ABIERTO, D = APERTURA_PUERTAS;

        animAltar.agregar(  0, { P,   P,   P,   P,   P,   0.0f,     0.0f    });
        animAltar.agregar( 24, { A,   P,   P,   P,   P,   D * 0.2f, D * 0.2f });
        animAltar.agregar( 48, { A,   A,   P,   P,   P,   D * 0.4f, D * 0.4f });
        animAltar.agregar( 72, { A,   A,   A,   P,   P,   D * 0.6f, D * 0.6f });
        animAltar.agregar( 96, { A,   A,   A,   A,   P,   D * 0.8f, D * 0.8f });
        animAltar.agregar(120, { A,   A,   A,   A,   A,   D,        D        });

        animJeol.agregar(  0, { 0.00f,   0,    0,     0,    0,   0,    0 });
        animJeol.agregar( 45, { 0.00f,   0,    0,    80,   70,   0,    0 });
        animJeol.agregar( 90, {-0.45f,   0,    0,    80,   70,   0,   90 });
        animJeol.agregar(135, {-0.60f,  85,   10,   150,   20,  70,  160 });
        animJeol.agregar(180, {-0.45f,   0,    0,    80,   70,   0,   90 });
        animJeol.agregar(225, { 0.00f,   0,    0,     0,    0,   0,    0 });

        hotspots.push_back({ glm::vec3(0.0f, 0.0f, 2.6f), 1.4f,
            "Jesa: el culto a los ancestros",
            "El Jesa es la ceremonia confuciana con la que las familias coreanas honran a sus "
            "ancestros: en el aniversario luctuoso (Gijesa) o en festividades como Seollal y "
            "Chuseok (Charye). Expresa la piedad filial (hyo) y mantiene el vínculo entre los "
            "vivos y los antepasados del linaje.",
            "E: preparar el altar (abrir biombo y cofre)" });

        hotspots.push_back({ glm::vec3(-0.9f, 0.0f, 0.8f), 0.9f,
            "La mesa ritual (Jesasang)",
            "Tradicionalmente la mesa se orienta hacia el norte y sigue reglas de acomodo. "
            "Al frente van las frutas: Jo-yul-i-si (azufaifa, castaña, pera y caqui de oeste "
            "a este) y Hongdong-baekseo (frutas rojas al este, blancas al oeste). En medio, "
            "el pescado va al este con la cabeza hacia el este (Eodong-yukseo, Dudong-mihseo). "
            "Atrás, el arroz va al oeste y la sopa al este (Banseo-gaengdong), junto a la copa "
            "de licor que se ofrece al ancestro. Se evitan el ajo, el chile y el durazno.",
            "" });

        hotspots.push_back({ glm::vec3(0.9f, 0.0f, 0.8f), 0.9f,
            "Biombo (Byeongpung) y tableta ancestral",
            "Detrás de la mesa se coloca un biombo plegable que delimita el espacio ritual. Al "
            "centro se coloca la tableta del ancestro: un sinju de madera, guardado en su cofre "
            "(sinjujip), o un jibang de papel con su nombre, que se quema al terminar el rito.",
            "E: abrir / cerrar el biombo y el cofre" });

        hotspots.push_back({ z3::POS_DOLIENTE + glm::vec3(0.0f, 0.0f, 0.7f), 0.8f,
            "La reverencia profunda (Jeol)",
            "Frente al altar, los familiares se arrodillan e inclinan hasta casi tocar el piso "
            "en señal de respeto. Durante el Jesa la reverencia suele repetirse dos veces, "
            "acompañando los ofrecimientos de licor al ancestro.",
            "F: ver la reverencia" });
    }

    void actualizar(float deltaTime, const glm::vec3& posCamara) {
        camara = posCamara;
        animAltar.actualizar(deltaTime);
        bool jeolAntes = animJeol.reproduciendo();
        animJeol.actualizar(deltaTime);
        actualizarMision(deltaTime, jeolAntes && !animJeol.reproduciendo());

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
        using namespace z3;
        if (key == GLFW_KEY_G) { alternarMision(); return; }
        if (key == GLFW_KEY_I && hotspotCercano >= 0)
            infoAbierta = !infoAbierta;

        bool cercaAltar = distanciaPiso(camara, origen) < DIST_ALTAR + 1.0f;
        bool cercaCojin = distanciaPiso(camara, origen + POS_DOLIENTE) < DIST_COJIN;

        if (mision.activa && mision.paso != PASO_FIN) {
            teclaMision(key, cercaAltar, cercaCojin);
            return;
        }
        if (key == GLFW_KEY_E && cercaAltar)
            animAltar.alternar();
        if (key == GLFW_KEY_F && cercaCojin && !animJeol.reproduciendo())
            animJeol.reproducir();
    }

    void dibujar(Shader& shader, GLint modelLoc) {
        using namespace z3;

        auto pintar = [&](const ModeloPtr& m, const glm::mat4& matriz, GLuint tex) {
            if (!m) return;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matriz));
            m->Draw(shader);
        };

        glm::mat4 base = glm::translate(glm::mat4(1.0f), origen);
        base = rotY(base, rotacionY);

        pintar(mesa, base, texMaderaMesa);

        for (int i = 0; i < NUM_JEGI; i++) {
            glm::mat4 mJegi = glm::translate(base, glm::vec3(JEGI_X[i], ALTURA_MESA, JEGI_Z));
            pintar(jegi, glm::scale(mJegi, glm::vec3(0.8f)), texLaca);
            glm::mat4 mPlato = glm::translate(mJegi, glm::vec3(0.0f, ALTURA_PLATO_JEGI, 0.0f));
            switch (i) {
            case 0: dibujarMonton(pintar, mPlato, azufaifa, texAzufaifa, 0.023f, 0.024f); break;
            case 1: dibujarMonton(pintar, mPlato, castana,  texCastana,  0.026f, 0.018f); break;
            case 2: dibujarPiramide(pintar, mPlato, pera,    0,        0.056f, 0.060f, 1.0f); break;
            case 3: dibujarPiramide(pintar, mPlato, caqui,   texCaqui, 0.070f, 0.040f, 1.0f); break;
            case 4: dibujarPiramide(pintar, mPlato, manzana, 0,        0.060f, 0.045f, ESCALA_MANZANA); break;
            }
        }

        glm::mat4 mPlatoLargo = glm::translate(base, glm::vec3(0.30f, ALTURA_MESA, -0.02f));
        pintar(platoLargo, mPlatoLargo, texLaca);
        pintar(pescado, glm::translate(mPlatoLargo, glm::vec3(0.0f, 0.06f, 0.0f)), texPescado);
        glm::mat4 mTteok = glm::translate(base, glm::vec3(-0.30f, ALTURA_MESA, -0.02f));
        pintar(jegi, glm::scale(mTteok, glm::vec3(0.8f)), texLaca);
        pintar(tteok, glm::translate(mTteok, glm::vec3(0.0f, ALTURA_PLATO_JEGI, 0.0f)), texTteok);

        pintar(tazonArroz, glm::translate(base, glm::vec3(-0.24f, ALTURA_MESA, -0.24f)), texTazon);
        pintar(tazonSopa,  glm::translate(base, glm::vec3( 0.24f, ALTURA_MESA, -0.24f)), texTazon);
        pintar(copa,       glm::translate(base, glm::vec3( 0.00f, ALTURA_MESA, -0.05f)), texLaton);

        pintar(incensario, glm::translate(base, glm::vec3(0.0f, ALTURA_MESA, 0.28f)), texMaderaOscura);

        glm::mat4 mCofre = glm::translate(base, POS_COFRE);
        pintar(cofre, mCofre, texMaderaOscura);
        pintar(tableta, mCofre, texMaderaClara);
        pintar(puertaIzq, rotY(glm::translate(mCofre, BISAGRA_IZQ), -animAltar.valor(PUERTA_IZQ)), texMaderaOscura);
        pintar(puertaDer, rotY(glm::translate(mCofre, BISAGRA_DER),  animAltar.valor(PUERTA_DER)), texMaderaOscura);

        glm::mat4 mPanel = glm::translate(base, POS_BIOMBO);
        pintar(panelDe(0), mPanel, texPapel);
        for (int i = 1; i < NUM_PANELES; i++) {
            float signo = (i % 2 == 1) ? -1.0f : 1.0f;
            mPanel = glm::translate(mPanel, glm::vec3(ANCHO_PANEL, 0.0f, 0.0f));
            mPanel = rotY(mPanel, signo * animAltar.valor(i - 1));
            pintar(panelDe(i), mPanel, texPapel);
        }
        glm::mat4 mCadera = glm::translate(base,
            POS_DOLIENTE + glm::vec3(0.0f, ALTURA_CADERA + animJeol.valor(BAJADA_Y), 0.0f));
        mCadera = rotY(mCadera, 180.0f);
        pintar(cadera, mCadera, texTela);

        glm::mat4 mTorso = rotX(glm::translate(mCadera, CINTURA), animJeol.valor(V_TORSO));
        pintar(torso, mTorso, texTela);
        pintar(cabeza, rotX(glm::translate(mTorso, CUELLO), animJeol.valor(V_CABEZA)), texCara);

        glm::mat4 mBI = rotX(glm::translate(mTorso, HOMBRO_IZQ), -animJeol.valor(V_HOMBRO));
        pintar(brazoIzq, mBI, texTela);
        pintar(antebrazoIzq, rotX(glm::translate(mBI, CODO), -animJeol.valor(V_CODO)), texTela);
        glm::mat4 mBD = rotX(glm::translate(mTorso, HOMBRO_DER), -animJeol.valor(V_HOMBRO));
        pintar(brazoDer, mBD, texTela);
        pintar(antebrazoDer, rotX(glm::translate(mBD, CODO), -animJeol.valor(V_CODO)), texTela);

        glm::mat4 mMI = rotX(glm::translate(mCadera, CADERA_IZQ), -animJeol.valor(V_MUSLO));
        pintar(musloIzq, mMI, texTela);
        pintar(piernaIzq, rotX(glm::translate(mMI, RODILLA), animJeol.valor(V_RODILLA)), texTela);
        glm::mat4 mMD = rotX(glm::translate(mCadera, CADERA_DER), -animJeol.valor(V_MUSLO));
        pintar(musloDer, mMD, texTela);
        pintar(piernaDer, rotX(glm::translate(mMD, RODILLA), animJeol.valor(V_RODILLA)), texTela);

        pintar(cojin, glm::scale(glm::translate(base, POS_DOLIENTE + glm::vec3(0.0f, 0.0f, -0.10f)),
                                 glm::vec3(1.5f, 0.3f, 2.2f)), texEstera);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void dibujarUI() {
        dibujarMision();
        if (hotspotCercano < 0) return;
        if (mision.activa && mision.paso == z3::PASO_MESA && hotspotCercano == 1) return;
        const Hotspot& h = hotspots[hotspotCercano];

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y - 30.0f),
                                ImGuiCond_Always, ImVec2(0.5f, 1.0f));
        ImGui::SetNextWindowSize(ImVec2(infoAbierta ? 640.0f : 420.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.82f);
        ImGui::Begin("##hotspotZona3", nullptr,
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

    ModeloPtr mesa, jegi, manzana, cojin, incensario, panel;
    ModeloPtr paneles[z3::NUM_PANELES];

    const ModeloPtr& panelDe(int i) const { return paneles[i] ? paneles[i] : panel; }
    ModeloPtr pera, caqui, castana, azufaifa, pescado, platoLargo, tteok, tazonArroz, tazonSopa, copa;
    ModeloPtr cofre, puertaIzq, puertaDer, tableta;
    ModeloPtr cadera, torso, cabeza, brazoIzq, brazoDer, antebrazoIzq, antebrazoDer;
    ModeloPtr musloIzq, musloDer, piernaIzq, piernaDer;

    GLuint texMaderaOscura = 0, texMaderaMesa = 0, texMaderaClara = 0;
    GLuint texLaca = 0, texPapel = 0, texTela = 0, texCara = 0, texEstera = 0;
    GLuint texCaqui = 0, texCastana = 0, texAzufaifa = 0, texPescado = 0, texLaton = 0, texTazon = 0, texTteok = 0;

    Animator animAltar;
    Animator animJeol;

    std::vector<Hotspot> hotspots;
    int  hotspotCercano = -1;
    bool infoAbierta = false;
    glm::vec3 camara = glm::vec3(0.0f);

    // ================= MODO MISIÓN =================
    struct EstadoMision {
        bool  activa = false;
        int   paso = z3::PASO_REVELAR;
        int   pregunta = 0;
        int   reverencias = 0;
        int   errores = 0;
        float tiempo = 0.0f;
        std::string aviso;
        int   tipoAviso = z3::AVISO_INFO;
        float tiempoAviso = 0.0f;
    } mision;

    void avisar(const std::string& texto, int tipo) {
        mision.aviso = texto;
        mision.tipoAviso = tipo;
        mision.tiempoAviso = 5.0f;
        if (tipo == z3::AVISO_ERROR) mision.errores++;
    }

    bool altarAbierto() const {
        return !animAltar.reproduciendo() && animAltar.frame() >= z3::FIN_ALTAR;
    }

    void alternarMision() {
        using namespace z3;
        if (mision.activa) {               // abandonar o cerrar resultados
            mision.activa = false;
            return;
        }
        mision = EstadoMision();
        mision.activa = true;
        if (animAltar.frame() > 0.0f && !animAltar.reproduciendo())
            animAltar.reproducirReversa();   // el rito empieza con el altar cerrado
        avisar("Comienza el Jesa. Sigue los pasos del ritual en orden.", AVISO_INFO);
    }

    void teclaMision(int key, bool cercaAltar, bool cercaCojin) {
        using namespace z3;
        switch (mision.paso) {
        case PASO_REVELAR:
            if (key == GLFW_KEY_E && cercaAltar && !animAltar.reproduciendo() && !altarAbierto()) {
                animAltar.reproducir();
                avisar("Se despliega el biombo y se abre el sinjujip: el ancestro está presente.", AVISO_INFO);
            }
            if (key == GLFW_KEY_F && cercaCojin)
                avisar("Todavía no: primero se revela la tableta del ancestro. Sin ella, la reverencia "
                       "no tiene a quién dirigirse.", AVISO_ERROR);
            break;

        case PASO_MESA:
            if (key == GLFW_KEY_E && cercaAltar)
                avisar("Durante el rito, el biombo y el cofre permanecen abiertos.", AVISO_INFO);
            if (key == GLFW_KEY_F && cercaCojin)
                avisar("Antes de la reverencia hay que verificar que las ofrendas estén bien "
                       "colocadas. Ve a la mesa.", AVISO_ERROR);
            if (hotspotCercano == 1 && key >= GLFW_KEY_1 && key <= GLFW_KEY_3)
                responder(key - GLFW_KEY_1);
            break;

        case PASO_JEOL:
            if (key == GLFW_KEY_E && cercaAltar)
                avisar("Durante el rito, el biombo y el cofre permanecen abiertos.", AVISO_INFO);
            if (key == GLFW_KEY_F && cercaCojin && !animJeol.reproduciendo())
                animJeol.reproducir();
            break;
        }
    }

    void responder(int opcion) {
        using namespace z3;
        const Pregunta& p = PREGUNTAS[mision.pregunta];
        if (opcion != p.correcta) {
            avisar(std::string("Incorrecto. ") + p.pista, AVISO_ERROR);
            return;
        }
        avisar(p.explicacion, AVISO_BIEN);
        if (++mision.pregunta >= NUM_PREGUNTAS) mision.paso = PASO_JEOL;
    }

    void actualizarMision(float dt, bool jeolTermino) {
        using namespace z3;
        if (mision.tiempoAviso > 0.0f) mision.tiempoAviso -= dt;
        if (!mision.activa || mision.paso == PASO_FIN) return;
        mision.tiempo += dt;

        if (mision.paso == PASO_REVELAR && altarAbierto()) {
            mision.paso = PASO_MESA;
            avisar("¡Bien! Ahora revisa la mesa de ofrendas (Jesasang).", AVISO_BIEN);
        }
        if (mision.paso == PASO_JEOL && jeolTermino) {
            mision.reverencias++;
            if (mision.reverencias == 1)
                avisar("Primera reverencia. En el Jesa la reverencia se hace dos veces: presiona F otra vez.",
                       AVISO_BIEN);
            else
                mision.paso = PASO_FIN;
        }
    }

    void dibujarMision() {
        using namespace z3;
        ImGuiIO& io = ImGui::GetIO();
        const ImGuiWindowFlags fijas = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;
        const ImVec4 dorado(0.95f, 0.80f, 0.55f, 1.0f), verde(0.55f, 0.90f, 0.55f, 1.0f),
                     rojo(1.0f, 0.50f, 0.45f, 1.0f), gris(0.65f, 0.65f, 0.65f, 1.0f);

        bool cercaZona = distanciaPiso(camara, origen) < 6.0f;
        if (!mision.activa) {
            if (!cercaZona) return;
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 12.0f, 12.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            ImGui::SetNextWindowBgAlpha(0.55f);
            ImGui::Begin("##misionInvitacion", nullptr, fijas);
            ImGui::TextColored(dorado, "Misión: el ritual Jesa");
            ImGui::TextDisabled("G: comenzar");
            ImGui::End();
            return;
        }

        // Panel de pasos (arriba a la derecha)
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 12.0f, 12.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.75f);
        ImGui::Begin("##misionPasos", nullptr, fijas);
        ImGui::TextColored(dorado, "Misión: el ritual Jesa");
        ImGui::Separator();
        const char* pasos[3] = {
            "1. Revela la tableta ancestral (E en el altar)",
            "2. Revisa la mesa de ofrendas (acércate a la mesa)",
            "3. Haz la reverencia Jeol dos veces (F en el cojín)" };
        for (int i = 0; i < 3; i++) {
            if (i < mision.paso)       ImGui::TextColored(verde, "[x] %s", pasos[i]);
            else if (i == mision.paso) ImGui::TextColored(dorado, "[ ] %s", pasos[i]);
            else                       ImGui::TextColored(gris, "[ ] %s", pasos[i]);
        }
        if (mision.paso == PASO_MESA) ImGui::TextDisabled("    Preguntas: %d / %d", mision.pregunta, NUM_PREGUNTAS);
        if (mision.paso == PASO_JEOL) ImGui::TextDisabled("    Reverencias: %d / 2", mision.reverencias);
        ImGui::Separator();
        ImGui::TextDisabled("Tiempo: %d s   Errores: %d   G: salir", (int)mision.tiempo, mision.errores);
        ImGui::End();

        // Pregunta de la mesa (centro de la pantalla)
        if (mision.paso == PASO_MESA && hotspotCercano == 1) {
            const Pregunta& p = PREGUNTAS[mision.pregunta];
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.55f),
                                    ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(620.0f, 0.0f), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.88f);
            ImGui::Begin("##misionPregunta", nullptr, fijas & ~ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextColored(dorado, "Pregunta %d de %d", mision.pregunta + 1, NUM_PREGUNTAS);
            ImGui::Separator();
            ImGui::TextWrapped("%s", p.texto);
            ImGui::Spacing();
            for (int i = 0; i < 3; i++) ImGui::Text("  %d)  %s", i + 1, p.opciones[i]);
            ImGui::Spacing();
            ImGui::TextDisabled("Responde con las teclas 1, 2 o 3");
            ImGui::End();
        }

        // Resultado final
        if (mision.paso == PASO_FIN) {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.40f),
                                    ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(620.0f, 0.0f), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.90f);
            ImGui::Begin("##misionFin", nullptr, fijas & ~ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextColored(dorado, "Ritual completado");
            ImGui::Separator();
            ImGui::TextWrapped("Revelaste la tableta del ancestro, verificaste la mesa de ofrendas y "
                               "rendiste la doble reverencia. Con el Jesa, la familia expresa la piedad "
                               "filial (hyo) y mantiene el vínculo con sus antepasados.");
            ImGui::Spacing();
            ImGui::Text("Tiempo: %d s     Errores: %d", (int)mision.tiempo, mision.errores);
            ImGui::TextColored(mision.errores == 0 ? verde : dorado, "%s",
                mision.errores == 0 ? "¡Ritual perfecto!" : "Puedes intentarlo de nuevo para no cometer errores.");
            ImGui::TextDisabled("G: cerrar");
            ImGui::End();
        }

        // Aviso temporal (arriba al centro)
        if (mision.tiempoAviso > 0.0f && !mision.aviso.empty()) {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 60.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(560.0f, 0.0f), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.80f);
            ImGui::Begin("##misionAviso", nullptr, fijas & ~ImGuiWindowFlags_AlwaysAutoResize);
            ImVec4 c = mision.tipoAviso == AVISO_BIEN ? verde : (mision.tipoAviso == AVISO_ERROR ? rojo : dorado);
            ImGui::PushStyleColor(ImGuiCol_Text, c);
            ImGui::TextWrapped("%s", mision.aviso.c_str());
            ImGui::PopStyleColor();
            ImGui::End();
        }
    }

    template <typename F>
    void dibujarPiramide(F& pintar, const glm::mat4& mPlato, const ModeloPtr& fruta,
                         GLuint tex, float d, float alto, float esc) {
        using namespace z3;
        if (!fruta) return;
        const float r = 0.6f * d;
        const glm::vec3 pos[4] = {
            glm::vec3(-0.866f * r, 0.0f, -0.5f * r),
            glm::vec3( 0.866f * r, 0.0f, -0.5f * r),
            glm::vec3( 0.0f,       0.0f,  r),
            glm::vec3( 0.0f,       alto,  0.0f) };
        for (int k = 0; k < 4; k++) {
            glm::mat4 m = rotY(glm::translate(mPlato, pos[k]), 70.0f * k);
            pintar(fruta, glm::scale(m, glm::vec3(esc)), tex);
        }
    }

    template <typename F>
    void dibujarMonton(F& pintar, const glm::mat4& mPlato, const ModeloPtr& fruto,
                       GLuint tex, float d, float alto) {
        using namespace z3;
        if (!fruto) return;
        std::vector<glm::vec3> pos = { glm::vec3(0.0f) };
        for (int k = 0; k < 6; k++) {
            float a = glm::radians(60.0f * k);
            pos.push_back(glm::vec3(d * cos(a), 0.0f, d * sin(a)));
        }
        for (int k = 0; k < 3; k++) {
            float a = glm::radians(120.0f * k + 30.0f);
            pos.push_back(glm::vec3(0.58f * d * cos(a), alto, 0.58f * d * sin(a)));
        }
        pos.push_back(glm::vec3(0.0f, 2.0f * alto, 0.0f));
        for (size_t k = 0; k < pos.size(); k++)
            pintar(fruto, rotY(glm::translate(mPlato, pos[k]), 47.0f * k), tex);
    }

    static bool existe(const std::string& ruta) { return std::ifstream(ruta).good(); }

    static ModeloPtr cargarModelo(const std::string& archivo) {
        std::string ruta = z3::RUTA + archivo;
        if (!existe(ruta)) {
            std::cout << "[Zona3] Aun no existe: " << ruta << " (se omite)\n";
            return nullptr;
        }
        return std::make_unique<Model>((GLchar*)ruta.c_str());
    }

    static GLuint cargarTextura(const std::string& archivo) {
        std::string dir = z3::RUTA.substr(0, z3::RUTA.size() - 1);
        if (!existe(z3::RUTA + archivo)) {
            std::cout << "[Zona3] Falta la textura " << archivo << "\n";
            return 0;
        }
        return (GLuint)TextureFromFile(archivo.c_str(), dir);
    }
};

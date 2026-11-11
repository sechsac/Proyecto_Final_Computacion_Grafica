#pragma once
// =====================================================================
//  Zona3.h  -  Sala principal: altar del Jesa
// ---------------------------------------------------------------------
//  Todo lo de la zona 3 vive aquí. main.cpp solo llama a:
//      zona3.cargar();                          (una vez, antes del loop)
//      zona3.actualizar(deltaTime, camPos);     (cada frame)
//      zona3.dibujar(lightingShader, modelLoc); (cada frame)
//      zona3.dibujarUI();                       (entre NewFrame y Render de ImGui)
//      zona3.tecla(key);                        (en KeyCallback, al presionar)
//
//  IMPORTANTE: este archivo es "header-only" (todo el código está aquí)
//  y SOLO debe incluirse en main.cpp. Model.h define una función global
//  (TextureFromFile) que daría error de enlace si se incluyera en dos .cpp.
// =====================================================================
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
// =====================================================================
//  MEDIDAS (en metros). Ejes de OpenGL: X derecha, Y arriba, Z hacia
//  el visitante. El altar está en el origen de la zona y "mira" hacia +Z.
//  (Al exportar de Blender con Forward -Z / Up Y: Blender Y = OpenGL -Z)
// =====================================================================
const std::string RUTA = "Models/z3/";

const float ALTURA_MESA = 0.70f;                 // superficie de la jesasang

// --- Pedestales jegi (fila al frente de la mesa) ---
const float JEGI_Z = 0.15f;
const float JEGI_X[] = { -0.40f, -0.20f, 0.0f, 0.20f, 0.40f };
const int   NUM_JEGI = 5;
const float ALTURA_PLATO_JEGI = 0.084f;          // fondo del plato (pedestal escalado a 0.8)
const float ESCALA_MANZANA = 0.7f;               // el .obj quedó de 12 cm; 0.7 -> 8.5 cm
                                                 // (si la vuelves a exportar a 8.5 cm, pon 1.0f)

// --- Cofre (sinjujip), sobre la mesa, al centro y hacia atrás ---
const glm::vec3 POS_COFRE(0.0f, ALTURA_MESA, -0.25f);
const glm::vec3 BISAGRA_IZQ(-0.08f, 0.0f, 0.06f);   // relativo al cofre
const glm::vec3 BISAGRA_DER( 0.08f, 0.0f, 0.06f);
const float APERTURA_PUERTAS = 110.0f;

// --- Biombo (byeongpung): el panel 0 tiene su bisagra izquierda aquí ---
const glm::vec3 POS_BIOMBO(-1.20f, 0.0f, -0.90f);
const float ANCHO_PANEL = 0.45f;
const int   NUM_PANELES = 6;
const float PLEGADO = 175.0f;                    // bisagra cerrada (acordeón)
const float ABIERTO = 30.0f;                     // bisagra abierta (zigzag)

// --- Doliente: posición de los pies, frente a la mesa ---
const glm::vec3 POS_DOLIENTE(0.0f, 0.0f, 1.10f);
const float     ALTURA_CADERA = 0.95f;
// Articulaciones: posición de cada pieza RESPECTO A SU PADRE (se miden en Blender)
const glm::vec3 CINTURA   ( 0.00f,  0.10f, 0.0f);
const glm::vec3 CUELLO    ( 0.00f,  0.50f, 0.0f);
const glm::vec3 HOMBRO_IZQ(-0.20f,  0.45f, 0.0f);
const glm::vec3 HOMBRO_DER( 0.20f,  0.45f, 0.0f);
const glm::vec3 CODO      ( 0.00f, -0.30f, 0.0f);
const glm::vec3 CADERA_IZQ(-0.10f, -0.05f, 0.0f);
const glm::vec3 CADERA_DER( 0.10f, -0.05f, 0.0f);
const glm::vec3 RODILLA   ( 0.00f, -0.45f, 0.0f);

// Índices de los valores de cada Animator
enum { PUERTA_IZQ = 5, PUERTA_DER = 6 };                                        // animAltar
enum { BAJADA_Y = 0, V_TORSO, V_CABEZA, V_HOMBRO, V_CODO, V_MUSLO, V_RODILLA };  // animJeol

const float DIST_ALTAR = 2.5f;
const float DIST_COJIN = 2.0f;

inline glm::mat4 rotX(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(1, 0, 0)); }
inline glm::mat4 rotY(const glm::mat4& m, float g) { return glm::rotate(m, glm::radians(g), glm::vec3(0, 1, 0)); }
inline float distanciaPiso(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(glm::vec2(a.x - b.x, a.z - b.z));
}
} // namespace z3

class Zona3 {
public:
    // Dónde está la zona 3 dentro del Hanok y hacia dónde mira (lo define
    // quien coloque el Hanok; mientras tanto, el altar queda en el origen).
    glm::vec3 origen    = glm::vec3(0.0f);
    float     rotacionY = 0.0f;   // grados

    // -----------------------------------------------------------------
    void cargar() {
        using namespace z3;
        // ---------- Modelos (si un archivo no existe, se omite) ----------
        mesa      = cargarModelo("z3_jesasang.obj");
        jegi      = cargarModelo("z3_jegi.obj");
        manzana   = cargarModelo("z3_manzana.obj");
        cojin     = cargarModelo("z3_cojin.obj");
        incensario= cargarModelo("z3_incensario.obj");
        panel     = cargarModelo("z3_biombo_panel.obj");
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

        // ---------- Texturas de respaldo ----------
        // Los modelos sin imagen en su .mtl se pintan con estas texturas.
        texMaderaOscura = cargarTextura("madera_oscura.png");
        texMaderaMesa   = cargarTextura("madera_mesa.png");
        texMaderaClara  = cargarTextura("madera_clara.png");
        texLaca         = cargarTextura("laca_roja.png");
        texPapel        = cargarTextura("papel_hanji.png");
        texTela         = cargarTextura("tela_canamo.png");   // sangbok de cáñamo
        texCara         = cargarTextura("cara_doliente.png"); // cara, cabello y sujil
        texEstera       = cargarTextura("estera_paja.png");   // estera dotjari

        // ---------- Animación 1 (KeyFrames): se prepara el altar ----------
        // El biombo se despliega panel por panel y las puertas del cofre
        // se abren poco a poco, revelando la tableta del ancestro.
        const float P = PLEGADO, A = ABIERTO, D = APERTURA_PUERTAS;
        //                  bis1 bis2 bis3 bis4 bis5   puertaI   puertaD
        animAltar.agregar(  0, { P,   P,   P,   P,   P,   0.0f,     0.0f    });
        animAltar.agregar( 24, { A,   P,   P,   P,   P,   D * 0.2f, D * 0.2f });
        animAltar.agregar( 48, { A,   A,   P,   P,   P,   D * 0.4f, D * 0.4f });
        animAltar.agregar( 72, { A,   A,   A,   P,   P,   D * 0.6f, D * 0.6f });
        animAltar.agregar( 96, { A,   A,   A,   A,   P,   D * 0.8f, D * 0.8f });
        animAltar.agregar(120, { A,   A,   A,   A,   A,   D,        D        });

        // ---------- Animación 2 (KeyFrames): reverencia jeol ----------
        //                 bajadaY  torso cabeza hombro codo muslo rodilla
        animJeol.agregar(  0, { 0.00f,   0,    0,     0,    0,   0,    0 });  // erguido
        animJeol.agregar( 45, { 0.00f,   0,    0,    80,   70,   0,    0 });  // manos a los ojos
        animJeol.agregar( 90, {-0.45f,   0,    0,    80,   70,   0,   90 });  // arrodillado
        animJeol.agregar(135, {-0.60f,  85,   10,   150,   20,  70,  160 });  // reverencia completa
        animJeol.agregar(180, {-0.45f,   0,    0,    80,   70,   0,   90 });  // regresa a rodillas
        animJeol.agregar(225, { 0.00f,   0,    0,     0,    0,   0,    0 });  // de pie

        // ---------- Hotspots culturales ----------
        hotspots.push_back({ glm::vec3(0.0f, 0.0f, 2.6f), 1.4f,
            "Jesa: el culto a los ancestros",
            "El Jesa es la ceremonia confuciana con la que las familias coreanas honran a sus "
            "ancestros: en el aniversario luctuoso (Gijesa) o en festividades como Seollal y "
            "Chuseok (Charye). Expresa la piedad filial (hyo) y mantiene el vínculo entre los "
            "vivos y los antepasados del linaje.",
            "E: preparar el altar (abrir biombo y cofre)" });

        hotspots.push_back({ glm::vec3(-0.9f, 0.0f, 0.8f), 0.9f,
            "La mesa ritual (Jesasang)",
            "Tradicionalmente la mesa se orienta hacia el norte. En la fila más cercana al "
            "ancestro van el arroz, la sopa y las copas de licor; en medio, carnes, pescados y "
            "verduras (namul); al frente, frutas y dulces sobre pedestales de madera (jegi). "
            "Reglas como Hongdong-baekseo (frutas rojas al este, blancas al oeste) ordenan los "
            "platillos. Se evitan el ajo, el chile y el durazno.",
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

    // -----------------------------------------------------------------
    void actualizar(float deltaTime, const glm::vec3& posCamara) {
        camara = posCamara;
        animAltar.actualizar(deltaTime);
        animJeol.actualizar(deltaTime);

        int anterior = hotspotCercano;
        hotspotCercano = -1;
        float mejor = 1e9f;
        for (int i = 0; i < (int)hotspots.size(); i++) {
            float d = hotspots[i].distancia(camara, origen);
            if (d < hotspots[i].radio && d < mejor) { mejor = d; hotspotCercano = i; }
        }
        if (hotspotCercano != anterior) infoAbierta = false;
    }

    // -----------------------------------------------------------------
    void tecla(int key) {
        using namespace z3;
        if (key == GLFW_KEY_I && hotspotCercano >= 0)
            infoAbierta = !infoAbierta;
        if (key == GLFW_KEY_E && distanciaPiso(camara, origen) < DIST_ALTAR + 1.0f)
            animAltar.alternar();
        if (key == GLFW_KEY_F && distanciaPiso(camara, origen + POS_DOLIENTE) < DIST_COJIN
            && !animJeol.reproduciendo())
            animJeol.reproducir();
    }

    // -----------------------------------------------------------------
    void dibujar(Shader& shader, GLint modelLoc) {
        using namespace z3;
        // Manda la matriz, activa la textura de respaldo y dibuja.
        auto pintar = [&](const ModeloPtr& m, const glm::mat4& matriz, GLuint tex) {
            if (!m) return;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matriz));
            m->Draw(shader);
        };

        glm::mat4 base = glm::translate(glm::mat4(1.0f), origen);
        base = rotY(base, rotacionY);

        // ---------- 1. Mesa ----------
        pintar(mesa, base, texMaderaMesa);

        // ---------- 2. Pedestales con fruta ----------
        for (int i = 0; i < NUM_JEGI; i++) {
            glm::mat4 mJegi = glm::translate(base, glm::vec3(JEGI_X[i], ALTURA_MESA, JEGI_Z));
            pintar(jegi, glm::scale(mJegi, glm::vec3(0.8f)), texLaca);
            // Hongdong-baekseo: frutas rojas al este (derecha del visitante)
            if (JEGI_X[i] > 0.1f) dibujarPiramide(pintar, mJegi);
        }

        // ---------- 3. Incensario al frente (si existe) ----------
        pintar(incensario, glm::translate(base, glm::vec3(0.0f, ALTURA_MESA, 0.28f)), texMaderaOscura);

        // ---------- 4. Cofre: jerarquía cofre -> puertas ----------
        glm::mat4 mCofre = glm::translate(base, POS_COFRE);
        pintar(cofre, mCofre, texMaderaOscura);
        pintar(tableta, mCofre, texMaderaClara);
        pintar(puertaIzq, rotY(glm::translate(mCofre, BISAGRA_IZQ), -animAltar.valor(PUERTA_IZQ)), texMaderaOscura);
        pintar(puertaDer, rotY(glm::translate(mCofre, BISAGRA_DER),  animAltar.valor(PUERTA_DER)), texMaderaOscura);

        // ---------- 5. Biombo: cada panel es HIJO del anterior ----------
        glm::mat4 mPanel = glm::translate(base, POS_BIOMBO);
        pintar(panel, mPanel, texPapel);
        for (int i = 1; i < NUM_PANELES; i++) {
            float signo = (i % 2 == 1) ? -1.0f : 1.0f;
            mPanel = glm::translate(mPanel, glm::vec3(ANCHO_PANEL, 0.0f, 0.0f));
            mPanel = rotY(mPanel, signo * animAltar.valor(i - 1));
            pintar(panel, mPanel, texPapel);
        }

        // ---------- 6. Doliente: modelo jerárquico ----------
        //  cadera -> torso -> cabeza
        //                  -> brazo -> antebrazo  (x2)
        //  cadera -> muslo -> pierna              (x2)
        // Se asume que en Blender el personaje mira al frente (-Y), como es lo normal;
        // el giro de 180° lo voltea hacia el altar. Si una parte se dobla al revés,
        // cambia el signo de su ángulo.
        glm::mat4 mCadera = glm::translate(base,
            POS_DOLIENTE + glm::vec3(0.0f, ALTURA_CADERA + animJeol.valor(BAJADA_Y), 0.0f));
        mCadera = rotY(mCadera, 180.0f);   // que mire hacia el altar
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

        // Estera de paja delgada (2.5 cm) y más larga que el cojín original
        pintar(cojin, glm::scale(glm::translate(base, POS_DOLIENTE + glm::vec3(0.0f, 0.0f, -0.10f)),
                                 glm::vec3(1.5f, 0.3f, 2.2f)), texEstera);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // -----------------------------------------------------------------
    void dibujarUI() {
        if (hotspotCercano < 0) return;
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
    ModeloPtr cofre, puertaIzq, puertaDer, tableta;
    ModeloPtr cadera, torso, cabeza, brazoIzq, brazoDer, antebrazoIzq, antebrazoDer;
    ModeloPtr musloIzq, musloDer, piernaIzq, piernaDer;

    GLuint texMaderaOscura = 0, texMaderaMesa = 0, texMaderaClara = 0;
    GLuint texLaca = 0, texPapel = 0, texTela = 0, texCara = 0, texEstera = 0;

    Animator animAltar;
    Animator animJeol;

    std::vector<Hotspot> hotspots;
    int  hotspotCercano = -1;
    bool infoAbierta = false;
    glm::vec3 camara = glm::vec3(0.0f);

    template <typename F>
    void dibujarPiramide(F& pintar, const glm::mat4& mJegi) {
        using namespace z3;
        if (!manzana) return;
        const glm::vec3 pos[4] = {
            glm::vec3(-0.045f, ALTURA_PLATO_JEGI,          -0.026f),
            glm::vec3( 0.045f, ALTURA_PLATO_JEGI,          -0.026f),
            glm::vec3( 0.000f, ALTURA_PLATO_JEGI,           0.052f),
            glm::vec3( 0.000f, ALTURA_PLATO_JEGI + 0.065f,  0.000f) };
        for (int k = 0; k < 4; k++) {
            glm::mat4 m = glm::translate(mJegi, pos[k]);
            m = rotY(m, 70.0f * k);                       // que no se vean idénticas
            m = glm::scale(m, glm::vec3(ESCALA_MANZANA));
            pintar(manzana, m, 0);                        // la manzana trae su propia textura
        }
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
        return (GLuint)TextureFromFile(archivo.c_str(), dir);   // función de Model.h
    }
};

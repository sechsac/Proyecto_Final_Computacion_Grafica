#pragma once
// =====================================================================
//  Animator.h  -  Sistema de animación por KeyFrames (Zona 3 - Jesa)
// ---------------------------------------------------------------------
//  IDEA: un "keyframe" es una pose guardada en un momento dado.
//  Tú solo defines algunas poses (frame 0, 45, 90...) y el Animator
//  calcula automáticamente las poses intermedias (interpolación).
//
//  Cada keyframe guarda una LISTA de valores. Cada posición de la lista
//  es "algo que se mueve": un ángulo en grados o un desplazamiento en m.
//
//  Ejemplo:  anim.agregar(0,  {0.0f, 0.0f});   // frame 0
//            anim.agregar(45, {90.0f, 0.5f});  // frame 45
//  En el frame 22 aprox. anim.valor(0) devolverá ~45 grados.
// =====================================================================
#include <vector>
#include <cstddef>

// Los keyframes se escriben en "número de frame", igual que en la
// propuesta (0, 45, 90, 135). Esta constante convierte segundos reales
// a frames: a 30 FPS, el frame 45 ocurre a los 1.5 segundos.
constexpr float FPS_ANIMACION = 30.0f;

struct KeyFrame {
    float frame;                 // número de frame (0, 45, 90...)
    std::vector<float> valores;  // un valor por cada cosa que se anima
};

class Animator {
public:
    // Agrega una pose. IMPORTANTE: agrégalas en orden (frame creciente)
    // y todas con la misma cantidad de valores.
    void agregar(float frame, const std::vector<float>& valores) {
        frames.push_back({ frame, valores });
    }

    // Reproduce desde el inicio hacia el final.
    void reproducir() {
        if (frames.size() < 2) return;
        actual = frames.front().frame;
        activo = true;
        reversa = false;
    }

    // Reproduce desde el final hacia el inicio (para "cerrar").
    void reproducirReversa() {
        if (frames.size() < 2) return;
        actual = frames.back().frame;
        activo = true;
        reversa = true;
    }

    // Si está al inicio, abre; si está al final, cierra.
    // Útil para el biombo: un clic lo abre, otro lo cierra.
    void alternar() {
        if (activo || frames.size() < 2) return;
        if (actual >= frames.back().frame) reproducirReversa();
        else reproducir();
    }

    bool reproduciendo() const { return activo; }

    // Llamar UNA vez por frame con deltaTime (segundos desde el frame anterior).
    void actualizar(float deltaTime) {
        if (!activo) return;
        float paso = deltaTime * FPS_ANIMACION;
        if (!reversa) {
            actual += paso;
            if (actual >= frames.back().frame) { actual = frames.back().frame; activo = false; }
        } else {
            actual -= paso;
            if (actual <= frames.front().frame) { actual = frames.front().frame; activo = false; }
        }
    }

    // Devuelve el valor número i ya interpolado para el momento actual.
    float valor(std::size_t i) const {
        if (frames.empty()) return 0.0f;
        if (actual <= frames.front().frame) return frames.front().valores[i];
        if (actual >= frames.back().frame)  return frames.back().valores[i];

        // 1) Buscar entre qué dos keyframes estamos: [a, b]
        std::size_t k = 0;
        while (k + 1 < frames.size() && frames[k + 1].frame < actual) k++;
        const KeyFrame& a = frames[k];
        const KeyFrame& b = frames[k + 1];

        // 2) u = qué tan avanzados vamos entre a y b (0 = en a, 1 = en b)
        float u = (actual - a.frame) / (b.frame - a.frame);

        // 3) Suavizado: arranca y frena despacio (se ve más natural y solemne)
        u = u * u * (3.0f - 2.0f * u);

        // 4) Interpolación lineal: a + (b - a) * u
        return a.valores[i] + (b.valores[i] - a.valores[i]) * u;
    }

private:
    std::vector<KeyFrame> frames;
    float actual = 0.0f;
    bool activo = false;
    bool reversa = false;
};

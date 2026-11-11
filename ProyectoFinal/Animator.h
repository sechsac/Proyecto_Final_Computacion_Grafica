#pragma once

#include <vector>
#include <cstddef>

constexpr float FPS_ANIMACION = 30.0f;

struct KeyFrame {
    float frame;
    std::vector<float> valores;
};

class Animator {
public:

    void agregar(float frame, const std::vector<float>& valores) {
        frames.push_back({ frame, valores });
    }

    void reproducir() {
        if (frames.size() < 2) return;
        actual = frames.front().frame;
        activo = true;
        reversa = false;
    }

    void reproducirReversa() {
        if (frames.size() < 2) return;
        actual = frames.back().frame;
        activo = true;
        reversa = true;
    }

    void alternar() {
        if (activo || frames.size() < 2) return;
        if (actual >= frames.back().frame) reproducirReversa();
        else reproducir();
    }

    bool reproduciendo() const { return activo; }

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

    float valor(std::size_t i) const {
        if (frames.empty()) return 0.0f;
        if (actual <= frames.front().frame) return frames.front().valores[i];
        if (actual >= frames.back().frame)  return frames.back().valores[i];

        std::size_t k = 0;
        while (k + 1 < frames.size() && frames[k + 1].frame < actual) k++;
        const KeyFrame& a = frames[k];
        const KeyFrame& b = frames[k + 1];

        float u = (actual - a.frame) / (b.frame - a.frame);

        u = u * u * (3.0f - 2.0f * u);

        return a.valores[i] + (b.valores[i] - a.valores[i]) * u;
    }

private:
    std::vector<KeyFrame> frames;
    float actual = 0.0f;
    bool activo = false;
    bool reversa = false;
};

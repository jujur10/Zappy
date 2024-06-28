//
// Created by quentinf on 28/06/24.
//

#pragma once

#define MAX_PARTICLES 100
#include <Vector2.hpp>
#include <vector>

namespace zappy_gui
{
struct Particle
{
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    float lifeTime;
};

class WinningEffect
{
   public:
    explicit WinningEffect(const std::string& team);

    // ~WinningEffect();

    static Particle CreateParticle();

    void Update(const float deltaTime);

    void Draw() const;

    private:
        std::string winningTeam;
        std::vector<Particle> particles;
        float timer;
};
}  // namespace zappy_gui

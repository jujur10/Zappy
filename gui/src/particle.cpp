//
// Created by quentinf on 28/06/24.
//

#include "particle.hpp"

#include <cmath>
#include <cstdint>
#include <string>

#include "raylib.h"

namespace zappy_gui
{
    WinningEffect::WinningEffect(const std::string& team) : winningTeam(team), timer(0)
    {
        for (uint32_t i = 0; i < MAX_PARTICLES; ++i)
        {
            particles.emplace_back(CreateParticle());
        }
    }

    Particle WinningEffect::CreateParticle()
    {
        Particle p{};
        p.position = {static_cast<float>(::GetScreenWidth()) / 2, static_cast<float>(::GetScreenHeight()) / 2};
        const float angle = static_cast<float>(::GetRandomValue(0, 360)) * DEG2RAD;
        const float speed = static_cast<float>(::GetRandomValue(100, 200));
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.radius = static_cast<float>(::GetRandomValue(2, 5));
        p.color = ::ColorFromHSV(static_cast<float>(::GetRandomValue(0, 360)), 0.7f, 1.0f);
        p.lifeTime = static_cast<float>(::GetRandomValue(2, 5));
        return p;
    }

    void WinningEffect::Update(const float deltaTime)
    {
        timer += deltaTime;

        for (auto& p : particles)
        {
            p.position.x += p.velocity.x * deltaTime;
            p.position.y += p.velocity.y * deltaTime;
            p.velocity.y += 98.1f * deltaTime;  // Apply gravity
            p.lifeTime -= deltaTime;

            if (p.lifeTime <= 0)
            {
                p = CreateParticle();
            }
        }
    }

    void WinningEffect::Draw() const
    {
        // Draw particles
        for (const auto& p : particles)
        {
            ::DrawCircleV(p.position, p.radius, ::Fade(p.color, p.lifeTime / 5.0f));
        }

        // Draw congratulatory text
        constexpr char const* const congrats = "Félicitations!";
        uint8_t fontSize = 40;
        const Vector2 textSize = ::MeasureTextEx(::GetFontDefault(), congrats, fontSize, 2);
        ::DrawText(congrats, ::GetScreenWidth() / 2 - textSize.x / 2, ::GetScreenHeight() / 3 - textSize.y / 2, fontSize, BLACK);

        // Draw pulsating winning team name with glow effect
        fontSize = 60;
        const float scale = 1.0f + 0.1f * std::sin(timer * 5.0f);
        const auto scaledFontSize = static_cast<uint8_t>(fontSize * scale);
        const Vector2 scaledTextSize = ::MeasureTextEx(::GetFontDefault(), winningTeam.c_str(), scaledFontSize, 2);
        const Vector2 position = {::GetScreenWidth() / 2.0f - scaledTextSize.x / 2, ::GetScreenHeight() / 2.0f - scaledTextSize.y / 2};

        // Draw glow effect
        const Color glowColor = ::ColorAlpha(BLUE, 0.3f + 0.2f * std::sin(timer * 5.0f));
        for (uint8_t i = 5; i > 0; --i)
        {
            ::DrawTextEx(::GetFontDefault(),
                       winningTeam.c_str(),
                       Vector2{position.x - i, position.y - i},
                       scaledFontSize,
                       2,
                       ::ColorAlpha(glowColor, 0.03f * i));
        }

        // Draw main text
        ::DrawTextEx(::GetFontDefault(), winningTeam.c_str(), position, scaledFontSize, 2, BLUE);
    }
}  // namespace zappy_gui

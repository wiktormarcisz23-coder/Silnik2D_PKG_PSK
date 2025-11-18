#include "Engine.hpp"
#include "Logger.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

void drawEllipse(sf::RenderWindow& win, float h, float k, float a, float b,
    const sf::Color& fillColor,
    const sf::Color& outlineColor = sf::Color::Transparent,
    float outlineThickness = 0.f,
    int pointCount = 100) {

    const float pi = 3.14159265359f;

    sf::VertexArray filled(sf::PrimitiveType::TriangleFan, pointCount + 2);
    filled[0].position = sf::Vector2f(h, k);
    filled[0].color = fillColor;

    for (int i = 0; i <= pointCount; ++i) {
        float angle = 2.0f * pi * i / pointCount;
        float x = h + a * std::cos(angle);
        float y = k + b * std::sin(angle);
        filled[i + 1].position = sf::Vector2f(x, y);
        filled[i + 1].color = fillColor;
    }
    win.draw(filled);

    if (outlineThickness > 0.f && outlineColor != sf::Color::Transparent) {
        sf::VertexArray border(sf::PrimitiveType::LineStrip, pointCount + 1);
        for (int i = 0; i <= pointCount; ++i) {
            float angle = 2.0f * pi * i / pointCount;
            float x = h + a * std::cos(angle);
            float y = k + b * std::sin(angle);
            border[i].position = sf::Vector2f(x, y);
            border[i].color = outlineColor;
        }
        win.draw(border);

        const int thicknessSteps = static_cast<int>(outlineThickness);
        for (int t = 1; t <= thicknessSteps; ++t) {
            sf::VertexArray thick1(sf::PrimitiveType::LineStrip, pointCount + 1);
            sf::VertexArray thick2(sf::PrimitiveType::LineStrip, pointCount + 1);
            for (int i = 0; i <= pointCount; ++i) {
                float angle = 2.0f * pi * i / pointCount;
                float x = h + a * std::cos(angle);
                float y = k + b * std::sin(angle);

                sf::Vector2f dir(std::cos(angle), std::sin(angle));
                sf::Vector2f perp(-dir.y, dir.x);
                sf::Vector2f offset = perp * (t / a);

                thick1[i].position = sf::Vector2f(x + offset.x, y + offset.y);
                thick1[i].color = outlineColor;
                thick2[i].position = sf::Vector2f(x - offset.x, y - offset.y);
                thick2[i].color = outlineColor;
            }
            win.draw(thick1);
            win.draw(thick2);
        }
    }
}

int main() {
    EngineConfig cfg;
    cfg.width = 1280;
    cfg.height = 720;
    cfg.title = "Silnik 2D";
    cfg.vsync = true;
    cfg.targetFPS = 120;
    cfg.clearColor = sf::Color(25, 28, 35);

    Engine engine(cfg);
    if (!engine.init()) {
        Logger::instance().error("Uruchomienie silnika zakonczono niepowodzeniem.");
        return 1;
    }

    auto onUpdate = [&](float dt, const InputState& in) {
        (void)dt;
        (void)in;
        };

    auto PrimitiveRenderer = [&](sf::RenderWindow& win) {

        sf::CircleShape circle(50.f);
        circle.setFillColor(sf::Color::Cyan);
        circle.setOrigin(sf::Vector2f(50.f, 50.f));
        circle.setPosition(sf::Vector2f(cfg.width / 2.f, cfg.height / 2.f));
        win.draw(circle);

        sf::RectangleShape rect(sf::Vector2f(120.f, 60.f));
        rect.setFillColor(sf::Color(255, 128, 0));
        rect.setOrigin(sf::Vector2f(60.f, 30.f));
        rect.setPosition(sf::Vector2f(cfg.width / 2.f - 200.f, cfg.height / 2.f));
        win.draw(rect);
        drawEllipse(
            win,
            cfg.width / 2.f + 200.f,
            cfg.height / 2.f,
            120.f,
            60.f,
            sf::Color::Yellow,
            sf::Color::Red,
            3.f,
            120
        );
        };

    try {
        engine.run(onUpdate, PrimitiveRenderer);
    }
    catch (const std::exception& e) {
        Logger::instance().error(std::string("Blad: ") + e.what());
    }

    engine.shutdown();
    return 0;
}
#include "Engine.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <thread>

Engine::Engine(const EngineConfig& cfg) : cfg_(cfg) {}

sf::VideoMode Engine::pickVideoMode_() const {
    if (cfg_.fullscreen) {
        const auto modes = sf::VideoMode::getFullscreenModes();
        auto it = std::find_if(modes.begin(), modes.end(),
            [&](const sf::VideoMode& m) {
                return m.size.x == cfg_.width && m.size.y == cfg_.height;
            });
        return (it != modes.end()) ? *it : modes.front();
    }
    else {
        return sf::VideoMode{ sf::Vector2u{ cfg_.width, cfg_.height } };
    }
}

bool Engine::init() {
    Logger::instance().setLogFile(cfg_.logFile);
    Logger::instance().info("Engine init...");

    try {
        const auto mode = pickVideoMode_();

        // SFML 3: dekoracje w sf::Style, tryb (windowed/fullscreen) w sf::State
        auto style = cfg_.fullscreen
            ? sf::Style::None            // fullscreen ignoruje dekoracje
            : (sf::Style::Titlebar | sf::Style::Close);
        auto state = cfg_.fullscreen ? sf::State::Fullscreen : sf::State::Windowed;

        window_.create(mode, cfg_.title, style, state); // ← 4. argument to sf::State
        if (!window_.isOpen()) {
            Logger::instance().error("Failed to create window.");
            return false;
        }

        window_.setVerticalSyncEnabled(cfg_.vsync);
        if (!cfg_.vsync && cfg_.targetFPS > 0)
            Logger::instance().info("VSync OFF, FPS capped via sleep to " + std::to_string(cfg_.targetFPS));
        else if (cfg_.vsync)
            Logger::instance().info("VSync ON");

        if (cfg_.clearTexturePath) {
            setClearTexture(*cfg_.clearTexturePath);
        }
        else {
            window_.clear(cfg_.clearColor);
            window_.display();
        }
    }
    catch (const std::exception& e) {
        Logger::instance().error(std::string("Exception during init: ") + e.what());
        return false;
    }
    Logger::instance().info("Engine initialized.");
    return true;
}


void Engine::run(const std::function<void(float, const InputState&)>& onUpdate,
    const std::function<void(sf::RenderWindow&)>& onRender) {
    if (!window_.isOpen()) {
        Logger::instance().error("Run called but window is not open. Did you call init()?");
        return;
    }

    running_ = true;
    frameClock_.restart();

    const float targetFrame =
        (cfg_.vsync || cfg_.targetFPS == 0) ? 0.f : 1.0f / static_cast<float>(cfg_.targetFPS);

    while (running_) {
        processEvents_();

        float dt = frameClock_.restart().asSeconds();

        if (onUpdate) onUpdate(dt, input_);

        renderFrame_(onRender);

        if (!cfg_.vsync && targetFrame > 0.f) {
            float elapsed = frameClock_.getElapsedTime().asSeconds();
            float toSleep = targetFrame - elapsed;
            if (toSleep > 0.0f) {
                std::this_thread::sleep_for(std::chrono::duration<float>(toSleep));
            }
        }
    }
}

void Engine::stop() {
    running_ = false;
}

void Engine::setClearColor(const sf::Color& c) {
    cfg_.clearColor = c;
    hasClearTexture_ = false;
    clearSprite_.reset();
}

bool Engine::setClearTexture(const std::string& path) {
    if (!clearTexture_.loadFromFile(path)) {
        Logger::instance().warn("Failed to load clear texture: " + path);
        hasClearTexture_ = false;
        clearSprite_.reset();
        return false;
    }

    clearSprite_.emplace(clearTexture_);

    auto size = window_.getSize();
    sf::Vector2f scale(
        static_cast<float>(size.x) / static_cast<float>(clearTexture_.getSize().x),
        static_cast<float>(size.y) / static_cast<float>(clearTexture_.getSize().y)
    );
    clearSprite_->setScale(scale);

    hasClearTexture_ = true;
    return true;
}

void Engine::shutdown() {
    Logger::instance().info("Engine shutdown...");
    if (window_.isOpen()) window_.close();
}

void Engine::processEvents_() {
    // 🧠 SFML 3: pollEvent() zwraca std::optional<sf::Event>, używamy .getIf<T>()
    while (auto e = window_.pollEvent()) {
        // Zamknięcie okna
        if (e->is<sf::Event::Closed>()) {
            Logger::instance().info("Window close requested.");
            stop();
        }

        // Zmiana rozmiaru
        if (auto r = e->getIf<sf::Event::Resized>()) {
            sf::FloatRect visible{
                {0.f, 0.f},
                { static_cast<float>(r->size.x), static_cast<float>(r->size.y) }
            };
            window_.setView(sf::View(visible));
        }

        // Klawiatura
        if (cfg_.enableKeyboard) {
            if (auto k = e->getIf<sf::Event::KeyPressed>()) {
                switch (k->code) {
                case sf::Keyboard::Key::W: input_.keyW = true; break;
                case sf::Keyboard::Key::A: input_.keyA = true; break;
                case sf::Keyboard::Key::S: input_.keyS = true; break;
                case sf::Keyboard::Key::D: input_.keyD = true; break;
                default: break;
                }
            }
            if (auto k = e->getIf<sf::Event::KeyReleased>()) {
                switch (k->code) {
                case sf::Keyboard::Key::W: input_.keyW = false; break;
                case sf::Keyboard::Key::A: input_.keyA = false; break;
                case sf::Keyboard::Key::S: input_.keyS = false; break;
                case sf::Keyboard::Key::D: input_.keyD = false; break;
                default: break;
                }
            }
        }

        // Mysz
        if (cfg_.enableMouse) {
            if (auto m = e->getIf<sf::Event::MouseMoved>()) {
                input_.mousePos = { m->position.x, m->position.y };
            }
            if (auto mb = e->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left)  input_.mouseLeft = true;
                if (mb->button == sf::Mouse::Button::Right) input_.mouseRight = true;
            }
            if (auto mb = e->getIf<sf::Event::MouseButtonReleased>()) {
                if (mb->button == sf::Mouse::Button::Left)  input_.mouseLeft = false;
                if (mb->button == sf::Mouse::Button::Right) input_.mouseRight = false;
            }
        }
    }
}

void Engine::renderFrame_(const std::function<void(sf::RenderWindow&)>& onRender) {
    if (hasClearTexture_ && clearSprite_) {
        window_.clear();
        window_.draw(*clearSprite_);
    }
    else {
        window_.clear(cfg_.clearColor);
    }

    if (onRender) onRender(window_);
    window_.display();
}

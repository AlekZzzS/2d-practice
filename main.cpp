#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <vector>

struct Snowflake { float x, y, speed, r; };

//дерево
void drawTree(sf::RenderWindow& w, float x, float y,
              float len, float angle, int depth)
{
    if (depth <= 0 || len < 2.f) return;
    float x2 = x + std::cos(angle) * len;
    float y2 = y - std::sin(angle) * len;
    sf::Vertex line[2] = {
        sf::Vertex{sf::Vector2f(x,  y),  sf::Color(200, 200, 200, 150)},
        sf::Vertex{sf::Vector2f(x2, y2), sf::Color(200, 200, 200, 150)}
    };
    w.draw(line, 2, sf::PrimitiveType::Lines);
    drawTree(w, x2, y2, len * 0.68f, angle + 0.42f, depth - 1);
    drawTree(w, x2, y2, len * 0.68f, angle - 0.42f, depth - 1);
}

//рука снеговика
void drawArm(sf::RenderWindow& w, float x, float y,
             float len, float angle, int depth)
{
    if (depth <= 0 || len < 3.f) return;
    float x2 = x + std::cos(angle) * len;
    float y2 = y - std::sin(angle) * len;
    sf::Vertex line[2] = {
        sf::Vertex{sf::Vector2f(x,  y),  sf::Color::White},
        sf::Vertex{sf::Vector2f(x2, y2), sf::Color::White}
    };
    w.draw(line, 2, sf::PrimitiveType::Lines);
    drawArm(w, x2, y2, len * 0.20f, angle + 0.65f, depth - 1);
    drawArm(w, x2, y2, len * 0.20f, angle - 0.45f, depth - 1);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Winter Snowman");
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int snowTarget = 150;
    std::vector<Snowflake> snow;
    snow.reserve(300);
    for (int i = 0; i < snowTarget; ++i)
        snow.push_back({ float(std::rand() % 800), float(std::rand() % 600),
                         20.f + float(std::rand() % 50), 1.f + float(std::rand() % 3) });

    float armAngle = 0.3f;
    float jumpTime = -1.f;
    float jumpY    = 0.f;

    bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional<sf::Event> ev = window.pollEvent())
        {
            if (ev->is<sf::Event::Closed>()) window.close();
            if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
                switch (kp->code) {
                    case sf::Keyboard::Key::Escape: window.close();          break;
                    case sf::Keyboard::Key::Space:
                        if (jumpTime < 0.f) jumpTime = 0.f;                  break;
                    case sf::Keyboard::Key::Up:    keyUp    = true;           break;
                    case sf::Keyboard::Key::Down:  keyDown  = true;           break;
                    case sf::Keyboard::Key::Left:  keyLeft  = true;           break;
                    case sf::Keyboard::Key::Right: keyRight = true;           break;
                    default: break;
                }
            }
            if (const auto* kr = ev->getIf<sf::Event::KeyReleased>()) {
                switch (kr->code) {
                    case sf::Keyboard::Key::Up:    keyUp    = false;          break;
                    case sf::Keyboard::Key::Down:  keyDown  = false;          break;
                    case sf::Keyboard::Key::Left:  keyLeft  = false;          break;
                    case sf::Keyboard::Key::Right: keyRight = false;          break;
                    default: break;
                }
            }
        }

        float dt = clock.restart().asSeconds();

        if (keyUp)   armAngle += 1.6f * dt;
        if (keyDown) armAngle -= 1.6f * dt;
        armAngle = std::clamp(armAngle, -0.8f, 1.3f);

        if (keyLeft)  snowTarget = std::max(20,  snowTarget - 2);
        if (keyRight) snowTarget = std::min(300, snowTarget + 2);

        if (jumpTime >= 0.f) {
            jumpTime += dt;
            constexpr float dur = 0.55f;
            jumpY = -std::sin(jumpTime / dur * 3.14159f) * 65.f;
            if (jumpTime >= dur) { jumpTime = -1.f; jumpY = 0.f; }
        }

        // Снег
        while ((int)snow.size() < snowTarget)
            snow.push_back({ float(std::rand() % 800), 0.f,
                             20.f + float(std::rand() % 50), 1.f + float(std::rand() % 3) });
        for (auto& s : snow) {
            s.y += s.speed * dt;
            if (s.y > 608.f) { s.y = -4.f; s.x = float(std::rand() % 800); }
        }

        window.clear(sf::Color(10, 12, 28));

        drawTree(window, 110, 575, 150.f, 1.57f, 7);

        //снег
        for (const auto& s : snow) {
            sf::CircleShape flake(s.r);
            flake.setFillColor(sf::Color::White);
            flake.setPosition({s.x, s.y});
            window.draw(flake);
        }

        //снеговик
        constexpr float cx    = 400.f;
        constexpr float botR  = 62.f;
        constexpr float midR  = 43.f;
        constexpr float headR = 28.f;

        const float botCY  = 498.f + jumpY;
        const float midCY  = 408.f + jumpY;
        const float headCY = 338.f + jumpY;

        //нижний шар
        sf::CircleShape bot(botR);
        bot.setFillColor(sf::Color::White);
        bot.setOrigin({botR, botR});
        bot.setPosition({cx, botCY});
        window.draw(bot);

        //средний шар
        sf::CircleShape mid(midR);
        mid.setFillColor(sf::Color::White);
        mid.setOrigin({midR, midR});
        mid.setPosition({cx, midCY});
        window.draw(mid);

        //руки
        const float leftAngle  = 3.14159f - armAngle;
        const float rightAngle = armAngle;
        drawArm(window, cx - midR, midCY, 50.f, leftAngle,  4);
        drawArm(window, cx + midR, midCY, 50.f, rightAngle, 4);

        //голова
        sf::CircleShape head(headR);
        head.setFillColor(sf::Color::White);
        head.setOrigin({headR, headR});
        head.setPosition({cx, headCY});
        window.draw(head);

        //шарф
        sf::RectangleShape scarf({86.f, 11.f});
        scarf.setFillColor(sf::Color(190, 30, 30));
        scarf.setOrigin({43.f, 5.5f});
        scarf.setPosition({cx, headCY + headR + 3.f});
        window.draw(scarf);

        //поля шляпы
        sf::RectangleShape brim({72.f, 8.f});
        brim.setFillColor(sf::Color(30, 30, 30));
        brim.setOrigin({36.f, 8.f});
        brim.setPosition({cx, headCY - headR + 1.f});
        window.draw(brim);

        //тулья шляпы
        sf::RectangleShape top({45.f, 40.f});
        top.setFillColor(sf::Color(30, 30, 30));
        top.setOrigin({22.5f, 40.f});
        top.setPosition({cx, headCY - headR + 1.f});
        window.draw(top);

        //глаза
        for (float ox : {-10.f, 10.f}) {
            sf::CircleShape eye(4.f);
            eye.setFillColor(sf::Color(10, 12, 28));
            eye.setOrigin({4.f, 4.f});
            eye.setPosition({cx + ox, headCY - 8.f});
            window.draw(eye);
        }

        //нос
        sf::RectangleShape nose({18.f, 6.f});
        nose.setFillColor(sf::Color(255, 115, 0));
        nose.setOrigin({0.f, 3.f});
        nose.setPosition({cx + 2.f, headCY + 4.f});
        window.draw(nose);

        //улыбка
        for (int i = -2; i <= 2; ++i) {
            sf::CircleShape dot(2.2f);
            dot.setFillColor(sf::Color(10, 12, 28));
            dot.setOrigin({2.2f, 2.2f});
            dot.setPosition({cx + i * 5.f,
                             headCY + 14.f - float(i * i) * 1.5f});
            window.draw(dot);
        }

        //пуговики
        for (int i = 0; i < 3; ++i) {
            sf::CircleShape btn(4.f);
            btn.setFillColor(sf::Color(10, 12, 28));
            btn.setOrigin({4.f, 4.f});
            btn.setPosition({cx, midCY - 16.f + i * 18.f});
            window.draw(btn);
        }

        window.display();
    }
    return 0;
}

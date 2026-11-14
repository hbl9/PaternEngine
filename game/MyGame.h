#pragma once

#include <game/IGame.h>
#include <cstdint>

class InputManager;
class SceneObject;
class Scene;

class MyGame : public IGame {
public:
    void OnInit(Scene& scene) override;

    void OnUpdate(float dt, Scene& scene, InputManager& im) override;

    void OnShutdown() override;

private:
    SceneObject* _player;
};
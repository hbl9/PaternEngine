#pragma once

#include <memory>

class Window;
class Scene;
class SceneObject;
class InputManager;

class EditorLayer
{
public:
    EditorLayer(Window& window, Scene& scene, InputManager& input);
    ~EditorLayer();

    void BeginFrame();
    void EndFrame();
    void Draw();

private:
    Window& _window;
    Scene& _scene;
    InputManager& _input;

    SceneObject* _selected = nullptr;

    void DrawHierarchy();
    void DrawInspector();
    void DrawContextMenu();
    void DrawObjectNode(SceneObject* obj);
};
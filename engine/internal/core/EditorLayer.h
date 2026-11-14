#pragma once

#include <scene/SceneHandle.h>
#include <cstdint>
#include <memory>

class InputManager;
class SceneObject;
class Window;
class Scene;

class EditorLayer
{
public:
    EditorLayer(Window& window, Scene& scene, InputManager& input);
    ~EditorLayer();

    void BeginFrame();
    void EndFrame();
    void Draw();

    std::uint64_t GetSelected() const;
    
    void SetSelected(std::uint64_t id);

private:
    Window& _window;
    Scene& _scene;
    InputManager& _input;
    std::uint64_t _selectedId = SceneHandle::INVALID;
    std::uint64_t _pendingDeleteObjId = SceneHandle::INVALID;

    void DrawHierarchy();
    void DrawPropertyPanel();
    void DrawObjectNode(std::uint64_t id);
};
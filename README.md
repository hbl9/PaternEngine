# PatternEngine

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-green)

[🇷🇺 Русская версия](README.ru.md)

**PatternEngine** is a lightweight game engine written from scratch in C++20 and OpenGL 3.3.
It provides a convenient API for creating 3D scenes, managing the camera, and features a built-in editor based on ImGui.

---

## Quick Start
**Requirements**: Windows 10/11, MSVC 2022+, Python 3.10+.

### Clone the repository
```bash
git clone https://github.com/YOUR_NICKNAME/PatternEngine.git
cd PatternEngine
```

### Build the engine
```bash
python generate_build.py
buildEngine.bat
```

After this, `engine.lib` will appear in `lib/`, and public headers will be generated in `include/`.

### Build the example game
```bash
python generate_game_bat.py
buildGame.bat
```

This creates `my_game.exe` (make sure `glfw3.dll` is placed next to it).

---

## How to use the engine in your project?
Copy the `include/` and `lib/` folders into your project.

### Visual Studio project setup
**C/C++ -> General -> Additional Include Directories:** Add `include/`

**Linker -> General -> Additional Library Directories:** Add `lib/`

**Linker -> Input -> Additional Dependencies:** Add `engine.lib`

Write your game class by inheriting from `IGame`:

```cpp
#include "game/IGame.h"
#include "scene/Scene.h"

class MyGame : public IGame
{
public:
    void OnInit(Scene& scene) override
    {
        std::uint64_t id = scene.CreateObject("Player", "cube", "plane");
        if (SceneObject* obj = scene.GetObjById(id)) {
            obj->SetColor(1.0f, 0.0f, 0.0f);
            obj->SetUniform("uColor", obj->GetColor());
        }
    }
    void OnUpdate(float dt, Scene& scene, InputManager& im) override {}
    void OnShutdown() override {}
};
```

### Run the engine in main.cpp

```cpp
#include "Engine.h"
#include "MyGame.h"

int main()
{
    return Engine::Run(std::make_unique<MyGame>());
}
```

---

## Editor Controls
- `F1` - Capture/release cursor.

- `W`, `A`, `S`, `D` - Camera movement.

- `Shift` - Speed boost.

- `Space` / `Ctrl` - Move up/down.

- `ПКМ` in Hierarchy panel - Context menu (create cube, empty object, delete).

- `ЛКМ` on object in scene or Hierarchy - Select object.

- Property Panel - Edit object's name, position, and scale.

---

## Project Structure:

- `engine/` - Engine source code (internal implementation).

- `include/` - Public headers (generated).

- `lib/` - Compiled static library (generated).

- `game/` - Example of engine usage (your game).

- `third_party/` - Third-party libraries (glfw, glad, glm, imgui).

---

**License: MIT.**
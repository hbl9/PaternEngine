# PatternEngine

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-green)

[🇬🇧 English version](README.md)

**PatternEngine** - это легковесный игровой движок на C++20 и OpenGL 3.3, написанный с нуля. 
Он предоставляет удобный API для создания 3D-сцен, управления камерой и встроенный редактор на базе ImGui.

---

## Быстрый старт
**Требования**: Windows 10/11, MSVC 2022+, Python 3.10+.

### Клонирование репозитория
```bash
git clone https://github.com/YOUR_NICKNAME/PatternEngine.git
cd PatternEngine
```

### Сборка движка
```bash
python generate_build.py
buildEngine.bat
```

После этого в `lib/` появится `engine.lib`, а в `include/` - публичные заголовочные файлы.

### Сборка игры
```bash
python generate_game_bat.py
buildGame.bat
```

После этого должен появится `my_game.exe` (файл `glfw3.dll` должен находится рядом с ним).

---

## Как использовать движок в своем проекте?
Скопируйте папки `include/` и `lib/` в свой проект.

### Настройка проекта в Visual Studio
**C/C++ -> Общие -> Дополнительные каталоги включаемых файлов:** добавьте `include/`

**Компоновщик -> Общие -> Дополнительные каталоги библиотек:** добавьте `lib/`

**Компоновщик -> Ввод -> Дополнительные зависимости:** добавьте `engine.lib`

Напишите класс игры, унаследовав его от `IGame`:

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

### Запуск движка в main.cpp

```cpp
#include "Engine.h"
#include "MyGame.h"

int main()
{
    return Engine::Run(std::make_unique<MyGame>());
}
```

---

## Управление в редакторе
- `F1` - захватить/отпустить управление.

- `W`, `A`, `S`, `D` - движение камеры.

- `Shift` - ускорение камеры.

- `Space` / `Ctrl` - движение вверх/вниз.

- `ПКМ` в панели Hierarchy - контекстное меню (создать куб, пустой объект, удалить).

- `ЛКМ` по объекту в сцене или в Hierarchy - выбрать объект.

- Панель Property Panel - изменить имя, позицию и масштаб объекта.

---

## Структура проекта:

- `engine/` - исходный код движка (внутренняя реализация).

- `include/` - публичные заголовочные файлы (генерируются).

- `lib/` - собранная статическая библиотека (генерируется).

- `game/` - пример использования движка (ваша игра).

- `third_party/` - сторонние библиотеки (glfw, glad, glm, imgui).

---

**Лицензия: MIT.**
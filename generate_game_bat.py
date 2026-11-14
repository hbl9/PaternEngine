import os

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

bat_content = f"""@echo off
set ENGINE_ROOT=%~dp0
set TP=%ENGINE_ROOT%engine\\third_party

echo Building Example Game...
cl /std:c++20 /EHsc /MD /O2 /W3 /nologo ^
   /I"%ENGINE_ROOT%include" ^
   /I"%TP%" ^
   /I"%TP%\\glfw\\include" ^
   /I"%TP%\\glad\\include" ^
   /I"%TP%\\imgui" ^
   game\\main.cpp game\\MyGame.cpp ^
   /Fe:my_game.exe ^
   /link "%ENGINE_ROOT%lib\\engine.lib" ^
         "%TP%\\glfw\\lib\\glfw3dll.lib" ^
         "%TP%\\glad\\lib\\glad.lib" ^
         opengl32.lib

if errorlevel 1 (
    echo [ERROR] Game build failed
    exit /b 1
)
echo [OK] my_game.exe created!
"""

with open(os.path.join(ROOT_DIR, "buildGame.bat"), "w", encoding="utf-8") as f:
    f.write(bat_content)
print("Successfully generated buildGame.bat!")
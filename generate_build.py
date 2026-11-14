import os
import shutil

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
ENGINE_DIR = os.path.join(ROOT_DIR, "engine")
INCLUDE_DIR = os.path.join(ROOT_DIR, "include")

def get_cpp_files(directory, exclude_dirs=None):
    if not os.path.exists(directory):
        return []
    cpp_files = []
    for root, dirs, files in os.walk(directory):
        if exclude_dirs:
            dirs[:] = [d for d in dirs if os.path.join(root, d) not in exclude_dirs]
        for file in files:
            if file.endswith(".cpp"):
                rel_path = os.path.relpath(os.path.join(root, file), ROOT_DIR)
                cpp_files.append(rel_path.replace("\\", "/"))
    return cpp_files

def copy_public_headers():
    """Copies only .h files from engine/public to include"""
    print("Copying header files...")
    
    # Clear the include folder before copying
    if os.path.exists(INCLUDE_DIR):
        shutil.rmtree(INCLUDE_DIR)
    os.makedirs(INCLUDE_DIR, exist_ok=True)

    public_dir = os.path.join(ENGINE_DIR, "public")
    
    # We go through all the files in engine/public
    for root, dirs, files in os.walk(public_dir):
        for file in files:
            # Copying only .h files
            if file.endswith(".h"):
                src_path = os.path.join(root, file)
                
                # Calculating the path relative to engine/public
                rel_path = os.path.relpath(src_path, public_dir)
                
                # Define the destination path in the include folder
                dst_path = os.path.join(INCLUDE_DIR, rel_path)
                
                # Creating necessary folders (Example, include/scene/)
                os.makedirs(os.path.dirname(dst_path), exist_ok=True)
                
                # Copy file
                shutil.copy2(src_path, dst_path)
                print(f"  -> {rel_path}")

    print(f"Headlines copied to {INCLUDE_DIR}")

def generate_bat():
    exclude = [os.path.join(ENGINE_DIR, "third_party")]
    engine_sources = get_cpp_files(ENGINE_DIR, exclude_dirs=exclude)
    
    imgui_sources = [
        "engine/third_party/imgui/imgui.cpp",
        "engine/third_party/imgui/imgui_draw.cpp",
        "engine/third_party/imgui/imgui_tables.cpp",
        "engine/third_party/imgui/imgui_widgets.cpp",
        "engine/third_party/imgui/imgui_impl_glfw.cpp",
        "engine/third_party/imgui/imgui_impl_opengl3.cpp"
    ]
    
    engine_list = " ^\n ".join(engine_sources)
    imgui_list = " ^\n ".join(imgui_sources)
    all_sources = engine_list + " ^\n " + imgui_list

    bat_content = f"""@echo off
set ENGINE_ROOT=%~dp0
set OBJ_DIR=%ENGINE_ROOT%obj
set LIB_DIR=%ENGINE_ROOT%lib
set TP=%ENGINE_ROOT%engine\\third_party

set CXXFLAGS=/std:c++20 /EHsc /MD /O2 /W3 /nologo /c
set INCLUDES=/I"%ENGINE_ROOT%engine" /I"%ENGINE_ROOT%engine\\public" /I"%ENGINE_ROOT%engine\\internal" /I"%TP%" /I"%TP%\\glad\\include" /I"%TP%\\glfw\\include" /I"%TP%\\imgui"

if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"
if not exist "%LIB_DIR%" mkdir "%LIB_DIR%"

echo [1/2] Compiling Engine and ImGui...
cl %CXXFLAGS% %INCLUDES% ^
 {all_sources} ^
 /Fo"%OBJ_DIR%\\\\" || goto :error

echo [2/2] Creating engine.lib...
lib /NOLOGO /OUT:"%LIB_DIR%\\engine.lib" "%OBJ_DIR%\\*.obj" || goto :error

echo [OK] Built %LIB_DIR%\\engine.lib
exit /b 0

:error
echo [ERROR] Build failed.
exit /b 1
"""
    with open(os.path.join(ROOT_DIR, "buildEngine.bat"), "w", encoding="utf-8") as f:
        f.write(bat_content)
    print("Successfully generated buildEngine.bat!")

if __name__ == "__main__":
    generate_bat()
    copy_public_headers()
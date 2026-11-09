#include "../../platform/InputManager.h"
#include "../../platform/Window.h"
#include "../../scene/Scene.h"
#include "../../scene/SceneObject.h"
#include "EditorLayer.h"
#include <imgui.h>
#include <vector>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

EditorLayer::EditorLayer(Window& window, Scene& scene, InputManager& input) :
	_window(window), _scene(scene), _input(input)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(_window.GetWindow(), false);
	ImGui_ImplOpenGL3_Init("#version 330");
}

EditorLayer::~EditorLayer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorLayer::BeginFrame()
{
	GLFWwindow* w = _window.GetWindow();
	ImGuiIO& io = ImGui::GetIO();

	if (_window.IsCursorCaptured())
	{
		io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
		for (int i = 0; i < 3; ++i)
			io.AddMouseButtonEvent(i, false);
	}
	else
	{
		double mx, my;
		glfwGetCursorPos(w, &mx, &my);
		io.AddMousePosEvent(static_cast<float>(mx), static_cast<float>(my));
		for (int i = 0; i < 3; i++)
			io.AddMouseButtonEvent(i, glfwGetMouseButton(w, i) == GLFW_PRESS);
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EditorLayer::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorLayer::Draw()
{
	DrawHierarchy();
	DrawInspector();
	DrawContextMenu();
}

void EditorLayer::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	std::vector<SceneObject*> roots = _scene.GetRoots();
	for (SceneObject* root : roots)
		DrawObjectNode(root);

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		_selected = nullptr;

	ImGui::End();
}

void EditorLayer::DrawInspector()
{

}

void EditorLayer::DrawContextMenu()
{

}

void EditorLayer::DrawObjectNode(SceneObject* obj)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (obj->GetChildren().empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (_selected == obj)
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::PushID(obj);

	bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(obj), flags, "%s", obj->GetName().c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		_selected = obj;

	if (ImGui::BeginPopupContextItem("obj_ctx"))
	{
		if (ImGui::MenuItem("Create object")) 
			_scene.CreateObject("object", "cube", "basic", glm::vec3{ 0.0f }, obj);
		if (ImGui::MenuItem("Delete"))
		{
			_scene.RemoveObject(_selected);
			_selected = nullptr;
		}
		ImGui::EndPopup();
	}

	if (open)
	{
		for (SceneObject* child : obj->GetChildren())
			DrawObjectNode(child);
		ImGui::TreePop();
	}

	ImGui::PopID();
}
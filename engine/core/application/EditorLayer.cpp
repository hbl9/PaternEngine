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
#include <string>
#include <glm/vec3.hpp>

EditorLayer::EditorLayer(Window& window, Scene& scene, InputManager& input) :
	_window(window), _scene(scene), _input(input)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(_window.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	_scene.AddDestroyListener([this](std::uint64_t id)
		{
			if (_selectedId == id) _selectedId = SceneHandle::INVALID;
			if (_pendingDeleteObjId == id) _pendingDeleteObjId = SceneHandle::INVALID;
		});
}

EditorLayer::~EditorLayer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorLayer::BeginFrame()
{
	if (_pendingDeleteObjId != SceneHandle::INVALID)
		_scene.RemoveObject(_pendingDeleteObjId);

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
	DrawPropertyPanel();
}

void EditorLayer::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	if (_window.IsCursorCaptured())
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Camera locked - press F1 to release");
	else
		ImGui::TextDisabled("F1 - capture camera");

	ImGui::Separator();

	if (ImGui::BeginPopupContextWindow("hierarchy_ctx",
		ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
	{
		if (ImGui::MenuItem("Create Cube")) _scene.CreateObject("Cube_obj", "cube", "basic");
		if (ImGui::MenuItem("Create Empty")) _scene.CreateDot("Dot_obj");
		ImGui::EndPopup();
	}

	for (std::uint64_t rootId : _scene.GetRootsId())
		DrawObjectNode(rootId);

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		SetSelected(SceneHandle::INVALID);

	ImGui::End();
}

void EditorLayer::DrawPropertyPanel()
{
	ImGui::Begin("Property Panel");

	SceneObject* obj = _scene.GetObjById(_selectedId);
	if (!obj)
	{
		ImGui::TextDisabled("Nothing selected");
		ImGui::End();
		return;
	}

	char buf[256];
	std::snprintf(buf, sizeof(buf), "%s", obj->GetName().c_str());
	if (ImGui::InputText("Name", buf, sizeof(buf)))
		obj->SetName(buf);

	glm::vec3 pos = obj->GetLocalPosition();
	if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
		obj->SetLocalPosition(pos);

	glm::vec3 scale = obj->GetLocalScale();
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
		obj->SetLocalScale(scale);

	if (SceneObject* parent = obj->GetParent(); parent != nullptr)
		ImGui::Text("Parent: %s", parent->GetName().c_str());
	else
		ImGui::TextDisabled("Root");

	ImGui::End();
}

void EditorLayer::DrawObjectNode(std::uint64_t id)
{
	SceneObject* obj = _scene.GetObjById(id);

	if (!obj) return;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	std::vector<std::uint64_t> children = _scene.GetChildren(id);

	if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
	if (_selectedId == id) flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::PushID(static_cast<int>(id));

	bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(id), flags, "%s", obj->GetName().c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		SetSelected(id);

	if (ImGui::BeginPopupContextItem("obj_ctx"))
	{
		if (ImGui::MenuItem("Create child Cube")) 
			_scene.CreateObject("Cube_obj", "cube", "basic", glm::vec3(0.0f), id);
		if (ImGui::MenuItem("Create child Empty")) 
			_scene.CreateDot("Dot_obj", glm::vec3(0.0f), id);
		if (ImGui::MenuItem("Delete"))
			_pendingDeleteObjId = id;
		ImGui::EndPopup();
	}

	if (open)
	{
		for (std::uint64_t child : children)
			DrawObjectNode(child);
		ImGui::TreePop();
	}

	ImGui::PopID();
}

std::uint64_t EditorLayer::GetSelected() const { return _selectedId; }

void EditorLayer::SetSelected(std::uint64_t id)
{
	if (id != SceneHandle::INVALID && _scene.Contains(id))
	{
		if (_selectedId != SceneHandle::INVALID)
			if (SceneObject* prev = _scene.GetObjById(_selectedId); prev != nullptr)
				prev->SetSelected(false);

		_selectedId = id;

		if (_selectedId != SceneHandle::INVALID)
			if (SceneObject* cur = _scene.GetObjById(_selectedId); cur != nullptr)
				cur->SetSelected(true);
	}
}
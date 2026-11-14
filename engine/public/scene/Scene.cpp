#include "Scene.h"
#include <graphics/ResourceManager.h>
#include <graphics/ShaderProgram.h>
#include <scene/SceneObject.h>
#include <graphics/Mesh.h>
#include <algorithm>
#include <iostream>
#include <utility>

Scene::Scene() : _resources(std::make_unique<ResourceManager>()) {}
Scene::~Scene() = default;

std::uint64_t Scene::CreateObject(const char* oName, const char* mName, const char* sName, const glm::vec3& pos, std::uint64_t p)
{
	Mesh* mesh = _resources->GetMesh(mName);
	ShaderProgram* shader = _resources->GetShader(sName);

	std::unique_ptr<SceneObject> obj = std::make_unique<SceneObject>(_nextId, oName, mesh, shader);
	obj->SetLocalPosition(pos);
	if (p != SceneHandle::INVALID && Contains(p))
		obj->SetParent(GetObjById(p));

	_objects.emplace(_nextId, std::move(obj));
	_order.push_back(_nextId);
	return _nextId++;
}

std::uint64_t Scene::CreateDot(const char* name, const glm::vec3& pos, std::uint64_t p)
{
	std::unique_ptr<SceneObject> dot = std::make_unique<SceneObject>(_nextId, name);
	dot->SetLocalPosition(pos);
	if (p != SceneHandle::INVALID && Contains(p))
		dot->SetParent(GetObjById(p));
	_objects.emplace(_nextId, std::move(dot));
	_order.push_back(_nextId);
	return _nextId++;
}

ResourceManager& Scene::GetResources() { return *_resources; }
std::uint64_t Scene::GetFirstByName(const std::string& name) const
{
	for (std::uint64_t id : _order)
		if (GetObjById(id)->GetName() == name)
			return id;
	return SceneHandle::INVALID;
}
SceneObject* Scene::GetObjById(std::uint64_t id)
{
	auto obj = _objects.find(id);
	return obj == _objects.end() ? nullptr : obj->second.get();
}
const SceneObject* Scene::GetObjById(std::uint64_t id) const
{
	auto obj = _objects.find(id);
	return obj == _objects.end() ? nullptr : obj->second.get();
}
std::vector<std::uint64_t> Scene::GetRootsId() const
{
	std::vector<std::uint64_t> roots;
	for (std::uint64_t id : _order)
		if (!GetObjById(id)->GetParent())
			roots.push_back(id);
	return roots;
}
std::vector<std::uint64_t> Scene::GetChildren(std::uint64_t id) const
{
	std::vector<std::uint64_t> children;
	const SceneObject* obj = GetObjById(id);
	if (obj)
		for (SceneObject* child : obj->GetChildren())
			children.push_back(child->GetId());
	return children;
}

bool Scene::Contains(std::uint64_t id) const { return _objects.contains(id); }

void Scene::AddDestroyListener(std::function<void(std::uint64_t)> dl) { _destroyListeners.push_back(std::move(dl)); }

void Scene::ClearObjects()
{
	std::vector<std::uint64_t> roots = GetRootsId();
	for (std::uint64_t id : roots)
		RemoveObject(id);

	while (!_objects.empty())
		RemoveObject(_objects.begin()->first);
}

void Scene::RemoveObject(std::uint64_t id)
{
	if (!Contains(id))
	{
		std::cout << "[Scene] Object to delete with ID=" << id << " not found\n";
		return;
	}

	std::vector<std::uint64_t> toRemove;
	CollectSubtree(id, toRemove);

	for (std::uint64_t childId : toRemove)
		NotifyDestroyed(childId);

	for (std::uint64_t childId : toRemove)
		if (SceneObject* obj = GetObjById(childId); obj != nullptr)
		{
			obj->SetParent(nullptr);
			obj->GetChildren().clear();
		}

	for (std::uint64_t childId : toRemove)
		_objects.erase(childId);

	std::erase_if(_order, [&](std::uint64_t id)
		{ return std::find(toRemove.begin(), toRemove.end(), id) != toRemove.end(); });
}

void Scene::CollectSubtree(std::uint64_t id, std::vector<std::uint64_t>& out) const
{
	const SceneObject* obj = GetObjById(id);
	if (!obj) return;

	out.push_back(id);
	for (SceneObject* child : obj->GetChildren())
		CollectSubtree(child->GetId(), out);
}

void Scene::Render(const glm::mat4& view, const glm::mat4& projection)
{
	for (const auto& obj : _objects)
		obj.second->Draw(view, projection);
}

void Scene::NotifyDestroyed(std::uint64_t id)
{
	for (auto& dl : _destroyListeners)
		dl(id);
}
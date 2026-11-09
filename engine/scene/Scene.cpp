#include "../graphics/Mesh.h"
#include "../graphics/ResourceManager.h"
#include "../graphics/ShaderProgram.h"
#include "SceneObject.h"
#include "Scene.h"
#include <algorithm>
#include <utility>

Scene::Scene() : _resources(std::make_unique<ResourceManager>()) {}
Scene::~Scene() = default;

SceneObject* Scene::CreateObject(const char* oName, const char* mName, const char* sName, const glm::vec3& pos, SceneObject* p)
{
	Mesh* mesh = _resources->GetMesh(mName);
	ShaderProgram* shader = _resources->GetShader(sName);

	std::unique_ptr<SceneObject> obj = std::make_unique<SceneObject>(oName, mesh, shader);
	obj->SetLocalPosition(pos);
	obj->SetParent(p);

	_objects.push_back(std::move(obj));
	return (_objects.end() - 1)->get();
}

SceneObject* Scene::CreateDot(const char* name, const glm::vec3& pos, SceneObject* p)
{
	std::unique_ptr<SceneObject> dot = std::make_unique<SceneObject>(name);
	dot->SetLocalPosition(pos);
	dot->SetParent(p);
	_objects.push_back(std::move(dot));
	return (_objects.end() - 1)->get();
}

ResourceManager& Scene::GetResources() { return *_resources; }
SceneObject* Scene::GetFirstOrNull(const std::string& name)
{
	for (const auto& obj : _objects)
		if (obj->GetName() == name)
			return obj.get();
	return nullptr;
}
std::vector<SceneObject*> Scene::GetRoots() const
{
	std::vector<SceneObject*> roots;
	for (auto& obj : _objects)
		if (!obj->GetParent())
			roots.push_back(obj.get());
	return roots;
}

void Scene::ClearObjects() { _objects.clear(); }
void Scene::RemoveObject(SceneObject* obj)
{
	if (!obj) return;

	std::vector<SceneObject*> toRemove;
	CollectChildren(obj, toRemove);

	std::erase_if(_objects, [&](const std::unique_ptr<SceneObject>& p)
		{ return std::find(toRemove.begin(), toRemove.end(), p.get()) != toRemove.end(); });
}

void Scene::CollectChildren(SceneObject* obj, std::vector<SceneObject*>& out) const
{
	if (!obj) return;

	out.push_back(obj);
	for (SceneObject* child : obj->GetChildren())
		CollectChildren(child, out);
}

void Scene::Draw(const glm::mat4& view, const glm::mat4& projection)
{
	for (const auto& obj : _objects)
		obj->Draw(view, projection);
}
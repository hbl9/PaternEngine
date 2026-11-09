#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <memory>
#include <string>
#include <vector>

class ResourceManager;
class SceneObject;

class Scene
{
public:
	Scene();
	~Scene();

	/// <summary>
	/// Creates SceneObject and adds it to the list of objects.
	/// </summary>
	/// <param name="oName">Object name</param>
	/// <param name="mName">Mesh name (ResourceManager key)</param>
	/// <param name="sName">Shader name (ResourceManager key)</param>
	/// <param name="pos">Object center in parent-local coordinates</param>
	/// <param name="p">Object parent (can be nullptr)</param>
	/// <returns>Pointer to the created object, or nullptr on failure</returns>
	SceneObject* CreateObject(const char* oName, const char* mName, const char* sName,
		const glm::vec3& pos = glm::vec3(0.0f), SceneObject* p = nullptr);

	SceneObject* CreateDot(const char* name, const glm::vec3& pos = glm::vec3(0.0f), SceneObject* p = nullptr);

	ResourceManager& GetResources();
	SceneObject* GetFirstOrNull(const std::string& name);
	std::vector<SceneObject*> GetRoots() const;

	void ClearObjects();
	void RemoveObject(SceneObject* obj);

	void CollectChildren(SceneObject* obj, std::vector<SceneObject*>& out) const;

	void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
	std::unique_ptr<ResourceManager> _resources = nullptr;
	std::vector<std::unique_ptr<SceneObject>> _objects;
};
#pragma once

#include <scene/SceneHandle.h>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <functional>
#include <cstdint>
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
	std::uint64_t CreateObject(const char* oName, const char* mName, const char* sName,
		const glm::vec3& pos = glm::vec3(0.0f), std::uint64_t p = SceneHandle::INVALID);

	std::uint64_t CreateDot(const char* name, const glm::vec3& pos = glm::vec3(0.0f), std::uint64_t p = SceneHandle::INVALID);

	ResourceManager& GetResources();
	std::uint64_t GetFirstByName(const std::string& name) const;
	SceneObject* GetObjById(std::uint64_t id);
	const SceneObject* GetObjById(std::uint64_t id) const;
	std::vector<std::uint64_t> GetRootsId() const;
	std::vector<std::uint64_t> GetChildren(std::uint64_t id) const;

	bool Contains(std::uint64_t id) const;

	void AddDestroyListener(std::function<void(std::uint64_t)> dl);

	void ClearObjects();
	void RemoveObject(std::uint64_t id);

	void Render(const glm::mat4& view, const glm::mat4& projection);

private:
	std::unique_ptr<ResourceManager> _resources = nullptr;
	std::unordered_map<std::uint64_t, std::unique_ptr<SceneObject>> _objects;
	std::vector<std::uint64_t> _order;
	std::vector<std::function<void(std::uint64_t)>> _destroyListeners;
	std::uint64_t _nextId = 1;

	void CollectSubtree(std::uint64_t id, std::vector<std::uint64_t>& out) const;
	void NotifyDestroyed(std::uint64_t id);
};
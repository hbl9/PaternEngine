#include "SceneHandle.h"

bool SceneHandle::IsValid() const { return id != INVALID; }
SceneHandle::operator bool() const { return IsValid(); }

bool SceneHandle::operator==(const SceneHandle& other) const { return id == other.id; }
bool SceneHandle::operator!=(const SceneHandle& other) const { return id != other.id; }
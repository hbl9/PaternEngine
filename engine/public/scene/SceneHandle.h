#pragma once

#include <cstdint>

struct SceneHandle
{
	static constexpr std::uint64_t INVALID = 0;

	std::uint64_t id = INVALID;

	bool IsValid() const;
	explicit operator bool() const;

	bool operator==(const SceneHandle& other) const;
	bool operator!=(const SceneHandle& other) const;
};
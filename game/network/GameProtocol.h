#pragma once

#include <span>
#include <vector>
#include <cstdint>
#include <glm/vec3.hpp>

struct PlayerState 
{
	std::uint32_t id = 0;
	glm::vec3 pos{ 0.0f };
	float yaw = 0.0f, pitch = 0.0f;
};

namespace GameProtocol
{
	constexpr std::uint32_t MAGIC = 0x47504C59;
	constexpr std::size_t STATE_PACKET_SIZE = 28;

	std::vector<std::uint8_t> SerializeState(const PlayerState& ps);
	bool DeserializeState(const std::uint8_t* data, std::size_t size, PlayerState& outState);
	bool DeserializeState(std::span<const std::uint8_t> bytes, PlayerState& outState);
}
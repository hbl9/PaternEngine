#pragma once

#include <span>
#include <cstdint>
#include <glm/vec3.hpp>

struct PlayerState 
{
	uint32_t id = 0;
	glm::vec3 pos{ 0.0f };
	float yaw = 0.0f, pitch = 0.0f;
};

namespace GameProtocol
{
	constexpr uint32_t MAGIC = 0x47504C59;
	constexpr std::size_t STATE_PACKET_SIZE = 28;

	std::vector<uint8_t> SerializeState(const PlayerState& ps);
	bool DeserializeState(const uint8_t* data, size_t size, PlayerState& outState);
	bool DeserializeState(std::span<const uint8_t> bytes, PlayerState& outState);
}
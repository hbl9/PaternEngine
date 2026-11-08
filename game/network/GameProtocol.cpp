#include "GameProtocol.h"

namespace GameProtocol
{
	std::vector<uint8_t> SerializeState(const PlayerState& ps)
	{
		std::vector<uint8_t> buf(STATE_PACKET_SIZE);
		std::size_t off = 0;

		std::memcpy(buf.data() + off, &MAGIC, 4); off += 4;
		std::memcpy(buf.data() + off, &ps.id, 4); off += 4;
		std::memcpy(buf.data() + off, &ps.pos.x, 4); off += 4;
		std::memcpy(buf.data() + off, &ps.pos.y, 4); off += 4;
		std::memcpy(buf.data() + off, &ps.pos.z, 4); off += 4;
		std::memcpy(buf.data() + off, &ps.yaw, 4); off += 4;
		std::memcpy(buf.data() + off, &ps.pitch, 4); off += 4;

		return buf;
	}

	bool DeserializeState(const uint8_t* data, size_t size, PlayerState& outState)
	{
		if (size >= STATE_PACKET_SIZE)
		{
			uint32_t magic = 0;
			std::memcpy(&magic, data, 4);

			if (magic == MAGIC)
			{
				std::size_t off = 4;
				std::memcpy(&outState.id, data + off, 4); off += 4;
				std::memcpy(&outState.pos.x, data + off, 4); off += 4;
				std::memcpy(&outState.pos.y, data + off, 4); off += 4;
				std::memcpy(&outState.pos.z, data + off, 4); off += 4;
				std::memcpy(&outState.yaw, data + off, 4); off += 4;
				std::memcpy(&outState.pitch, data + off, 4); off += 4;

				return true;
			}
		}
		return false;
	}
	bool DeserializeState(std::span<const uint8_t> bytes, PlayerState& outState) { return DeserializeState(bytes.data(), bytes.size(), outState); }
}
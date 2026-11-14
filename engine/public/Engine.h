#pragma once

#include <camera/ICameraController.h>
#include <game/IGame.h>
#include <string>
#include <memory>

namespace Engine
{
	struct Config
	{
		int width = 800;
		int height = 800;
		std::string title = "PaternEngine";
		bool showEditor = true;
	};

	int Run(std::unique_ptr<IGame> game, std::unique_ptr<ICameraController> cameraController = nullptr, const Config& cfg = {});
	int Run(const Config& cfg = {});
}
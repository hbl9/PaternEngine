#include "Engine.h"
#include <core/Application.h>
#include <iostream>
#include <memory>

int Engine::Run(std::unique_ptr<IGame> game, std::unique_ptr<ICameraController> cameraController, const Config& cfg)
{
	Application app(cfg.width, cfg.height, cfg.title, cfg.showEditor);

	if (!app.Init(std::move(game), std::move(cameraController)))
	{
		std::cout << "[Engine] Initialization failed\n";
		return 1;
	}
	app.Run();

	return 0;
}

int Engine::Run(const Config& cfg) { return Run(nullptr, nullptr, cfg); }
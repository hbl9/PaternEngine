#include "MyGame.h"
#include <Engine.h>
#include <clocale>

int main() {
	setlocale(LC_ALL, ".UTF8");

	Engine::Config cfg;
	cfg.showEditor = false;

	// Moving our game into the engine
	return Engine::Run(std::make_unique<MyGame>(), nullptr, cfg);
}
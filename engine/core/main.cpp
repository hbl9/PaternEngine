#include "..\..\game\Game.h"
#include "Application.h"
#include <clocale>
#include <iostream>

int main(int argc, char** argv)
{
	setlocale(LC_ALL, ".UTF8");

	bool isHost = false;
	const char* connectIp = nullptr;

	for (int i = 1; i < argc; ++i)
		if (strcmp(argv[i], "--host") == 0) isHost = true;
		else if (strcmp(argv[i], "--connect") == 0 && i + 1 < argc) connectIp = argv[++i];

	Application app(800, 800, "PatternEngine");

	std::unique_ptr<Game> game = std::make_unique<Game>(isHost, connectIp);
	if (!app.Init(std::move(game)))
	{
		std::cout << "Ошибка инициализации\n";
		return 1;
	}
	app.Run();

	return 0;
}
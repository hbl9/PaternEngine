#include "Engine.h"
#include <clocale>

int main()
{
	setlocale(LC_ALL, ".UTF8");

	Engine::Run();

	return 0;
}
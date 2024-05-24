#include <iostream>
#include <windows.h>
#include "Repository.h"
#include "App.h"

using namespace Model;



int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	App app;
	app.Run();
}

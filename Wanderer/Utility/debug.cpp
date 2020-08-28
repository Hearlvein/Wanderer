#include "Utility/debug.hpp"

std::ofstream logFile("debug.txt");

void openLog()
{
	logFile.open("debug.txt");
	logFile << "hello world" << std::flush;
}

void closeLog()
{
	logFile.close();
}

#pragma once

#include <iostream>
#include <fstream>
#include <chrono>

extern std::ofstream logFile;

void openLog();
void closeLog();

struct DebugTimer
{
	DebugTimer() { start = std::chrono::high_resolution_clock::now(); }
	~DebugTimer()
    {
        std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
        logFile << "\ttook " << duration.count() << "ms\n" << std::flush;
    }
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

#ifdef __PRETTY_FUNCTION__
#define L std::cerr << __PRETTY_FUNCTION__ << " (line " << __LINE__ << ")\n" << std::flush;
#else
#define L std::cerr << __FUNCSIG__ << " (line " << __LINE__ << ")\n" << std::flush;
#endif

#define DEBUG_FC DebugTimer timer;

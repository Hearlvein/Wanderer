#ifndef DEBUG_HPP
#define DEBUG_HPP

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
        logFile << "\t took " << duration.count() << "ms\n" << std::flush;
    }
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

#endif

#ifndef DEBUG_FC
#define DEBUG_FC DebugTimer timer; LINE_TRACK
#endif

#ifndef LINE_TRACK
#define LINE_TRACK logFile << __PRETTY_FUNCTION__ << " (line " << __LINE__ << ")\n" << std::flush;
#endif

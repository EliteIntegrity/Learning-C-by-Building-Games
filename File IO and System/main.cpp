// =============================================================================
// SDL3 Tutorial: File I/O and System Information
// =============================================================================
//
// WHAT THIS DEMO COVERS
//   Part 1 - System info utilities
//     SDL_GetPlatform()        returns the OS name as a string
//     SDL_GetCPUCount()        returns the number of logical CPU cores
//     SDL_GetSystemRAM()       returns total RAM in megabytes
//     SDL_GetCPUCacheLineSize() returns the CPU cache line size in bytes
//
//   Part 2 - File I/O utilities
//     SDL_GetPrefPath()        gets the correct user-writable save folder
//     SDL_LoadFile()           reads an entire file into memory in one call
//     SDL_SaveFile()           writes a buffer to a file in one call
//
// WHY USE SDL INSTEAD OF <fstream> OR fopen()?
//   On Windows the practical difference is small, but SDL_GetPrefPath() is
//   the standout reason: on Android, iOS, and game consoles each platform
//   has its own rules about where apps are allowed to write files. SDL hands
//   you the right directory automatically so the same code works everywhere
//   without a single #ifdef. For a Windows-only project it is still a clean
//   habit, and SDL_LoadFile/SDL_SaveFile are genuinely more convenient than
//   the standard library equivalents for small text files.
//
// WHAT THE PROGRAM DOES
//   On the first run it collects CPU/RAM info, saves it to a text file, and
//   writes "Times written = 1" at the bottom. On every run after that it
//   reads the file back, finds that line, increments the number, and saves
//   the updated file. Watch the counter grow each time you run the app.
// =============================================================================

#include <SDL3/SDL.h>
#include <string>
#include <sstream>

int main(int argc, char* argv[])
{
    // -------------------------------------------------------------------------
    // PART 1: SYSTEM INFORMATION
    //
    // These four functions query the OS directly and need no SDL_Init call.
    // They are useful anywhere you want to log what machine your app is
    // running on -- handy for bug reports or adaptive quality settings.
    //
    // SDL_Log() is SDL's cross-platform print function. On Windows it writes
    // to the debugger output and stderr. On Android it goes to logcat.
    // Using SDL_Log keeps your logging portable alongside the rest of SDL.
    // -------------------------------------------------------------------------
    SDL_Log("=== System Information ===");
    SDL_Log("Platform  : %s",    SDL_GetPlatform());           // e.g. "Windows"
    SDL_Log("CPU cores : %d",    SDL_GetNumLogicalCPUCores());           // logical cores (includes hyperthreading)
    SDL_Log("RAM       : %d MB", SDL_GetSystemRAM());          // total installed RAM
    SDL_Log("Cache line: %d B",  SDL_GetCPUCacheLineSize());   // CPU L1 cache line in bytes
    SDL_Log("");

    // -------------------------------------------------------------------------
    // PART 2: GET THE SAVE DIRECTORY WITH SDL_GetPrefPath
    //
    // SDL_GetPrefPath(organisation, application) returns the platform-correct
    // folder for saving user data. On Windows this looks like:
    //   C:\Users\<you>\AppData\Roaming\MyOrg\FileIODemo\
    //
    // The folder is created automatically if it does not exist yet.
    //
    // IMPORTANT -- memory ownership:
    //   SDL allocates the string internally and hands you a pointer to it.
    //   You are responsible for freeing it with SDL_free() when you are done.
    //   Forgetting to do this is a memory leak.
    // -------------------------------------------------------------------------
    char* prefPath = SDL_GetPrefPath("MyOrg", "FileIODemo");
    if (!prefPath)
    {
        SDL_Log("ERROR: Could not get pref path: %s", SDL_GetError());
        return 1;
    }

    std::string filePath = std::string(prefPath) + "system_info.txt";
    SDL_free(prefPath);   // done with the SDL-allocated string -- free it now

    SDL_Log("Save location: %s", filePath.c_str());
    SDL_Log("");

    // -------------------------------------------------------------------------
    // PART 3: READ THE EXISTING FILE WITH SDL_LoadFile
    //
    // SDL_LoadFile(path, &outSize) reads the whole file into a single buffer
    // that SDL allocates for you. It also appends a null terminator so you
    // can treat the buffer as a C string safely.
    //
    // Return value:
    //   Non-null pointer  -- success, outSize holds the byte count
    //   nullptr           -- file not found or read error (first run = fine)
    //
    // IMPORTANT -- memory ownership:
    //   Same rule as SDL_GetPrefPath: SDL allocated the buffer, so free it
    //   with SDL_free() once you have copied out what you need.
    // -------------------------------------------------------------------------
    int timesWritten = 0;

    size_t fileSize = 0;
    void*  fileData = SDL_LoadFile(filePath.c_str(), &fileSize);

    if (fileData)
    {
        // Wrap the raw buffer in a std::string so we can use find() on it,
        // then immediately free the SDL buffer -- we no longer need it.
        std::string content(static_cast<char*>(fileData), fileSize);
        SDL_free(fileData);

        // Search for the counter line we wrote on the previous run.
        // Using a fixed prefix makes it easy to locate reliably.
        const std::string marker = "Times written = ";
        size_t pos = content.find(marker);
        if (pos != std::string::npos)
        {
            // Everything after the marker on that line is the number.
            timesWritten = std::stoi(content.substr(pos + marker.length()));
            SDL_Log("Existing file found -- counter was: %d", timesWritten);
        }
    }
    else
    {
        SDL_Log("No file found -- this must be the first run.");
    }

    timesWritten++;   // increment before writing

    // -------------------------------------------------------------------------
    // PART 4: BUILD THE NEW FILE CONTENTS
    //
    // We write the system info fresh every run so the file always reflects
    // the current machine. The counter line goes at the bottom with a
    // consistent prefix so the next run can find it with string::find().
    // -------------------------------------------------------------------------
    std::ostringstream oss;
    oss << "=== SDL3 System Info ===\n"
        << "Platform  : " << SDL_GetPlatform()          << "\n"
        << "CPU cores : " << SDL_GetNumLogicalCPUCores()           << "\n"
        << "RAM       : " << SDL_GetSystemRAM()           << " MB\n"
        << "Cache line: " << SDL_GetCPUCacheLineSize()    << " bytes\n"
        << "\n"
        << "Times written = " << timesWritten             << "\n";

    std::string output = oss.str();

    // -------------------------------------------------------------------------
    // PART 5: SAVE THE FILE WITH SDL_SaveFile
    //
    // SDL_SaveFile(path, data, size) writes a buffer to disk in one call.
    // It creates the file if it does not exist and overwrites it if it does.
    //
    // We pass output.size() (not +1) because we do not want to store the
    // null terminator on disk -- SDL_LoadFile will add one automatically
    // when we read the file back next time.
    //
    // Return value: true = success, false = failure (check SDL_GetError).
    // -------------------------------------------------------------------------
    if (SDL_SaveFile(filePath.c_str(), output.c_str(), output.size()))
    {
        SDL_Log("File saved successfully.");
        SDL_Log("Times written = %d", timesWritten);
        SDL_Log("Run the app again to increment the counter.");
    }
    else
    {
        SDL_Log("ERROR: Save failed: %s", SDL_GetError());
        return 1;
    }

    return 0;
}

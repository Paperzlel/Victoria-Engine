#pragma once

#include "core/typedefs.h"
#include "core/error/error_macros.h"


class Main {
public:

    /**
     * @brief Sets as to whether we should quit the engine.
     * @param p_value Whether to quit or not
     */
    static void set_should_quit(bool p_value);
    
    /**
     * @brief Sets up the engine for use. Initializes the OS, creates the renderer and parses command-line arguments
     * @param argc The number of command line arguments
     * @param argv Array of argument strings
     * @return `OK` on success, and an error on failure.
     */
    static Error setup(int argc, char *argv[]);
    /**
     * @brief Starts the engine for use. Creats importers and rendering devices, amongst other things.
     * @return `OK` on success, and an error on failure.
     */
    static Error start();
    /**
     * @brief An iteration of the engine's main loop. Renders any items onto the screen and processes other items as well.
     * @return `true` if the engine can run for another frame, and `false` if it has called to quit.
     */
    static bool iteration();
    /**
     * @brief Finalizes the engine, as it shuts down all subsystems and other processes.
     */
    static void finalize();
};
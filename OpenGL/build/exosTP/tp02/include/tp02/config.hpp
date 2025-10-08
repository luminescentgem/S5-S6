#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <cstddef>
#include <string>

const char* _resources_directory = "/amuhome/b23005473/Cours/S5_S6/OpenGL/src/exosTP/tp02/resources/";	///< repertoire contenant les ressources.

std::string full_path(std::string uri) {
    return std::string(_resources_directory) + uri;
}

#endif

#include "util.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int util::read_shader_file(const char *name, std::string& shader_code)
{
    // TODO: Fix. This breaks if the program is not executed within root directory.
    fs::path current_path = fs::current_path();
    const fs::path shader_path = current_path / "assets" / "shaders" / name;

    std::ifstream shader_file;
    shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        shader_file.open(shader_path);
        std::stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        shader_code = shader_stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return false;
    }

    return true;
}
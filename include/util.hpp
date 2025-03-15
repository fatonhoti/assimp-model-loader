#include <string>

namespace util {

    // function that takes string name of a shader and returns the file contents read into a string
    int read_shader_file(const char* name, std::string& shader_source);

}; // end namespace 'util'
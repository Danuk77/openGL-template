#include <shader/Shader.hpp>

/**
 * Constructor for a shader program.
 * @param vertex_path The path to the vertex shader source code
 * @param fragment_path The path to the fragment shader source code
 */
Shader::Shader(const char *vertex_path, const char *fragment_path)
{
    string vertex_source;
    string fragment_source;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertex_path);
        fShaderFile.open(fragment_path);

        stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertex_source = vShaderStream.str();
        fragment_source = fShaderStream.str();
    }
    catch (ifstream::failure e)
    {
        cout << "ERROR::SHADER::COULDN'T READ SHADER FILE" << endl;
    }

    const char *vertex_source_char = vertex_source.c_str();
    const char *fragement_source_char = fragment_source.c_str();

    // Compile the shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // The vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source_char, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    };

    // The fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragement_source_char, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    };

    // Create the shader program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    // print linking errors if any
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

/**
 * Set this shader as the currently used shader
 */
void Shader::use()
{
    glUseProgram(id);
}

/**
 * Utility function used for setting a boolean uniform,
 * @param uniform_name The uniform to set.
 * @param value The value to set the uniform to.
 */
void Shader::set_bool(const std::string &uniform_name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, uniform_name.c_str()), (int)value);
}

/**
 * Utility function used for setting an int uniform,
 * @param uniform_name The uniform to set.
 * @param value The value to set the uniform to.
 */
void Shader::set_int(const std::string &uniform_name, int value) const
{
    glUniform1i(glGetUniformLocation(id, uniform_name.c_str()), value);
}

/**
 * Utility function used for setting a float uniform,
 * @param uniform_name The uniform to set.
 * @param value The value to set the uniform to.
 */
void Shader::set_float(const std::string &uniform_name, float value) const
{
    glUniform1f(glGetUniformLocation(id, uniform_name.c_str()), value);
}

/**
 * Utility function used for setting a 4 dimensional matrix uniform
 * @param uniform_name The uniform to set.
 * @param value The value to set the uniform to.
 */
void Shader::set_mat4(const std::string &uniform_name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, uniform_name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

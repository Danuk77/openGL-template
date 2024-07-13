#define GLFW_USE_WIN32
#include <glad/glad.h>
// Make sure to include glad before any other libraries that depend on opengl
#include <GLFW/glfw3.h>
#include <shader/Shader.hpp>
#include <stb/stb_image.h>
#include <textures/Textures.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
using namespace std;
using namespace glm;

// Variables relating to the camera
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

// Time variables
float last_frame_time = 0.0f;
float delta_time = 0.0f;

// Variables used for tracking the mouse position (in the callback function mouse_callback)
float lastX = 400;
float lastY = 300;
float pitch_angle = 0;
float yaw_angle = -90;
bool firstMouse = true;
const float sensitivity = 0.1f; // Mouse sensitivity

vec3 main_cube_pos = vec3(0.0f, 1.0f, -1.0f);
vec3 secondary_cube_pos = glm::vec3(2.4f, -0.4f, -6.5f);
vec3 light_direction = vec3(-0.2f, -1.0f, -0.3f);
vec3 light_position = vec3(0.0f, 0.0f, 0.0f);

// Callback function to handle the window resizing.
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * Process input the user enters
 */
void process_input(GLFWwindow *window)
{

    float current_frame_time = glfwGetTime();
    delta_time = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;

    float camera_speed = 2.5f * delta_time;

    vec3 move_step_forward = camera_speed * cameraFront;
    vec3 move_step_sideways = normalize(cross(cameraFront, cameraUp)) * camera_speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += move_step_forward;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= move_step_forward;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= move_step_sideways;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += move_step_sideways;
    }
}

// Initialises GLFW and GLAD.
GLFWwindow *initialise()
{
    if (!glfwInit())
    {
        cout << "Failed to initialise GLFW" << endl;
        return nullptr;
    }

    // Specify the OpenGL versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Engine", nullptr, nullptr);

    if (window == nullptr)
    {
        cout << "Couldn't create a GLFW window" << endl;
        glfwTerminate();
        return nullptr;
    }

    // Make this windows the current context
    glfwMakeContextCurrent(window);

    // Initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return nullptr;
    }

    // Set the viewport of the opengl context (specifies how much of the window we will take)
    glViewport(0, 0, 800, 600);

    // Register the callback function 'freambuffer_size_callback' to handle window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

// Creates a simple triangle to render.
void create_triangle()
{
    // The vertices of the triangle we want to render
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // Copy the vertices from 'vertices' to the vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy the indices from 'indices' to the index buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void create_cube()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void link_vertex_attributes()
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

unsigned int create_VAO()
{
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    return VAO;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    // Initialise the values in the first time the callback is called
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw_angle += xoffset;
    pitch_angle += yoffset;

    if (pitch_angle > 89.0f)
        pitch_angle = 89.0f;
    if (pitch_angle < -89.0f)
        pitch_angle = -89.0f;

    // Calculate the direction to point at from the view matrix
    vec3 direction;
    direction.x = cos(radians(yaw_angle)) * cos(radians(pitch_angle));
    direction.y = sin(radians(pitch_angle));
    direction.z = sin(radians(yaw_angle)) * cos(radians(pitch_angle));
    cameraFront = normalize(direction);
}

void draw_cube(Shader shader, unsigned int VAO, vec3 pos, float angle)
{
    glBindVertexArray(VAO);
    mat4 model(1.0f);
    mat4 projection;
    mat4 view;

    shader.use();

    projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.01f, 100.0f);
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    model = translate(model, pos);
    model = rotate(model, radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    shader.set_mat4("projection", projection);
    shader.set_mat4("model", model);
    shader.set_mat4("view", view);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// The main render loop.
void render_loop(GLFWwindow *&window, Shader main_shader, unsigned int main_VAO, Texture diffuse_map, Texture specular_map)
{
    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        // Process the user's input
        process_input(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_cube(main_shader, main_VAO, main_cube_pos, 0.0f);
        draw_cube(main_shader, main_VAO, secondary_cube_pos, 55.6f);
        main_shader.set_vec3("viewPos", cameraPos);

        glActiveTexture(GL_TEXTURE0);
        diffuse_map.bind(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        specular_map.bind(GL_TEXTURE_2D);

        main_shader.set_vec3("material.specular", vec3(0.5f, 0.5f, 0.5f));
        main_shader.set_float("material.shininess", 32.0f);

        // Directional light shader inputs
        main_shader.set_vec3("directional_light.direction", cameraFront);
        main_shader.set_vec3("directional_light.ambient", vec3(0.2f, 0.2f, 0.2f));
        main_shader.set_vec3("directional_light.diffuse", vec3(0.5f, 0.5f, 0.5f));
        main_shader.set_vec3("directional_light.specular", vec3(1.0f, 1.0f, 1.0f));

        // Point light shader inputs
        main_shader.set_float("point_light.constant", 1.0f);        
        main_shader.set_float("point_light.linear", 0.09f); 
        main_shader.set_float("point_light.quadratic", 0.032f); 
        main_shader.set_vec3("point_light.position", cameraPos);
        main_shader.set_vec3("point_light", vec3(0.2f, 0.2f, 0.2f));        
        main_shader.set_vec3("point_light", vec3(0.5f, 0.5f, 0.5f));
        main_shader.set_vec3("point_light", vec3(1.0f, 1.0f, 1.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main()
{
    // Initialise GLFW and GLAD
    GLFWwindow *window = initialise();

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    unsigned int VAO = create_VAO();

    // Initialise the vertex buffer object for storing our triangle within the GPU
    unsigned int VBO; //, EBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // For the main cube
    create_cube();
    link_vertex_attributes();

    // Load our shader programs
    Shader new_shader("X:\\Side projects\\OpenGL learning\\openGL-template\\shaders\\vertex.glsl",
                      "X:\\Side projects\\OpenGL learning\\openGL-template\\shaders\\fragment.glsl");

    Texture diffuse_map("X:\\Side projects\\OpenGL learning\\openGL-template\\textures\\container2.png",
                        GL_RGB,
                        GL_RGBA,
                        false);

    Texture specular_map("X:\\Side projects\\OpenGL learning\\openGL-template\\textures\\container2_specular.png",
                         GL_RGB,
                         GL_RGBA,
                         false);

    new_shader.use();
    new_shader.set_int("material.diffuse", 0);
    new_shader.set_int("material.specular", 1);

    // Call the render loop
    render_loop(window, new_shader, VAO, diffuse_map, specular_map);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Free up the resources once instructed to close
    glfwTerminate();
    return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <stdio.h>

#include <shader.hpp>
#include <Game.hpp>
#include <Entity.hpp>

int main(){

    glm::vec3 position = glm::vec3(0, 0, 0);

    if(!glfwInit()){
        printf("Initialization failed");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // makes macOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates a window and an OpenGL context
    window = glfwCreateWindow(800, 600, "GAME", NULL, NULL);
    if(window == NULL){
        fprintf(stderr, "Failed to open GLFW Window.");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //glewExperimental = true; // needed for core profile
    GLenum err = glewInit();
    
    if(err != GLEW_OK){
        fprintf(stderr, "Failed to initialize GLEW: %s", glewGetErrorString(err));
        getchar();
        glfwTerminate();
        return -1;
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glClearColor(0.0f, 0.15f, 0.3f, 0);

    GLuint ProgramID = LoadShaders("./shaders/vertex1.glsl", "./shaders/frag1.glsl");

    glm::mat4 PerspectiveProjection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 OrthoProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);
    
    glm::mat4 View = glm::lookAt(
        glm::vec3(0.0f, 1.0f, 4.0f),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    glm::mat4 Model = glm::mat4(1.0f);

    GLint MVP_Location = glGetUniformLocation(ProgramID, "MVP");

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat test_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Game game;
    game.createEntity("./resources/cube.obj", ProgramID);
    game.createEntity("./resources/person.obj", ProgramID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[0].out_vertices.size()*sizeof(glm::vec3), &game.entities[0].out_vertices[0], GL_STATIC_DRAW);

    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, game.entities[1].out_vertices.size()*sizeof(glm::vec3), &game.entities[1].out_vertices[0], GL_STATIC_DRAW);

    float lastTime = glfwGetTime();
    float currentTime = lastTime;
    do{
        currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Swap buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ProgramID);
        
        game.gameLoop(window, deltaTime);

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while( glfwGetKey(window, game.input.EXIT_GAME) != GLFW_PRESS && glfwWindowShouldClose(window) == false );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(ProgramID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
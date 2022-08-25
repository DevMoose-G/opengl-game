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

    GLint MVP_Location = glGetUniformLocation(ProgramID, "MVP");

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // ordering of displaying models (closer to camera will load in front)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //Transparency & blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint personTexture = loadDDS("./resources/texture.dds");
    GLuint crackedTexture = loadDDS("./resources/cracked-ground.dds");

    Game game;
    Entity* person = game.createEntity("Player", "./resources/person.obj", glm::vec3(2, 3, 5), ProgramID, personTexture);
    Entity* ring = game.createEntity("Ring", "./resources/ring.obj", glm::vec3(2, 3, 5), ProgramID, personTexture);
    Entity* ground = game.createEntity("Ground", "./resources/floor.obj", glm::vec3(0, -1.0f, 0), ProgramID, crackedTexture);

    person->scale(0.125f);
    game.setPlayer(person);
    game.addGround(ground);
    ring->setTransparency(0.5f);

    // Temporary: find way to initialize vertexbuffer in the entity initialization
    glGenBuffers(1, &game.entities[0].vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, game.entities[0].vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[0].out_vertices.size()*sizeof(glm::vec3), &game.entities[0].out_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &game.entities[0].normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, game.entities[0].normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[0].out_normals.size()*sizeof(glm::vec3), &game.entities[0].out_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &game.entities[1].vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, game.entities[1].vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[1].out_vertices.size()*sizeof(glm::vec3), &game.entities[1].out_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &game.entities[1].normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, game.entities[1].normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[1].out_normals.size()*sizeof(glm::vec3), &game.entities[1].out_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &game.entities[2].vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, game.entities[2].vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[2].out_vertices.size()*sizeof(glm::vec3), &game.entities[2].out_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &game.entities[2].normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, game.entities[2].normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, game.entities[2].out_normals.size()*sizeof(glm::vec3), &game.entities[2].out_normals[0], GL_STATIC_DRAW);

    float lastTime = glfwGetTime();
    float currentTime;
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
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(ProgramID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
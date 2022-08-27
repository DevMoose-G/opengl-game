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

// generates buffers for every entity and all colliderDisplays in game
void GenBuffers(Game &game){
    for(int i = 0; i < game.EntityCount; i++){
        glGenBuffers(1, &game.entities[i].vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, game.entities[i].vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, game.entities[i].out_vertices.size()*sizeof(glm::vec3), &game.entities[i].out_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &game.entities[i].normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, game.entities[i].normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, game.entities[i].out_normals.size()*sizeof(glm::vec3), &game.entities[i].out_normals[0], GL_STATIC_DRAW);
    }

    for(std::map<Entity*, AABBDisplay>::iterator pair = game.colliders.begin(); pair != game.colliders.end(); pair++){
        printf("Making the colliderDisplay for %s\n", pair->first->_name.c_str());
        glGenBuffers(1, &pair->second.vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, pair->second.vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, pair->second.out_vertices.size()*sizeof(glm::vec3), &pair->second.out_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &pair->second.normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, pair->second.normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, pair->second.out_normals.size()*sizeof(glm::vec3), &pair->second.out_normals[0], GL_STATIC_DRAW);
    
        if(std::strcmp(pair->first->_name.c_str(), "Player") == 0){
            //printf("\t{%f, %f, %f}", pair->second.out_vertices.size());
            //printf("Number of vertices: %d\n", pair->second.out_vertices.size());
        }
    }
}

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
    GLuint ColliderProgramID = LoadShaders("./shaders/basicVertex.glsl", "./shaders/basicFrag.glsl");

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
    Entity* player = game.createEntity("Player", "./resources/person.obj", glm::vec3(2, 10, 2), ProgramID, personTexture);
    //Entity* trainer1 = game.createEntity("Trainer1", "./resources/person.obj", glm::vec3(2, 10, 2), ProgramID, personTexture);
    Entity* ring = game.createEntity("Ring", "./resources/ring.obj", glm::vec3(1, 2, 0), ProgramID, personTexture);
    Entity* creature1 = game.createEntity("Creature1", "./resources/creatureOutline.obj", glm::vec3(-5, 6, -5), ProgramID, personTexture);
    Entity* creature2 = game.createEntity("Creature2", "./resources/creatureOutline.obj", glm::vec3(0, 6, -5), ProgramID, personTexture);
    Entity* creature3 = game.createEntity("Creature2", "./resources/creatureOutline.obj", glm::vec3(0, 6, 5), ProgramID, personTexture);
    Entity* ground = game.createEntity("Ground", "./resources/floor.obj", glm::vec3(0, -1.0f, 0), ProgramID, crackedTexture);

    player->scale(0.125f);
    ring->scale(0.4f);
    creature1->scale(0.2f);
    creature2->scale(0.2f);
    creature3->scale(0.2f);
    game.setPlayer(player);
    game.addGround(ground);
    ring->setTransparency(0.5f);

    game.setCreatureOwner(player, creature1);
    game.setCreatureOwner(player, creature2);
    game.setCreatureOwner(player, creature3);
    //game.createColliderDisplays(ColliderProgramID);

    // Be the last thing game does before the loop
    GenBuffers(game);

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
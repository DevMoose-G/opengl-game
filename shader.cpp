#include <GL/glew.h>

#include <string>
#include <fstream>

#include <iostream>
#include <vector>

#include <shader.hpp>

GLuint LoadShaders(const char* vertex_file_path, const char* frag_file_path){
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string VertexShaderCode;
    std::ifstream vertexFile(vertex_file_path);

    char c;
    while(vertexFile.get(c)){
        VertexShaderCode += c;
    }
    vertexFile.close();

    std::string FragShaderCode;
    std::ifstream fragFile(frag_file_path);

    while(fragFile.get(c)){
        FragShaderCode += c;
    }
    fragFile.close();

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    const char* VertexShaderPointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexShaderPointer, NULL);
    glCompileShader(VertexShaderID);

    // Check if Vertex Shader is correct
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0){
        fprintf(stderr, "Vertex Shader was not compiled correctly.\n");
        std::string ErrorMessage;
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &ErrorMessage[0]);
        fprintf(stderr, "%s\n", ErrorMessage.c_str());
    }

    // Compile Fragment Shader
    const char* FragShaderPointer = FragShaderCode.c_str();
    glShaderSource(FragShaderID, 1, &FragShaderPointer, NULL);
    glCompileShader(FragShaderID);

    // Check if Fragment Shader is correct
    glGetShaderiv(FragShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0){
        fprintf(stderr, "Fragment Shader was not compiled correctly.\n");
        std::string ErrorMessage;
        glGetShaderInfoLog(FragShaderID, InfoLogLength, NULL, &ErrorMessage[0]);
        fprintf(stderr, "%s\n", ErrorMessage.c_str());
    }

    // Linking the program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragShaderID);
    glLinkProgram(ProgramID);

    // Check if Program is valid
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if( InfoLogLength > 0 ){
        fprintf(stderr, "Shader Program could not be linked correctly.\n");
        std::string ErrorMessage;
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ErrorMessage[0]);
        fprintf(stderr, "%s\n", ErrorMessage.c_str());
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragShaderID);

    return ProgramID;
}
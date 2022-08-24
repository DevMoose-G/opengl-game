#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <Game.hpp>

class Inputs{
    public:
        int EXIT_GAME = GLFW_KEY_ESCAPE;
        int TOGGLE_ORTHO_PERSPECT = GLFW_KEY_TAB;
        int CAM_RIGHT = GLFW_KEY_D;
        int CAM_LEFT = GLFW_KEY_A;
        int CAM_FORWARD = GLFW_KEY_W;
        int CAM_BACKWARD = GLFW_KEY_S;
};

#endif
#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <Game.hpp>

class Inputs{
    public:
        int EXIT_GAME = GLFW_KEY_ESCAPE;
        int TOGGLE_ORTHO_PERSPECT = GLFW_KEY_TAB;
        int MOVE_RIGHT = GLFW_KEY_D;
        int MOVE_LEFT = GLFW_KEY_A;
        int MOVE_FORWARD = GLFW_KEY_W;
        int MOVE_BACKWARD = GLFW_KEY_S;
        int TOGGLE_CREATURE = GLFW_KEY_TAB;

        bool TOGGLE_CREATURE_PRESSED = false;
};

#endif
#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <Game.hpp>
#include <GLFW/glfw3.h>

class Inputs{
    public:
        int EXIT_GAME = GLFW_KEY_ESCAPE;
        int TOGGLE_ORTHO_PERSPECT = GLFW_KEY_TAB;
        int MOVE_RIGHT = GLFW_KEY_D;
        int MOVE_LEFT = GLFW_KEY_A;
        int MOVE_FORWARD = GLFW_KEY_W;
        int MOVE_BACKWARD = GLFW_KEY_S;
        int TOGGLE_CREATURE = GLFW_KEY_TAB;
		int CREATURE_ATTACK = GLFW_KEY_LEFT_SHIFT;

        bool TOGGLE_CREATURE_PRESSED = false;
};

#endif
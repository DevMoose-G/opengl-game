#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <Util.hpp>

// Axis-Aligned Bounding Box
class AABB{
    public:
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;

        AABB();
        AABB(float lowX, float lowY, float lowZ, float highX, float highY, float highZ);

        AABB copy();
};

class Entity{
    public:
        std::string type;

        glm::mat4 MVP = glm::mat4(1.0f);
        glm::mat4 Model = glm::mat4(1.0f);
        int programID = 0;

        std::vector<glm::vec3> out_vertices;
        std::vector<glm::vec2> out_uvs;
        std::vector<glm::vec3> out_normals;

        GLuint vertexbuffer;
        GLuint uvbuffer;
        GLuint normalbuffer;

        GLuint textureID;

        std::string _name;

        glm::vec3 position;
        glm::vec3 _scale;
        // rotation only around the y-axis
        float rotation;

        glm::vec3 motion;

        float transparency = 1.0f;

        bool isGrounded;
        float weight = 1.0f;

        AABB collider;
		bool collisions = true;

        Entity();
        Entity(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint texture);
        ~Entity();

        void AABBFromVertices();
        void initialize();
        void translate(float x, float y, float z);
        void translate(glm::vec3 motion);
        void scale(float s);
        void scale(float x, float y, float z);
        void setTransparency(float alpha);
        void update();
        void draw();
};

// shows the actual collision box
class AABBDisplay: public Entity {
    public:
        Entity* entity;

        AABBDisplay(Entity* entity, int programID, GLuint colliderTexture);

        void gameLoop();
};

AABB getGlobalAABB(Entity* entity);
AABB getScaledAABB(Entity *entity);

class Telegraph: public Entity{
	public:

		Telegraph();
		Telegraph(const char* filepath, glm::vec3 position, int program);
};

class Move{
	public:
		glm::vec3 finalPos;
		float movementSpeed;
		float damage;
		std::vector<Entity*> alreadyHit; // list of those already hit by current move

		Move();
		Move(glm::vec3 finalPos, float speed, float damage);

		// resets variables alreadyHit and finalPos
		void reset();
};

class Creature: public Entity{
    public:
        float health = 100;
		float speed = 1.0f;

		// as soon as this key is let go, creature is in attackMode
		bool attackPressed = false;
		// for charge attacks, if range > 0, creature charges in your chosen direction
		float attackRange = -1;

		Telegraph currentTelegraph;
		
		Move move1 = Move(glm::vec3(0.0f), 17.5f, 50.0f);
		Move* activeMove = nullptr;

        Creature(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint texture);
		
		void gameLoop(float deltaTime);
};

class Trainer: public Entity{
	public:
		std::vector<Creature*> creatures;
		
        Trainer(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint texture);

};

#endif
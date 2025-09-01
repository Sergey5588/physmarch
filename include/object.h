#include <glm/glm.hpp>
#ifndef OBJECT_H
#define OBJECT_H

enum ObjectType {
	T_SPHERE,
	T_BOX,
	T_PRISM,
	T_TORUS,
	T_PLANE,
	T_BULB
};

enum OperationType {
	O_BASE
};



struct Object {
    int type;
    int operation;
    glm::vec3 pos;
    glm::vec4 args;
    int material_id;

};
const int object_size = sizeof(int) + sizeof(int) + sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(int);
// const int object_size = 48;

#endif
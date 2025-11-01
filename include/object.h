#include <glm/glm.hpp>
#include <cstddef>

#ifndef OBJECT_H
#define OBJECT_H

enum ObjectType {
	T_SPHERE,
	T_BOX,
	T_PRISM,
	T_TORUS,
	T_PLANE,
	T_BULB,
	T__LENGTH
};



enum OperationType {
	O_BASE
};

//Object{T_BOX, O_BASE, glm::vec3(0,3,0), glm::vec4(0), 0},
// THANK YOU DEEPSEEK!!!!!!!!!!!!!!!!!
struct alignas(16) Object {
	alignas(4) int type;
    alignas(4) int operation;
    alignas(16) glm::vec3 pos;  // vec3 equivalent
    alignas(16) glm::vec4 args; // vec4 equivalent  
    alignas(4) int material_id;
    alignas(4) int _padding;   // Explicit padding
	Object(int type, int operation, glm::vec3 pos, glm::vec4 args, int material_id) : type(type), operation(operation), pos(pos), args(args), material_id(material_id) {}
	Object() {};
};
// struct Material {
//     float roughness;
//     vec4 color;
//     bool reflective;
// };

struct alignas(16) Material {
	alignas(16) glm::vec4 color;
	alignas(4) float roughness;
    //alignas(1) reflective;
    alignas(4) int _padding;   // Explicit padding
	alignas(4) int _padding2;   // Explicit padding
	alignas(4) int _padding3;
	Material(glm::vec4 color, float roughness) : color(color), roughness(roughness) {}
	Material() {};
};

static_assert(offsetof(Object, pos) == 16, "pos offset incorrect");
static_assert(offsetof(Object, args) == 32, "args offset incorrect");
static_assert(offsetof(Object, material_id) == 48, "material_id offset incorrect");
static_assert(sizeof(Object) == 64, "Object size incorrect");

// const int object_size = sizeof(int) + sizeof(int) + sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(int);
const int object_size = 64;
// const int object_size = 48;
const int material_size = 32;
#endif
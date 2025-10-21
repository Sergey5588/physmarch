#ifndef UBO_CLASS_H
#define UBO_CLASS_H

#ifndef __EMSCRIPTEN__
#include"glad.h"
#else
#include <GLES3/gl3.h>
#endif
#include"object.h"
#include<vector>
#include <string>
//with max-ts help
class UBO {
    public:
        GLuint ID;

        UBO(int struct_size, int buffer_size);
        void Bind();
        void UnBind();
        void BindBase(GLuint shaderProgram, GLuint base, std::string buffer_name);
        void WriteData(std::vector<Object> &objects, int struct_size);
        void WriteData(std::vector<Material> &materials, int struct_size);
        void Delete();
};

#endif
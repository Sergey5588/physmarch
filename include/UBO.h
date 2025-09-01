#ifndef UBO_CLASS_H
#define UBO_CLASS_H

#include"glad.h"
#include"object.h"
#include<vector>
//with max-ts help
class UBO {
    public:
        GLuint ID;

        UBO(int struct_size, int buffer_size);
        void Bind();
        void UnBind();
        void BindBase(GLuint shaderProgram, GLuint base);
        void WriteData(std::vector<Object> &objects, int struct_size);
        void Delete();
};

#endif
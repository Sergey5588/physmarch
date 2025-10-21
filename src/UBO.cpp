#include "UBO.h"

UBO::UBO(int struct_size, int buffer_size) {
    glGenBuffers(1, &ID);
    Bind();

    glBufferData(GL_UNIFORM_BUFFER, struct_size * buffer_size, NULL, GL_DYNAMIC_DRAW);
    UnBind();
}

void UBO::Bind() {
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

void UBO::UnBind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::BindBase(GLuint shaderProgram, GLuint base, std::string buffer_name) {
    GLuint UBOBufferIdx = glGetUniformBlockIndex(shaderProgram, buffer_name.c_str());
    glUniformBlockBinding (shaderProgram, UBOBufferIdx, base);

    glBindBufferBase(GL_UNIFORM_BUFFER, base, ID);
}

void UBO::WriteData(std::vector<Object> &objects, int struct_size) { 
    GLsizeiptr size = struct_size;
    GLintptr offset = 0;
    for (const auto& object : objects) {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, (void*)&object.type);
        offset += struct_size;
    }
}

void UBO::WriteData(std::vector<Material> &materials, int struct_size) { 
    GLsizeiptr size = struct_size;
    GLintptr offset = 0;
    for (const auto& object : materials) {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, (void*)&object.color.r);
        offset += struct_size;
    }
}

void UBO::Delete() {
    glDeleteBuffers(1, &ID);
}

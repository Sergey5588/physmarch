#include "UBO.h"

UBO::UBO(int struct_size, int buffer_size) {
    glGenBuffers(1, &ID);

    glBufferData(GL_UNIFORM_BUFFER, struct_size * buffer_size, NULL, GL_DYNAMIC_DRAW);
}

void UBO::Bind() {
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

void UBO::UnBind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::BindBase(GLuint shaderProgram, GLuint base) {
    GLuint UBOBufferIdx = glGetUniformBlockIndex(shaderProgram, "UBO");
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

void UBO::Delete() {
    glDeleteBuffers(1, &ID);
}

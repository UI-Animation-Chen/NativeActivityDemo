//
// Created by czf on 2019-10-20.
//

#include "Triangles.h"

Triangles::Triangles(): Shape() {
    glGenVertexArrays(2, vaos);
    glGenBuffers(2, buffers);

    // vao会记录所有的状态数据，包括attrib的开启与否。
    // 绑定成功后，之前的vao绑定会解除，后续的所有数据操作都会记录到这个vao上。
    // 后续在切换数据时，只要切换绑定vao就行。
    glBindVertexArray(vaos[0]);
    bind_buf0();
    // glVertexAttribPointer操作的是[当前绑定]到GL_ARRAY_BUFFER上的VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(vaos[1]);
    bind_buf1();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

Triangles::~Triangles() {
    glDeleteBuffers(2, buffers);
    glDeleteVertexArrays(2, vaos);
}

void Triangles::bind_buf0() {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]); // 绑定成功后，会自动接触之前的绑定。
    GLfloat triangles[] = {
             0.0f,   0.75f, -0.2f,
            -0.25f, -0.0f,   0.2f,
             0.0f,  -0.75f,  0.2f,
             0.25f,  0.0f,   0.2f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
}

void Triangles::bind_buf1() {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    GLfloat triangles[] = {
             0.0f,  0.5f, -0.2f,
            -0.5f,  0.0f,  0.2f,
             0.0f, -0.5f,  0.2f,
             0.5f,  0.0f,  0.2f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
}

int state = 0;

void Triangles::draw() {
    if (state == 0) {
        glBindVertexArray(vaos[1]);
        state = 1;
    } else {
        glBindVertexArray(vaos[0]);
        state = 1;
    }
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

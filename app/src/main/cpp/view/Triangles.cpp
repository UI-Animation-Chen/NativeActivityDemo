//
// Created by czf on 2019-10-20.
//

#include "Triangles.h"
#include "../texture/TextureUtils.h"

// 子类必须调用父类构造函数，不写的话系统会默认调用，如果没有默认就会报错。
// 析构函数一定会调用父类的，编译器强制。
Triangles::Triangles(): Shape() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(2, buffers);

    // vao会记录所有的状态数据，包括attrib的开启与否。
    // 绑定成功后，之前的vao绑定会解除，后续的所有数据操作都会记录到这个vao上。
    // 后续在切换数据时，只要切换绑定vao就行。
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]); // 绑定成功后，会自动解除之前的绑定。
    GLfloat triangles[] = {
             0.0f,   0.75f, -0.2f,
            -0.25f,  0.0f,   0.2f,
             0.0f,  -0.75f,  0.2f,
             0.25f,  0.0f,   0.2f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
    // glVertexAttribPointer操作的是[当前绑定]到GL_ARRAY_BUFFER上的VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    GLfloat texCoords[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

Triangles::~Triangles() {
    glDeleteBuffers(2, buffers);
    glDeleteVertexArrays(1, &vao);
}

void Triangles::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

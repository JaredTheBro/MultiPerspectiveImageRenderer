#include <GL/glew.h>
#include <GL/freeglut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw vertex array
    glBindVertexArray();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Hello FreeGLUT");
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        exit(1);
    }
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
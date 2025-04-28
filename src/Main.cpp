#include <GL/freeglut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Hello FreeGLUT");
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
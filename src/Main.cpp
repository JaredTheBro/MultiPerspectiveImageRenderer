#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "Model.h"

Model* obj;
std::string filename;

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 16.0f / 10, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 3,   // eye
        0, 0, 0,   // center
        0, 1, 0);  // up

    // Draw vertex array
    obj->drawVertexArray();

    glPopMatrix();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Hello FreeGLUT");
    
    printf("test1\n");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        exit(1);
    }

    printf("test2\n");

    glEnable(GL_DEPTH_TEST);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    printf("test3\n");

    // Load model
    filename = "models/test1.obj";
    obj = new Model();
    obj->loadFileObj(filename);

    printf("test4\n");

    glutDisplayFunc(display);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutMainLoop();

    delete obj;
    return 0;
}
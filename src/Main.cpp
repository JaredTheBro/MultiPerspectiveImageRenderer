#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "Model.h"
#include "Camera.h"

// Important variables
Model* obj;
Camera* cam;
std::string filename;

// GLUT callback functions
static void display();
static void keyPressed(unsigned char key, int mousex, int mousey);
static void specialPressed(int key, int mousex, int mousey);

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 16.0f / 9, 0.1, 100.0);

    glm::vec3 pos = cam->getPos();
    glm::vec3 lookPoint = cam->getLookPoint();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(pos.x, pos.y, pos.z,   // eye
        lookPoint.x, lookPoint.y, lookPoint.z,   // center
        0, 1, 0);  // up

    // Draw vertex array
    obj->drawVertexArray();

    // glPopMatrix();
    glutSwapBuffers();
}

// Handle normal keyboard input
static void keyPressed(unsigned char key, int mousex, int mousey)
{
    switch (key)
    {
    case 'w':
        cam->moveForward();
        glutPostRedisplay(); // Redraw scene after camera moves
        break;
    case 'a':
        cam->moveLeft();
        glutPostRedisplay();
        break;
    case 's':
        cam->moveBackward();
        glutPostRedisplay();
        break;
    case 'd':
        cam->moveRight();
        glutPostRedisplay();
        break;
    case 'q':
        cam->moveDown();
        glutPostRedisplay();
        break;
    case 'e':
        cam->moveUp();
        glutPostRedisplay();
        break;
    }
}

static void specialPressed(int key, int mousex, int mousey)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        cam->lookUp();
        glutPostRedisplay(); // Redraw scene after camera moves
        break;
    case GLUT_KEY_DOWN:
        cam->lookDown();
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        cam->lookLeft();
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        cam->lookRight();
        glutPostRedisplay();
        break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Multi-Perspective Renderer");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    // Load model
    filename = "models/cube.obj";
    obj = new Model();
    obj->loadFileObj(filename);

    // Make camera
    cam = new Camera();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(specialPressed);

    // Wireframe mode, uncomment for testing
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Start main loop
    glutMainLoop();

    delete obj;
    delete cam;
    return 0;
}
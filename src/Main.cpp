#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

#include <direct.h>

#include "Model.h"
#include "Camera.h"
#include "PathTracer.h"

// Public-domain image write header
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Objects
Model* obj;
Camera* cam;
PathTracer* pather;

// File data
std::string filename;
unsigned int imageNum;

// OpenGL data
int windowWidth;
int windowHeight;

// GLUT callback functions
static void display();
static void keyPressed(unsigned char key, int mousex, int mousey);
static void specialPressed(int key, int mousex, int mousey);

static void display() {
    // Set up solid color
    glColor3f(0.6f, 0.6f, 0.6f);
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

    // Display frame on screen
    glutSwapBuffers();

    // Wait for finish
    glFinish();
}

// Render a frame for image processing only. Don't display on screen.
static void internalDisplay()
{
    // Set up solid color
    glColor3f(0.6f, 0.6f, 0.6f);
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

    // Wait until rendering is complete
    glFinish();
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
    case ' ':
        pather->addPoint(cam, obj);
        break;
    case 'g':
        std::vector<unsigned char> pixelData = 
            pather->renderFromPath(cam, windowWidth, windowHeight, /*internalDisplay*/ display); // Render image

        if (pixelData.empty()) {
            break;
        }

        std::string outFile = "output/multiRender" + std::to_string(imageNum) + ".png";

        char filebuf[FILENAME_MAX + 1];
        _getcwd(filebuf, FILENAME_MAX);
        printf("%s\n", filebuf);

        int finalWidth = pixelData.size() / windowHeight / 3;
        stbi_write_png(outFile.c_str(), finalWidth, windowHeight, 3, pixelData.data(), finalWidth * 3);
        printf("* * *\nImage generated! Saved to \"%s\"\n* * *\n", outFile.c_str());
        imageNum++;
        break;
    }
}

static void specialPressed(int key, int mousex, int mousey)
{
    switch (key)
    {
    /*
    case GLUT_KEY_UP:
        cam->lookUp();
        glutPostRedisplay(); // Redraw scene after camera moves
        break;
    case GLUT_KEY_DOWN:
        cam->lookDown();
        glutPostRedisplay();
        break;
    */
    case GLUT_KEY_LEFT:
        cam->lookLeft();
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        cam->lookRight();
        glutPostRedisplay();
        break;
    case GLUT_KEY_SHIFT_L:
        pather->removePoint(obj);
        break;
    }
}

int main(int argc, char** argv) {
    windowWidth = 1280;
    windowHeight = 720;
    imageNum = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Multi-Perspective Renderer");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    // Load model
    filename = "models/dlamp.obj";
    obj = new Model();
    obj->loadFileObj(filename);

    // Make camera
    cam = new Camera();

    // Make path tracer
    pather = new PathTracer();

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(specialPressed);

    // Wireframe mode, uncomment for testing
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Start main loop
    glutMainLoop();

    delete obj;
    delete cam;
    return 0;
}
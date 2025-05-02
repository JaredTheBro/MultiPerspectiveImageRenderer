#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>

class Model
{
public:

	Model();
	~Model();

	void loadFileObj(const std::string& filename);
	void drawVertexArray();
	
	// For rendering purposes only. Should be called only by
	// the PathTracer class.
	void addPathPoint(std::vector<float> verts, std::vector<unsigned int> tris);
	void removePathPoint(size_t vertsSize, size_t trisSize); // Can only remove the most recent point, for now

protected:

	// Vertices and triangles of the loaded model
	std::vector<float> vertices; // triples (x, y, z)
	std::vector<unsigned int> triangles; // triples

	// Vertices and triangles of points plotted by user
	std::vector<float> pointVerts;
	std::vector<unsigned int> pointTris;

	GLuint varr;
	GLuint vbuf;
	GLuint vcount;
	GLuint ebuf;

	std::vector<std::string> splitBySpaces(std::string& in);

	// Internal function to be called when OpenGL needs to update
	// its vertices and triangles to render.
	void updateOpenGLData();
};

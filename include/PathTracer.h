#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Camera.h"
#include "Model.h"

// Stores position and orientation of placed point
struct PathPoint
{
	glm::vec3 pos;
	CamOrientation rot;
};

class PathTracer
{
public:

	PathTracer();
	~PathTracer();

	// Use ONLY these functions to plot or remove points from the path.
	// The vertex array sent to OpenGL must be updated when this happens
	// to correctly render the points, and these functions handle that.
	void addPoint(Camera* cam, Model* model);
	// Right now, remove point can only remove the most recent point added.
	void removePoint(Model* model);

	// Render final multi-perspective image
	std::vector<unsigned char> renderFromPath(Camera* cam, int windowWidth, int windowHeight, void (*internalDisplay)());

protected:

	// Array of points placed by user
	std::vector<struct PathPoint> points;

	// Arrays of vertices and faces that make up a point,
	// as it should be visualized by OpenGL. The vertex coordinates
	// are relative to the point's position.
	std::vector<float> pointVerts; // Triples (x, y, z)
	std::vector<unsigned int> pointFaces; // Triples (triangles)

	// Read pixels from back buffer for multi-perspective image generation
	void appendPixels(std::vector<unsigned char>& dest, int x, int y, int stripWidth, int stripHeight);

};

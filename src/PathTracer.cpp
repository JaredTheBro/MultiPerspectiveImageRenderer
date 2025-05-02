#include "PathTracer.h"

PathTracer::PathTracer()
{
	// Describe how a point plotted on the path is rendered
	// (rhombus for now)
	pointVerts = {
		0, 0.05f, 0,
		0.05f, 0, 0,
		-0.05f, 0, 0,
		0, 0, 0.05f,
		0, 0, -0.05f,
		0, -0.05f, 0
	};
	pointFaces = { // zero-indexed
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		5, 1, 2,
		5, 2, 3,
		5, 3, 4,
		5, 4, 1
	};
}

PathTracer::~PathTracer()
{}

// Add a point to the path at the camera's current position and orientation
void PathTracer::addPoint(Camera* cam, Model* model)
{
	// Save point data to this object
	points.push_back({ cam->getPos(), cam->getRot() });

	// Create arrays of vertices and faces for this new point
	struct PathPoint point = points.back();
	std::vector<float> verts = std::vector<float>();
	for (size_t i = 0; i < pointVerts.size() - 2; i += 3) {
		verts.push_back(point.pos.x + pointVerts[i]);
		verts.push_back(point.pos.y + pointVerts[i + 1]);
		verts.push_back(point.pos.z + pointVerts[i + 2]);
	}

	// Send vertices and triangles to Model so they can be sent to
	// OpenGL for rendering
	model->addPathPoint(verts, pointFaces);
}

// Remove the most recently added point. (for now)
void PathTracer::removePoint(Model* model)
{
	if (!points.empty()) {
		points.pop_back();
		model->removePathPoint(pointVerts.size(), pointFaces.size());
	}
}

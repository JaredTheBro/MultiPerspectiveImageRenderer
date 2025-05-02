#include <fstream>
#include <sstream>

#include <iostream>

#include "Model.h"

Model::Model()
{
	vertices = std::vector<float>();
	triangles = std::vector<unsigned int>();

	pointVerts = std::vector<float>();
	pointTris = std::vector<unsigned int>();

	varr = 0;
	vbuf = 0;
	vcount = 0;
	ebuf = 0;
}

Model::~Model()
{}

void Model::loadFileObj(const std::string& filename)
{
	// Prepare file
	std::ifstream objFile(filename);
	if (!objFile) {
		// File error
		exit(1);
	}

	std::string line;
	std::getline(objFile, line);

	// Loop line by line to parse the file
	while (!(objFile.eof()))
	{
		// Split up line into its elements
		std::vector<std::string> elems = splitBySpaces(line);

		// Make sure line isn't empty
		if (elems.size() == 0) {
			std::getline(objFile, line);
			continue;
		}

		// Check what line describes
		if (elems[0].compare("v") == 0)
		{
			// Vertex
			vertices.push_back(std::stof(elems[1]));
			vertices.push_back(std::stof(elems[2]));
			vertices.push_back(std::stof(elems[3]));
		}
		else if (elems[0].compare("f") == 0)
		{
			// Face - only supports convex shapes for now
			if (elems.size() == 4)
			{
				// Triangle
				triangles.push_back((unsigned int) std::stoi(elems[1].substr(0, elems[1].find('/'))) - 1);
				triangles.push_back((unsigned int) std::stoi(elems[2].substr(0, elems[2].find('/'))) - 1);
				triangles.push_back((unsigned int) std::stoi(elems[3].substr(0, elems[3].find('/'))) - 1);
			}
			else
			{
				// N-gon
				for (size_t i = 1; i < elems.size() - 2; i++)
				{
					// Fan triangulation only does convex shapes
					triangles.push_back((unsigned int) std::stoi(elems[1].substr(0, elems[1].find('/'))) - 1);
					triangles.push_back((unsigned int) std::stoi(elems[i+1].substr(0, elems[i+1].find('/'))) - 1);
					triangles.push_back((unsigned int) std::stoi(elems[i+2].substr(0, elems[i+2].find('/'))) - 1);
				}
			}
		}
		
		// Next line
		std::getline(objFile, line);
	}

	// Test prints
	/*
	for (size_t i = 0; i < vertices.size() - 2; i += 3) {
		printf("v %f %f %f\n", vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	for (size_t i = 0; i < triangles.size() - 2; i += 3) {
		printf("f %d %d %d\n", triangles[i], triangles[i + 1], triangles[i + 2]);
	}
	*/

	// Send obj data to OpenGL for rendering
	glGenVertexArrays(1, &varr);
	glGenBuffers(1, &vbuf);
	glGenBuffers(1, &ebuf);
	glBindVertexArray(varr);

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(unsigned int), triangles.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Model::drawVertexArray()
{
	glBindVertexArray(varr);
	glDrawElements(GL_TRIANGLES, triangles.size() + pointTris.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<std::string> Model::splitBySpaces(std::string& in)
{
	std::istringstream ss(in);
	std::vector<std::string> elements;
	std::string item;

	// Split string by one or more whitespace characters
	while (ss >> item) {
		elements.push_back(item);
	}
	return elements;
}

// Add the polygon representation of a point plotted by the user.
// The given vectors are that new point's vertices and faces.
void Model::addPathPoint(std::vector<float> verts, std::vector<unsigned int> tris)
{
	// For this point's faces, we have to add the number of vertices already in
	// the model (including other plotted points) to each index, so the new faces
	// match up with the new vertices.
	for (size_t i = 0; i < tris.size(); i++) {
		tris[i] += (vertices.size() + pointVerts.size()) / 3;
	}

	// Add the new point's vertices and faces to the arrays that contain all the
	// points' vertices and faces.
	pointVerts.insert(pointVerts.end(), verts.begin(), verts.end());
	pointTris.insert(pointTris.end(), tris.begin(), tris.end());

	// Send updated arrays to OpenGL
	updateOpenGLData();
}

// Remove the most recently added point.
// The arguments are the number of vertices * 3 and the number
// of triangles * 3 that correspond to one point.
void Model::removePathPoint(size_t vertsSize, size_t trisSize)
{
	// Remove point from pointVerts and pointTris data.
	for (size_t i = 0; i < vertsSize; i++) {
		pointVerts.pop_back();
	}
	for (size_t i = 0; i < trisSize; i++) {
		pointTris.pop_back();
	}

	// Send updated arrays to OpenGL
	updateOpenGLData();
}

void Model::updateOpenGLData()
{
	// Send new arrays to OpenGL, which are the model's vertices/faces
	// concatenated with all the path points' vertices/faces.
	std::vector<float> completeVerts = vertices;
	completeVerts.insert(completeVerts.end(), pointVerts.begin(), pointVerts.end());
	std::vector<unsigned int> completeTris = triangles;
	completeTris.insert(completeTris.end(), pointTris.begin(), pointTris.end());

	// Send the new data to OpenGL
	glBindVertexArray(varr);

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, completeVerts.size() * sizeof(float), completeVerts.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, completeTris.size() * sizeof(unsigned int), completeTris.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Data updated, now redraw scene to show it
	glutPostRedisplay();
}

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

// Use the path created by the user to render a
// multi-perspective image.
std::vector<unsigned char> PathTracer::renderFromPath(Camera* cam, int width, int height, void (*internalDisplay)())
{
	// Initialize variables and such
	// Array of bytes, where each pixel is three consecutive bytes for RGB.
	// For now, it's ordered top to bottom, left to right. It will be
	// transposed later.
	std::vector<unsigned char> pixels = std::vector<unsigned char>();
	if (points.size() < 2) {
		return pixels;
	}
	const float stripDecimal = 0.0125f; // CRASHES IF < 0.0125!!!
	const float edgeStripDecimal = 0.5f + (stripDecimal / 2);
	const float stripOffsetDecimal = 0.5f - (stripDecimal / 2);
	const int stripWidth = (int) glm::floor(width * stripDecimal);
	const int edgeStripWidth = (int) glm::floor(width * edgeStripDecimal);

	// Movement speed (rotation speed is derived from this)
	const float moveStep = 0.9f;

	// Camera's original position and orientation when path render started
	glm::vec3 origCamPos = cam->getPos();
	CamOrientation origCamRot = cam->getRot();

	/* First frame, contains extra data on left side of image */

	// Move camera to first point
	cam->setPos(points[0].pos);
	cam->setRot(points[0].rot);

	// Render frame at first point's position
	internalDisplay();

	// printf("c1\n");

	// Save 52.5% of pixel columns from the left.
	appendPixels(pixels, 0, 0, edgeStripWidth, height);

	// printf("c2\n");
	
	/* Loop for all points except first/last */
	for (size_t p = 0; p < points.size() - 1; p++)
	{
		// First, divide the distance between current point and next point
		// by moveStep, to see approximately how many steps it will take to
		// get from one point to the next. Then, divide the point distance
		// and orientation difference by step amount, to get an exact
		// move vector and rotate vector to apply to the camera at each step,
		// so the camera moves uniformly between the points.
		glm::vec3 travelVec = points[p + 1].pos - points[p].pos;
		glm::vec3 direction = glm::normalize(travelVec);
		float distance = glm::length(travelVec);
		int numFrames = (int)glm::round(distance / moveStep);
		float curMoveStep = distance / numFrames;
		CamOrientation rotationDiff = {
			points[p + 1].rot.pitch - points[p].rot.pitch,
			points[p + 1].rot.yaw - points[p].rot.yaw,
			points[p + 1].rot.roll - points[p].rot.roll
		};

		// Make sure the rotation done between points
		// isn't more than 180 for each axis
		if (rotationDiff.pitch > 180) {
			rotationDiff.pitch -= 360;
		}
		else if (rotationDiff.pitch < -180) {
			rotationDiff.pitch += 360;
		}
		if (rotationDiff.yaw > 180) {
			rotationDiff.yaw -= 360;
		}
		else if (rotationDiff.yaw < -180) {
			rotationDiff.yaw += 360;
		}
		if (rotationDiff.roll > 180) {
			rotationDiff.roll -= 360;
		}
		else if (rotationDiff.roll < -180) {
			rotationDiff.roll += 360;
		}

		CamOrientation curRotStep = {
			rotationDiff.pitch / numFrames,
			rotationDiff.yaw / numFrames,
			rotationDiff.roll / numFrames
		};

		/* Loop for current pair of points */
		// Exact position of first point is not included,
		// exact position of second point IS included.
		// Last frame is handled by different code.
		for (int f = 0; f < numFrames - 1; f++)
		{
			// Move and rotate camera by computed step
			cam->setPos(cam->getPos() + (direction * moveStep));
			CamOrientation initRot = cam->getRot();
			CamOrientation endRot = {
				initRot.pitch + curRotStep.pitch,
				initRot.yaw + curRotStep.yaw,
				initRot.roll + curRotStep.roll
			};
			cam->setRot(endRot);

			// Render image to back buffer
			internalDisplay();

			// Read middle 5% of pixel columns and save data
			appendPixels(pixels, (int) glm::floor(width * stripOffsetDecimal), 0, stripWidth, height);

			// Repeat for the number of steps computed earlier - 1
		}

		// For last image in this pair of points, exit the inner loop. Since
		// floats aren't exact, the position and rotation might drift a tiny
		// bit, so for this last image in the pair of points, explicitly
		// set the camera's position and orientation to that of the ending
		// point in the pair. Do this for each pair of points except
		// the pair that ends with the last point in the path.
		cam->setPos(points[p + 1].pos);
		cam->setRot(points[p + 1].rot);

		// Render image and read pixel data (if not final point in path)
		if (p < points.size() - 2) {
			internalDisplay();
			appendPixels(pixels, (int) glm::floor(width * stripOffsetDecimal), 0, stripWidth, height);
		}

		// Repeat outer loop for each pair of points in the path
	}

	// After the loop for all points is finished, we have to render the right
	// side of the image at the last point. So, render the image to the
	// back buffer and save 52.5% of pixel columns from the right.
	internalDisplay();
	appendPixels(pixels, (int) glm::floor(width * stripOffsetDecimal), 0, edgeStripWidth, height);

	// Move the camera back to its original position and orientation and request screen redraw
	cam->setPos(origCamPos);
	cam->setRot(origCamRot);

	/* End of rendering */
	

	// The render is complete. But, our pixel array is ordered top to bottom, left
	// to right. We need to transpose it so it's ordered left to right, top to bottom.
	std::vector<unsigned char> rowMajorPixels = std::vector<unsigned char>();
	int finalImageWidth = pixels.size() / height / 3;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < finalImageWidth; x++)
		{
			rowMajorPixels.push_back(pixels[(x * height * 3) + (y * 3)]);
			rowMajorPixels.push_back(pixels[(x * height * 3) + (y * 3) + 1]);
			rowMajorPixels.push_back(pixels[(x * height * 3) + (y * 3) + 2]);
		}
	}

	// Return the final vector of pixel colors
	return rowMajorPixels;
}

void PathTracer::appendPixels(std::vector<unsigned char>& dest, int xOff, int yOff, int stripWidth, int stripHeight)
{
	// printf("%d %d\n", stripWidth, stripHeight);

	// OpenGL reads from left to right, bottom to top.
	// # of bytes read = stripWidth * stripHeight * 3 bytes for RGB
	std::vector<unsigned char> pixelRead = std::vector<unsigned char>(stripWidth * stripHeight * 3);
	// Read pixel data
	// glReadBuffer(GL_BACK);
	glReadBuffer(GL_FRONT); // Testing only!
	
	// printf("c3\n");

	// Make sure we don't read past the end of the image due to imprecision or something
	if (xOff + stripWidth > 1280) {
		printf("READING PAST IMAGE EDGE!\n");
		exit(1);
	}

	glReadPixels(xOff, yOff, stripWidth, stripHeight, GL_RGB, GL_UNSIGNED_BYTE, pixelRead.data());

	// printf("c4\n");

	// Reserve data ahead of time for performance and weird issues
	dest.reserve(pixelRead.size());

	// Pixel data is read left to right, bottom to top. Add data to pixels
	// array, ordered top to bottom, left to right.
	for (int x = 0; x < stripWidth; x++)
	{
		// Start at top left
		for (int y = stripHeight - 1; y >= 0; y--)
		{
			/*
			// Check if we're within size bounds
			if ((stripWidth * 3 * y) + (x * 3) + 2 >= (int) pixelRead.size()) {
				printf("Reading past the pixelRead data!\n");
				printf("Trying to read indexes %d, %d, %d,\n", (stripWidth * 3 * y) + (x * 3),
					(stripWidth * 3 * y) + (x * 3) + 1, (stripWidth * 3 * y) + (x * 3) + 2);
				printf("but the highest valid index is %d.\n", pixelRead.size() - 1);
			}
			*/

			/*
			// Print indexes
			printf("\nReading indexes: %d, %d, %d\n", (stripWidth * 3 * y) + (x * 3),
				(stripWidth * 3 * y) + (x * 3) + 1, (stripWidth * 3 * y) + (x * 3) + 2);
			printf("Highest valid index is %d\n", pixelRead.size() - 1);
			*/

			/*
			if ((stripWidth * 3 * y) + (x * 3) < 0
				|| (stripWidth * 3 * y) + (x * 3) + 2 >= pixelRead.size()) {
				printf("INVALID READ!\n");
			}
			*/

			// Add pixel RGB
			// y * stripWidth = one row
			// x = offset from start of each row
			// Don't forget to do * 3 for RGB
			/*
			dest.push_back(pixelRead[(stripWidth * 3 * y) + (x * 3)]);
			dest.push_back(pixelRead[(stripWidth * 3 * y) + (x * 3) + 1]);
			dest.push_back(pixelRead[(stripWidth * 3 * y) + (x * 3) + 2]);
			*/
			int index = ((y * stripWidth) + x) * 3;
			/*
			if (index + 2 >= pixelRead.size()) {
				printf("OUT OF BOUNDS READ!\n");
			}
			*/
			/*
			dest.push_back(pixelRead[index]);
			dest.push_back(pixelRead[index + 1]);
			dest.push_back(pixelRead[index + 2]);
			*/


			// Write pixel's 3 bytes to master array
			dest.insert(dest.end(), pixelRead.begin() + index, pixelRead.begin() + index + 3);

		}

		// printf("c5 x=%d\n", x);
	}

}

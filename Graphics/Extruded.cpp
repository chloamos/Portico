#include <cmath>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Extruded.h"

using namespace std;

Extruded::Extruded(int samples,int slices) {
	numSamples = samples;
	numSlices = slices;
}

void Extruded::init() {
	numVertices = numSamples * (numSlices + 1);
	numIndices =  numSamples * numSlices * 6;

	for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

	// calculate the base vertices
	for (int i = 0; i < numSamples; i++) {
		vertices[i] = this->base(i);
		float s = ((float) i)/numSamples;
		texCoords[i] = glm::vec2(s,0);
	}

	// Calculate the remaining vertices
	for (int j = 1; j <= numSlices; j++) {
		float t = ((float) j)/numSlices;
		glm::mat4 tMat = this->T(t);
		for (int i = 0; i < numSamples; i++) {
			vertices[j*numSamples+i] = glm::vec3(tMat * glm::vec4(vertices[i],1.0f));
			float s = ((float) i)/numSamples;
			texCoords[j*numSamples+i] = glm::vec2(s,t);
		}
	}

	// calculate triangle indices
	for (int slice = 0; slice < numSlices; slice++) {
		for (int i = 0; i < numSamples-1; i++) {
			indices[((slice*numSamples + i) * 2) * 3 + 0] = slice*numSamples + i;
			indices[((slice*numSamples + i) * 2) * 3 + 1] = slice*numSamples + i + 1;
			indices[((slice*numSamples + i) * 2) * 3 + 2] = (slice+1)*numSamples + i;
			indices[((slice*numSamples + i) * 2 + 1) * 3 + 0] = slice*numSamples + i + 1;
			indices[((slice*numSamples + i) * 2 + 1) * 3 + 1] = (slice + 1)*numSamples + i + 1;
			indices[((slice*numSamples + i) * 2 + 1) * 3 + 2] = (slice + 1)*numSamples + i;
		}
		indices[((slice*numSamples + numSamples - 1) * 2) * 3 + 0] = slice*numSamples + numSamples - 1;
		indices[((slice*numSamples + numSamples - 1) * 2) * 3 + 1] = slice*numSamples;
		indices[((slice*numSamples + numSamples - 1) * 2) * 3 + 2] = (slice+1)*numSamples + numSamples - 1;
		indices[((slice*numSamples + numSamples - 1) * 2 + 1) * 3 + 0] = slice*numSamples;
		indices[((slice*numSamples + numSamples - 1) * 2 + 1) * 3 + 1] = (slice + 1)*numSamples;
		indices[((slice*numSamples + numSamples - 1) * 2 + 1) * 3 + 2] = (slice + 1)*numSamples + numSamples - 1;
	}
}

int Extruded::getNumVertices() { return numVertices; }
int Extruded::getNumIndices() { return numIndices; }
std::vector<int> Extruded::getIndices() { return indices; }
std::vector<glm::vec3> Extruded::getVertices() { return vertices; }
std::vector<glm::vec2> Extruded::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Extruded::getNormals() { return vertices; }

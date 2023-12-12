#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Extruded
{
private:
	int numVertices;
	int numIndices;
	int numSlices;

	std::vector<int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;

protected:
	int numSamples;

	void init();

	virtual glm::vec3 base(int i) = 0;
  virtual glm::mat4 T(float t) = 0;

public:
	Extruded(int samples,int slices);

	int getNumVertices();
	int getNumIndices();
	std::vector<int> getIndices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTexCoords();
	std::vector<glm::vec3> getNormals();
};

#include "Extruded.h"

class Cylinder : public Extruded
{
public:
  Cylinder(int samples,float h) : Extruded(samples,1) {
    height = h;

    init();
  }

protected:
  virtual glm::vec3 base(int i) {
    float s = ((float) i)/numSamples;
    return glm::vec3(cos(-s * 2.0f * 3.14159f), 0, sin(-s * 2.0f * 3.14159f));
  }

  virtual glm::mat4 T(float t) {
    return glm::translate(glm::mat4(1.0),glm::vec3(0,height,0));
  }
private:
  float height;
};

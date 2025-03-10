#include "../deserialize-utils.hpp"
#include "entity.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

// This function computes and returns a matrix that represents this transform
// Remember that the order of transformations is: Scaling, Rotation then
// Translation HINT: to convert euler angles to a rotation matrix, you can use
// glm::yawPitchRoll
glm::mat4 Transform::toMat4() const {
  // TODO: (Req 3) Write this function

  // Create scaling matrix
  glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

  // Create rotation matrix from Euler angles (yaw, pitch, roll)
  glm::mat4 rotationMat = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

  // Create translation matrix
  glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);

  // Combine transformations: Scaling → Rotation → Translation
  return translationMat * rotationMat * scaleMat;
}

// Deserializes the entity data and components from a json object
void Transform::deserialize(const nlohmann::json &data) {
  position = data.value("position", position);
  rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
  scale = data.value("scale", scale);
}

} // namespace our

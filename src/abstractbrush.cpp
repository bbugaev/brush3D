#include "abstractbrush.h"

AbstractBrush::AbstractBrush(std::vector<Vertex> vertices, TextureStorage* textureStorage)
{
    this->vertices = vertices;
    this->textureStorage = textureStorage;
}

void AbstractBrush::setRadius(double radius) {
    this->radius = radius;
}

void AbstractBrush::setColor(Color color) {
    this->color = color;
}

TextureStorage *AbstractBrush::getTextureStorage() {
    return textureStorage;
}

glm::vec3 AbstractBrush::fromScreenCoordinates(glm::vec2 point, glm::mat4x4 matrixProjection) {
    return glm::vec3(point.x / matrixProjection[0][0],
                     point.y / matrixProjection[1][1],
                     -1.0);
}

void AbstractBrush::setIdsStorage(IdsStorage *idsStorage) {
    this->idsStorage = idsStorage;
}

std::vector<std::pair<glm::i32vec2, std::pair<Color, Color>>>
        AbstractBrush::paint(glm::i32vec2 previousPoint, glm::i32vec2 lastPoint,
              glm::mat4x4 matrixModelView, glm::mat4x4 projection, glm::i32vec2 screenSize) {
    std::vector<std::pair<glm::i32vec2, std::pair<Color, Color>>> diff;

    glm::vec2 d(previousPoint - lastPoint);
    int cntRounds = glm::length(d);
    for (int i = 0; i < cntRounds; i++) {
        glm::i32vec2 currentPoint = previousPoint + (lastPoint - previousPoint) * i / cntRounds;
        auto currentPointDiff = paint(currentPoint, matrixModelView, projection, screenSize);
        diff.insert(std::end(diff), std::begin(currentPointDiff), std::end(currentPointDiff));
    }

    return diff;
}

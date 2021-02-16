#include "Server.h"

std::vector<obstacle> Server::obstacles = std::vector<obstacle>();


void Server::AddObstacle(std::vector<Point>& points, timing& tim) {
    auto center = glm::vec3(0, 0, 0);
    auto obs = obstacle(tim, center, points);
    obstacles.push_back(obs);
}

void Server::DisplayObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot) {
    for (auto & obst : obstacles) {
        GLuint VAO, VBO, VBO_color;
        auto& points = obst.points;
        std::vector<Point> colors = std::vector<Point>(points.size(), Point(1, 0, 0));

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBO_color);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), &points[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Point), &colors[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);

        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, points.size());
    }
}

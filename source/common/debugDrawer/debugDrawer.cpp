//
// Created by AICDG on 2017/8/9.
//

#include "debugDrawer.hpp"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

namespace our
{
    void DebugDrawer::initialize()
    {
        // Initialize VAO and VBO for line rendering
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

        // Allocate buffer memory (dynamic usage for frequent updates)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, nullptr, GL_DYNAMIC_DRAW);

        // Define vertex attributes (position and color)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3)); // Color
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader = new ShaderProgram();
        // Load and compile shaders (not shown here, assume shaderProgram is initialized)
        shader->attach("assets/shaders/tinted.vert", GL_VERTEX_SHADER);
        shader->attach("assets/shaders/tinted.frag", GL_FRAGMENT_SHADER);
        shader->link();
    }

    void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        std::cout << "Line starts at: (" << from.getX() << ", " << from.getY() << ", " << from.getZ() << ") ";
        std::cout << "and ends at: (" << to.getX() << ", " << to.getY() << ", " << to.getZ() << ")" << std::endl;
        // Prepare line data (positions and color)
        float lineData[] = {
            from.getX(), from.getY(), from.getZ(), color.getX(), color.getY(), color.getZ(),
            to.getX(), to.getY(), to.getZ(), color.getX(), color.getY(), color.getZ()};

        // Bind VAO and VBO
        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

        // Update buffer data
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineData), lineData);

        // Use shader program
        shader->use();

        shader->set("model", glm::mat4(1));
        shader->set("PV", glm::mat4(1));

        // Draw the line
        glDrawArrays(GL_LINES, 0, 2);

        // Unbind VAO and VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void DebugDrawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
    {
        // Calculate start and end points
        btVector3 const startPoint = pointOnB;
        btVector3 const endPoint = pointOnB + normalOnB * distance;

        // Draw the line
        drawLine(startPoint, endPoint, color);
    }

    void DebugDrawer::ToggleDebugFlag(int flag)
    {
        // checks if a flag is set and enables/
        // disables it
        if (m_debugMode & flag)
        {
            // flag is enabled, so disable it
            m_debugMode = m_debugMode & (~flag);
        }
        else
        {
            // flag is disabled, so enable it
            m_debugMode |= flag;
        }
    }

    DebugDrawer::~DebugDrawer()
    {
        glDeleteBuffers(1, &lineVBO);
        glDeleteVertexArrays(1, &lineVAO);
        delete shader;
    }
}
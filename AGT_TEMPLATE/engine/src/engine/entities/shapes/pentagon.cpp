// = The code written has been assisted with Github Copilot AI = //

#include "pch.h"
#include "pentagon.h"
#include <engine.h>
#include <cmath>

engine::pentagon::pentagon(float radius, float depth) : m_radius(radius), m_depth(depth)
{
    std::vector<mesh::vertex> pentagon_vertices;
    std::vector<uint32_t> pentagon_indices;

    const float angleStep = 2 * PI / 5; // 5 sides for a pentagon

    // Front face vertices
    for (uint32_t i = 0; i < 5; ++i)
    {
        float angle = i * angleStep;
        float x = m_radius * cosf(angle);
        float y = m_radius * sinf(angle);

        pentagon_vertices.push_back({ {x, y, m_depth / 2}, {0.0f, 0.0f, 1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Back face vertices
    for (uint32_t i = 0; i < 5; ++i)
    {
        float angle = i * angleStep;
        float x = m_radius * cosf(angle);
        float y = m_radius * sinf(angle);

        pentagon_vertices.push_back({ {x, y, -m_depth / 2}, {0.0f, 0.0f, -1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Front face indices (triangles)
    for (uint32_t i = 1; i < 4; ++i)
    {
        pentagon_indices.push_back(0);
        pentagon_indices.push_back(i);
        pentagon_indices.push_back(i + 1);
    }

    // Back face indices (triangles)
    for (uint32_t i = 6; i < 9; ++i)
    {
        pentagon_indices.push_back(5);
        pentagon_indices.push_back(i + 1);
        pentagon_indices.push_back(i);
    }

    // Side faces vertices and indices
    for (uint32_t i = 0; i < 5; ++i)
    {
        uint32_t next = (i + 1) % 5;

        // Calculate normal for the side face
        glm::vec3 v0 = pentagon_vertices[i].position;
        glm::vec3 v1 = pentagon_vertices[next].position;
        glm::vec3 v2 = pentagon_vertices[i + 5].position;
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Add vertices for the side face
        pentagon_vertices.push_back({ pentagon_vertices[i].position, normal, pentagon_vertices[i].tex_coords });
        pentagon_vertices.push_back({ pentagon_vertices[next].position, normal, pentagon_vertices[next].tex_coords });
        pentagon_vertices.push_back({ pentagon_vertices[i + 5].position, normal, pentagon_vertices[i + 5].tex_coords });
        pentagon_vertices.push_back({ pentagon_vertices[next + 5].position, normal, pentagon_vertices[next + 5].tex_coords });

        // Add indices for the side face
        uint32_t base_index = 10 + i * 4;
        pentagon_indices.push_back(base_index);
        pentagon_indices.push_back(base_index + 2);
        pentagon_indices.push_back(base_index + 1);

        pentagon_indices.push_back(base_index + 1);
        pentagon_indices.push_back(base_index + 2);
        pentagon_indices.push_back(base_index + 3);
    }

    m_mesh = engine::mesh::create(pentagon_vertices, pentagon_indices);
}

engine::pentagon::~pentagon() {}

engine::ref<engine::pentagon> engine::pentagon::create(float radius, float depth)
{
    return std::make_shared<engine::pentagon>(radius, depth);
}


// = The code written has been assisted with Github Copilot AI = //

#include "pch.h"
#include "heart.h"
#include <engine.h>
#include <cmath>

engine::heart::heart(float size, float depth) : m_size(size), m_depth(depth)
{
    std::vector<mesh::vertex> heart_vertices;
    std::vector<uint32_t> heart_indices;

    const uint32_t segments = 100;
    const float angleStep = 2 * PI / segments;

    // Front face vertices
    for (uint32_t i = 0; i <= segments; ++i)
    {
        float angle = i * angleStep;
        float x = m_size * 16 * static_cast<float>(pow(sinf(angle), 3));
        float y = m_size * (13 * cosf(angle) - 5 * cosf(2 * angle) - 2 * cosf(3 * angle) - cosf(4 * angle));

        heart_vertices.push_back({ {x, y, m_depth / 2}, {0.0f, 0.0f, 1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Back face vertices
    for (uint32_t i = 0; i <= segments; ++i)
    {
        float angle = i * angleStep;
        float x = m_size * 16 * static_cast<float>(pow(sinf(angle), 3));
        float y = m_size * (13 * cosf(angle) - 5 * cosf(2 * angle) - 2 * cosf(3 * angle) - cosf(4 * angle));

        heart_vertices.push_back({ {x, y, -m_depth / 2}, {0.0f, 0.0f, -1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Front face indices (triangles)
    for (uint32_t i = 1; i < segments; ++i)
    {
        heart_indices.push_back(i+1);
        heart_indices.push_back(i);
        heart_indices.push_back(0);
    }

    // Back face indices (triangles)
    for (uint32_t i = 1; i < segments; ++i)
    {
        heart_indices.push_back(segments + 1 + i);
        heart_indices.push_back(segments + 1 + i + 1);
        heart_indices.push_back(segments + 1);
    }

    // Side faces vertices and indices
    for (uint32_t i = 0; i < segments; ++i)
    {
        uint32_t next = (i + 1) % segments;

        // Calculate normal for the side face
        glm::vec3 v0 = heart_vertices[i].position;
        glm::vec3 v1 = heart_vertices[next].position;
        glm::vec3 v2 = heart_vertices[i + segments + 1].position;
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Add vertices for the side face
        heart_vertices.push_back({ heart_vertices[i].position, normal, heart_vertices[i].tex_coords });
        heart_vertices.push_back({ heart_vertices[next].position, normal, heart_vertices[next].tex_coords });
        heart_vertices.push_back({ heart_vertices[i + segments + 1].position, normal, heart_vertices[i + segments + 1].tex_coords });
        heart_vertices.push_back({ heart_vertices[next + segments + 1].position, normal, heart_vertices[next + segments + 1].tex_coords });

        // Add indices for the side face
        uint32_t base_index = 2 * (segments + 1) + i * 4;
        heart_indices.push_back(base_index + 1);
        heart_indices.push_back(base_index + 2);
        heart_indices.push_back(base_index);

        heart_indices.push_back(base_index + 3);
        heart_indices.push_back(base_index + 2);
        heart_indices.push_back(base_index + 1);
    }

    m_mesh = engine::mesh::create(heart_vertices, heart_indices);
}

engine::heart::~heart() {}

engine::ref<engine::heart> engine::heart::create(float size, float depth)
{
    return std::make_shared<engine::heart>(size, depth);
}

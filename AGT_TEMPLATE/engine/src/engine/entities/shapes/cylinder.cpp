// = The code written has been assisted with Github Copilot AI = //

#include "pch.h"
#include "cylinder.h"
#include <engine.h>
#include <cmath>

engine::cylinder::cylinder(float radius, float height, uint32_t segments)
    : m_radius(radius), m_height(height), m_segments(segments)
{
    std::vector<mesh::vertex> cylinder_vertices;
    std::vector<uint32_t> cylinder_indices;

    const float angleStep = 2 * PI / m_segments;

    // Top face vertices
    for (uint32_t i = 0; i < m_segments; ++i)
    {
        float angle = i * angleStep;
        float x = m_radius * cosf(angle);
        float y = m_radius * sinf(angle);

        cylinder_vertices.push_back({ {x, y, m_height / 2}, {0.0f, 0.0f, 1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Bottom face vertices
    for (uint32_t i = 0; i < m_segments; ++i)
    {
        float angle = i * angleStep;
        float x = m_radius * cosf(angle);
        float y = m_radius * sinf(angle);

        cylinder_vertices.push_back({ {x, y, -m_height / 2}, {0.0f, 0.0f, -1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Top face indices (triangles)
    for (uint32_t i = 1; i < m_segments - 1; ++i)
    {
        cylinder_indices.push_back(0);
        cylinder_indices.push_back(i);
        cylinder_indices.push_back(i + 1);
    }

    // Bottom face indices (triangles)
    for (uint32_t i = 1; i < m_segments - 1; ++i)
    {
        cylinder_indices.push_back(m_segments);
        cylinder_indices.push_back(m_segments + i + 1);
        cylinder_indices.push_back(m_segments + i);
    }

    // Side faces vertices and indices
    for (uint32_t i = 0; i < m_segments; ++i)
    {
        uint32_t next = (i + 1) % m_segments;

        // Calculate normal for the side face
        glm::vec3 v0 = cylinder_vertices[i].position;
        glm::vec3 v1 = cylinder_vertices[next].position;
        glm::vec3 v2 = cylinder_vertices[i + m_segments].position;
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Add vertices for the side face
        cylinder_vertices.push_back({ cylinder_vertices[i].position, normal, cylinder_vertices[i].tex_coords });
        cylinder_vertices.push_back({ cylinder_vertices[next].position, normal, cylinder_vertices[next].tex_coords });
        cylinder_vertices.push_back({ cylinder_vertices[i + m_segments].position, normal, cylinder_vertices[i + m_segments].tex_coords });
        cylinder_vertices.push_back({ cylinder_vertices[next + m_segments].position, normal, cylinder_vertices[next + m_segments].tex_coords });

        // Add indices for the side face
        uint32_t base_index = 2 * m_segments + i * 4;
        cylinder_indices.push_back(base_index);
        cylinder_indices.push_back(base_index + 2);
        cylinder_indices.push_back(base_index + 1);

        cylinder_indices.push_back(base_index + 1);
        cylinder_indices.push_back(base_index + 2);
        cylinder_indices.push_back(base_index + 3);
    }

    m_mesh = engine::mesh::create(cylinder_vertices, cylinder_indices);
}

engine::cylinder::~cylinder() {}

engine::ref<engine::cylinder> engine::cylinder::create(float radius, float height, uint32_t segments)
{
    return std::make_shared<engine::cylinder>(radius, height, segments);
}

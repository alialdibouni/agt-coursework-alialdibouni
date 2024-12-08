#include "pch.h"
#include "cone.h"
#include <engine.h>
#include <cmath>

engine::cone::cone(float radius, float height, uint32_t segments)
    : m_radius(radius), m_height(height), m_segments(segments)
{
    std::vector<mesh::vertex> cone_vertices;
    std::vector<uint32_t> cone_indices;

    const float angleStep = 2 * PI / m_segments;

    // Base vertices
    for (uint32_t i = 0; i < m_segments; ++i)
    {
        float angle = i * angleStep;
        float x = m_radius * cosf(angle);
        float y = m_radius * sinf(angle);

        cone_vertices.push_back({ {x, y, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)} });
    }

    // Apex vertex
    cone_vertices.push_back({ {0.0f, 0.0f, m_height}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f} });

    // Base center vertex
    cone_vertices.push_back({ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.5f} });

    // Base indices (triangles)
    for (uint32_t i = 1; i < m_segments - 1; ++i)
    {
        cone_indices.push_back(m_segments + 1); // center vertex
        cone_indices.push_back(i + 1);
        cone_indices.push_back(i);
    }

    // Side faces indices (triangles)
    for (uint32_t i = 0; i < m_segments; ++i)
    {
        uint32_t next = (i + 1) % m_segments;

        // Calculate normal for the side face
        glm::vec3 v0 = cone_vertices[i].position;
        glm::vec3 v1 = cone_vertices[next].position;
        glm::vec3 v2 = cone_vertices[m_segments].position; // apex vertex
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Add vertices for the side face
        cone_vertices.push_back({ cone_vertices[i].position, normal, cone_vertices[i].tex_coords });
        cone_vertices.push_back({ cone_vertices[next].position, normal, cone_vertices[next].tex_coords });
        cone_vertices.push_back({ cone_vertices[m_segments].position, normal, cone_vertices[m_segments].tex_coords });

        // Add indices for the side face
        uint32_t base_index = m_segments + 2 + i * 3;
        cone_indices.push_back(base_index);
        cone_indices.push_back(base_index + 1);
        cone_indices.push_back(base_index + 2);
    }

    m_mesh = engine::mesh::create(cone_vertices, cone_indices);
}

engine::cone::~cone() {}

std::shared_ptr<engine::cone> engine::cone::create(float radius, float height, uint32_t segments)
{
    return std::make_shared<cone>(radius, height, segments);
}

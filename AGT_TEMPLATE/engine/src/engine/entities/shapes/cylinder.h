#pragma once

namespace engine
{
    class mesh;

    /// \brief Class creating a 3D cylinder object with a mesh of a specified size
    class cylinder
    {
    public:
        /// \brief Constructor
        cylinder(float radius, float height, uint32_t segments);

        /// \brief Destructor
        ~cylinder();

        /// \brief Getter methods
        float radius() const { return m_radius; }
        float height() const { return m_height; }
        uint32_t segments() const { return m_segments; }
        ref<engine::mesh> mesh() const { return m_mesh; }

        static ref<cylinder> create(float radius, float height, uint32_t segments);
    private:
        /// \brief Fields
        // Radius of the cylinder
        float m_radius;
        // Height of the cylinder
        float m_height;
        // Number of segments for the cylinder
        uint32_t m_segments;

        ref<engine::mesh> m_mesh;
    };
}

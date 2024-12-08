// = The code written has been assisted with Github Copilot AI = //

#pragma once

namespace engine
{
    class mesh;

    /// \brief Class creating a 3D cone object with a mesh of a specified size
    class cone
    {
    public:
        /// \brief Constructor
        cone(float radius, float height, uint32_t segments);

        /// \brief Destructor
        ~cone();

        /// \brief Getter methods
        float radius() const { return m_radius; }
        float height() const { return m_height; }
        uint32_t segments() const { return m_segments; }
        ref<engine::mesh> mesh() const { return m_mesh; }

        static ref<cone> create(float radius, float height, uint32_t segments);
    private:
        /// \brief Fields
        // Radius of the cone base
        float m_radius;
        // Height of the cone
        float m_height;
        // Number of segments for the cone base
        uint32_t m_segments;

        ref<engine::mesh> m_mesh;
    };
}

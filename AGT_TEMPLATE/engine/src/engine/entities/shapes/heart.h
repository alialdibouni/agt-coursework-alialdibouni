// = The code written has been assisted with Github Copilot AI = //

#pragma once

namespace engine
{
    class mesh;

    /// \brief Class creating a 3D heart object with a mesh of a specified size
    class heart
    {
    public:
        /// \brief Constructor
        heart(float size, float depth);

        /// \brief Destructor
        ~heart();

        /// \brief Getter methods
        float size() const { return m_size; }
        float depth() const { return m_depth; }
        ref<engine::mesh> mesh() const { return m_mesh; }

        static ref<heart> create(float size, float depth);
    private:
        /// \brief Fields
        // Size of the heart
        float m_size;
        // Depth of the heart
        float m_depth;

        ref<engine::mesh> m_mesh;
    };
}

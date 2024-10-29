#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a 3D pentagon object with a mesh of a specified size
	class pentagon
	{
	public:
		/// \brief Constructor
		pentagon(float radius, float depth);

		/// \brief Destructor
		~pentagon();

		/// \brief Getter methods
		float radius() const { return m_radius; }
		float depth() const { return m_depth; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<pentagon> create(float radius, float depth);
	private:
		/// \brief Fields
		// Radius of the pentagon
		float m_radius;
		// Depth of the pentagon
		float m_depth;

		ref<engine::mesh> m_mesh;
	};
}

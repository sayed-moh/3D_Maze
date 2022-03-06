#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>



namespace our {

    // An enum that defines the type of the light (DIRECTIONAL or POINT or SPOT) 
    enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    // This component defines the parameters of the light except its position and direction
    class LightComponent : public Component {
    public:
        // The type of the light
        LightType lightType; 
        
        // This defines the color and intensity of the light.
        glm::vec3 color;

        // Attentuation factors are used for point and spot lights.
        glm::vec3 attenuation;
    
        // Cone angles are used for spot lights.
        glm::vec2 cone_angles;

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads Light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };
}
#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/light.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{
    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
    };
    // struct Light stores the light and its direction and position
    struct Light {
        LightComponent* lightComponent;
        glm::vec3 position;
        glm::vec3 direction;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        std::vector<Light> lights;
    public:
        // This function should be called every frame to draw the given world
        // Both viewportStart and viewportSize are using to define the area on the screen where we will draw the scene
        // viewportStart is the lower left corner of the viewport (in pixels)
        // viewportSize is the width & height of the viewport (in pixels). It is also used to compute the aspect ratio
        void render(World* world, glm::ivec2 viewportStart, glm::ivec2 viewportSize){
            // First of all, we search for a camera and for all the mesh renderers
            CameraComponent* camera = nullptr;
            opaqueCommands.clear();
            transparentCommands.clear();
            for(auto entity : world->getEntities()){
                // If we hadn't found a camera yet, we look for a camera in this entity
                if(!camera) camera = entity->getComponent<CameraComponent>();
                // If this entity has a mesh renderer component
                if(auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer){
                    // We construct a command from it
                    RenderCommand command;
                    command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                    command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                    command.mesh = meshRenderer->mesh;
                    command.material = meshRenderer->material;
                    // if it is transparent, we add it to the transparent commands list
                    if(command.material->transparent){
                        transparentCommands.push_back(command);
                    } else {
                    // Otherwise, we add it to the opaque command list
                        opaqueCommands.push_back(command);
                    }
                }
                // If this entity is a LightComponent
                if(auto lightComponent = entity->getComponent<LightComponent>(); lightComponent){
                    Light light;
                    light.lightComponent = lightComponent;
                    //Calculate light direction
                    light.direction = lightComponent->getOwner()->getLocalToWorldMatrix() * glm::vec4(0,0,-1,0);
                    //Calculate light position
                    light.position = lightComponent->getOwner()->getLocalToWorldMatrix() * glm::vec4(0,0,0,1);
                    lights.push_back(light);
                }
            }

            // If there is no camera, we return (we cannot render without a camera)
            if(camera == nullptr) return;

            //TODO: Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
            // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
            glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0,0,-1,0);
            std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
                //TODO: Finish this function
                // HINT: the following return should return true "first" should be drawn before "second".
                glm::vec3 AB = second.center - first.center;
                GLfloat d = dot(cameraForward,AB);
                if(d<0) return true; 
                return false;
            });

            //TODO: Get the camera ViewProjection matrix and store it in VP
            glm::mat4 VP = camera->getProjectionMatrix(viewportSize) * camera->getViewMatrix();
            
            //TODO: Set the OpenGL viewport using viewportStart and viewportSize
            glViewport(viewportStart.x,viewportStart.y,viewportSize.x,viewportSize.y);
            
            //TODO: Set the clear color to black and the clear depth to 1
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0f);
            
            //TODO: Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
            glDepthMask(true);
            glColorMask(true, true, true, true);
            
            //TODO: Clear the color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //TODO: Draw all the opaque commands followed by all the transparent commands
            // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
            std::sort(opaqueCommands.begin(), opaqueCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
                glm::vec3 AB = second.center - first.center;
                GLfloat d = dot(cameraForward,AB);
                if(d>0) return true; 
                return false;
            });

            //Loop on the opaqueCommands to draw them
            for (auto comp = begin (opaqueCommands); comp != end (opaqueCommands); ++comp) {
                //Check if the material is Lit or not to set values for its shader
                if(dynamic_cast<LitMaterial*> (comp->material)){
                    comp->material->setup();
                    comp->material->shader->set("object_to_world",comp->localToWorld);
                    comp->material->shader->set("object_to_world_inv_transpose",transpose(inverse(comp->localToWorld)));
                    comp->material->shader->set("view_projection",VP);
                    comp->material->shader->set("camera_position",camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0,0,0,1));

                    // We will go through all the lights and send them to the shader.
                    int light_index = 0;
                    const int MAX_LIGHT_COUNT = 16;
                    //Loop on all lights
                    for(const auto& light : lights) {
                        std::string prefix = "lights[" + std::to_string(light_index) + "].";

                        comp->material->shader->set(prefix + "type", static_cast<int>(light.lightComponent->lightType));
                        comp->material->shader->set(prefix + "color", light.lightComponent->color);

                        switch (light.lightComponent->lightType) {
                            case LightType::DIRECTIONAL:
                                comp->material->shader->set(prefix + "direction", glm::normalize(light.direction));
                                break;
                            case LightType::POINT:
                                comp->material->shader->set(prefix + "position", light.position);
                                comp->material->shader->set(prefix + "attenuation_constant", light.lightComponent->attenuation.x);
                                comp->material->shader->set(prefix + "attenuation_linear", light.lightComponent->attenuation.y);
                                comp->material->shader->set(prefix + "attenuation_quadratic", light.lightComponent->attenuation.z);
                                break;
                            case LightType::SPOT:
                                comp->material->shader->set(prefix + "position", light.position);
                                comp->material->shader->set(prefix + "direction", glm::normalize(light.direction));
                                comp->material->shader->set(prefix + "attenuation_constant", light.lightComponent->attenuation.x);
                                comp->material->shader->set(prefix + "attenuation_linear", light.lightComponent->attenuation.y);
                                comp->material->shader->set(prefix + "attenuation_quadratic", light.lightComponent->attenuation.z);
                                comp->material->shader->set(prefix + "inner_angle", glm::radians(light.lightComponent->cone_angles.x));
                                comp->material->shader->set(prefix + "outer_angle", glm::radians(light.lightComponent->cone_angles.y));
                                break;
                        }
                        light_index++;
                        if(light_index >= MAX_LIGHT_COUNT) break;
                    }
                    // Since the light array in the shader has a constant size, we need to tell the shader how many lights we sent.
                    comp->material->shader->set("light_count", light_index);

                    comp->mesh->draw();
                }
                else{
                    comp->material->setup();
                    comp->material->shader->set("transform",VP * comp->localToWorld);
                    comp->mesh->draw();
                }
            }

            //Loop on the transparentCommands to draw them
            for (auto comp = begin (transparentCommands); comp != end (transparentCommands); ++comp) {
                //Check if the material is Lit or not to set values for its shader
                if(dynamic_cast<LitMaterial*> (comp->material)){
                    comp->material->setup();
                    comp->material->shader->set("object_to_world",comp->localToWorld);
                    comp->material->shader->set("object_to_world_inv_transpose",transpose(inverse(comp->localToWorld)));
                    comp->material->shader->set("view_projection",VP);
                    comp->material->shader->set("camera_position",camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0,0,0,1));

                    // We will go through all the lights and send them to the shader.
                    int light_index = 0;
                    const int MAX_LIGHT_COUNT = 16;
                    for(const auto& light : lights) {
                        std::string prefix = "lights[" + std::to_string(light_index) + "].";

                        comp->material->shader->set(prefix + "type", static_cast<int>(light.lightComponent->lightType));
                        comp->material->shader->set(prefix + "color", light.lightComponent->color);

                        switch (light.lightComponent->lightType) {
                            case LightType::DIRECTIONAL:
                                comp->material->shader->set(prefix + "direction", glm::normalize(light.direction));
                                break;
                            case LightType::POINT:
                                comp->material->shader->set(prefix + "position", light.position);
                                comp->material->shader->set(prefix + "attenuation_constant", light.lightComponent->attenuation.x);
                                comp->material->shader->set(prefix + "attenuation_linear", light.lightComponent->attenuation.y);
                                comp->material->shader->set(prefix + "attenuation_quadratic", light.lightComponent->attenuation.z);
                                break;
                            case LightType::SPOT:
                                comp->material->shader->set(prefix + "position", light.position);
                                comp->material->shader->set(prefix + "direction", glm::normalize(light.direction));
                                comp->material->shader->set(prefix + "attenuation_constant", light.lightComponent->attenuation.x);
                                comp->material->shader->set(prefix + "attenuation_linear", light.lightComponent->attenuation.y);
                                comp->material->shader->set(prefix + "attenuation_quadratic", light.lightComponent->attenuation.z);
                                comp->material->shader->set(prefix + "inner_angle", light.lightComponent->cone_angles.x);
                                comp->material->shader->set(prefix + "outer_angle", light.lightComponent->cone_angles.y);
                                break;
                        }
                        light_index++;
                        if(light_index >= MAX_LIGHT_COUNT) break;
                    }
                    // Since the light array in the shader has a constant size, we need to tell the shader how many lights we sent.
                    comp->material->shader->set("light_count", light_index);

                    comp->mesh->draw();
                }
                else {
                    comp->material->setup();
                    comp->material->shader->set("transform",VP * comp->localToWorld);
                    comp->mesh->draw();
                }
            }
        };


    };

}
#pragma once

#include <shader/shader.hpp>
#include <mesh/mesh.hpp>
#include <texture/texture2d.hpp>
#include <texture/texture-utils.hpp>
#include <application.hpp>

// This state tests and shows how to use the Texture2D class.
class MenuState: public our::State {

    our::ShaderProgram* shader;
    our::Mesh* mesh;
    our::Texture2D* texture1;
    our::Texture2D* texture2;
    bool firstTime = true;
    int state = 0;
    
    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // Then we load the shader that will be used for this scene
        shader = new our::ShaderProgram();
        shader->attach("assets/shaders/texture-test.vert", GL_VERTEX_SHADER);
        shader->attach("assets/shaders/texture-test.frag", GL_FRAGMENT_SHADER);
        shader->link();
        
        // We create a simple 2D plane to use for viewing the plane
        std::vector<our::Vertex> vertices = {
            { {-1, -1,  0}, {255, 255, 255, 255}, {0.00, 0.00}, {0, 0, 1} },
            { { 1, -1,  0}, {255, 255, 255, 255}, {1.00, 0.00}, {0, 0, 1} },
            { { 1,  1,  0}, {255, 255, 255, 255}, {1.00, 1.00}, {0, 0, 1} },
            { {-1,  1,  0}, {255, 255, 255, 255}, {0.00, 1.00}, {0, 0, 1} },
        };
        std::vector<unsigned int> elements = {
            0, 1, 2,
            2, 3, 0,
        };
        mesh = new our::Mesh(vertices, elements);
        
        // Then we create a texture and load an image into it
        texture1 = new our::Texture2D();
        texture2 = new our::Texture2D();
        auto images = config["menu"];
        our::texture_utils::loadImage(*texture1, images.value("play", "").c_str());
        our::texture_utils::loadImage(*texture2, images.value("exit", "").c_str());
    }

    void onDraw(double deltaTime) override {
        //When user press ENTER, based on which state the pointer was in the menue it changes the state (0 for PLAY, 1 for EXIT)
        if (getApp()->getKeyboard().justReleased(GLFW_KEY_ENTER)){
            if(state == 0){
                getApp()->changeState("main");
            }
            else {
                getApp()->~Application();
            }
        }
        //Draws the menu state and pointer is at the PLAY word
        if(firstTime){
            glClear(GL_COLOR_BUFFER_BIT);
            shader->use();
            // Here we set the active texture unit to 0 then bind the texture to it
            glActiveTexture(GL_TEXTURE0);
            texture1->bind();
            // Then we send 0 (the index of the texture unit we used above) to the "tex" uniform
            shader->set("tex", 0);
            mesh->draw();
        }
        //When user press down arrow the pointer moves to the EXIT word
        if (getApp()->getKeyboard().isPressed(GLFW_KEY_DOWN)){
            firstTime = false;
            state = 1;
            glClear(GL_COLOR_BUFFER_BIT);
            shader->use();
            // Here we set the active texture unit to 0 then bind the texture to it
            glActiveTexture(GL_TEXTURE0);
            texture2->bind();
            // Then we send 0 (the index of the texture unit we used above) to the "tex" uniform
            shader->set("tex", 0);
            mesh->draw();
        }
        //When user press up arrow the pointer moves to the PLAY word
        else if(getApp()->getKeyboard().isPressed(GLFW_KEY_UP)){
            firstTime = false;
            state = 0;
            glClear(GL_COLOR_BUFFER_BIT);
            shader->use();
            // Here we set the active texture unit to 0 then bind the texture to it
            glActiveTexture(GL_TEXTURE0);
            texture1->bind();
            // Then we send 0 (the index of the texture unit we used above) to the "tex" uniform
            shader->set("tex", 0);
            mesh->draw();
        }
    }

    void onDestroy() override {
        delete shader;
        delete mesh;
        delete texture1;
        delete texture2;
    }
};
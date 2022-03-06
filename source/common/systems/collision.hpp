#pragma once
#include "../ecs/world.hpp"

#include <glm/glm.hpp>
#include <iostream>

namespace our
{

    //This system is reponsible for defferent type of collision detection.
    class CollisionSystem {

        //Co-ordinates of the maze walls
        glm::vec4 walls[16] = {
                //(small thickness(x), big z , big thickness(x), small z)
                glm::vec4(36, 36, 40, -36),
                glm::vec4(-40, 36, -35, -36),
                glm::vec4(10, 27, 15, -15),
                glm::vec4(-15, 36, -10, 23),
                glm::vec4(-27, 27, -22, 10),
                glm::vec4(-27, -10, -22, -27),
                glm::vec4(-2, 15, 2, -15),
                glm::vec4(23, 13, 27, -25),
                glm::vec4(23, 36, 27, 24),
                //(small x, big thickness z,big x ,small thickness z)
                glm::vec4(-36, 40, 36, 36),
                glm::vec4(-12, 27, 13, 23),
                glm::vec4(-25, 15, 25, 10),
                glm::vec4(-36, 2, -25, -2),
                glm::vec4(-13, -9, 0, -15),
                glm::vec4(-25, -22, 27, -27),
                glm::vec4(-36, -35, 36, -40)
            };

        //Winning Location
        glm::vec3 win = glm::vec3(-34,0,10);

        //Obstacles bounding boxes
        glm::vec4 deaths[3] ={
            glm::vec4(35, 27, -1,-9),
            glm::vec4(-22,-29,-3,-11),
            glm::vec4(9,1,24,16)
        };

        //Player health
        int health = 3;
    public:

        // This function checks for collision with maze walls. 
        bool checkCollision(glm::vec3 newPosition) {
            for(int i = 0;i < 16;i++){
                if(newPosition.x >= walls[i].r && newPosition.x <= walls[i].b && newPosition.z <= walls[i].g && newPosition.z >= walls[i].a){
                    return true;
                }
            }
            return false;
        }

        // This function checks if the player have escaped from the maze or not
        bool isWin(glm::vec3 newPosition){
            if(newPosition.x < win.x && newPosition.z > win.y && newPosition.z < win.z){
                return true;
            }
            return false;
        }

        // This function checks if the player hit an obstacle to delete this obstacle and decrease number of lives remaining
        bool checkDeath(glm::vec3 newPosition,World* world){
            // Loop on the 3 Obstacles
            for(int i = 0;i < 3;i++){
                //Checks if the player position is collided with the obstacle bounding boxe 
                if(newPosition.x <= deaths[i].r && newPosition.x >= deaths[i].g && newPosition.z <= deaths[i].b && newPosition.z >= deaths[i].a){
                    //Loop on the entites of the world
                    for(auto entity : world->getEntities()){
                        //If the player is collided with the first obstacle
                        if(entity->name == "death1" && i == 0){
                            world->markForRemoval(entity);
                            health -= 1;
                            //Loop on entites again to remove first live
                            for(auto health : world->getEntities()){
                                if(health->name == "health1"){
                                    world->markForRemoval(health);
                                }
                            }
                            return true;
                        }
                        //If the player is collided with the second obstacle
                        else if(entity->name == "death2" && i == 1){
                            world->markForRemoval(entity);
                            health -= 1;
                            //Loop on entites again to remove second live
                            for(auto health : world->getEntities()){
                                if(health->name == "health2"){
                                    world->markForRemoval(health);
                                }
                            }
                            return true;

                        }
                        //If the player is collided with the second obstacle
                        else if(entity->name == "death3" && i == 2){
                            world->markForRemoval(entity);
                            health -= 1;
                            //Loop on entites again to remove second live
                            for(auto health : world->getEntities()){
                                if(health->name == "health3"){
                                    world->markForRemoval(health);
                                }
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        }
        
        //This function checks if the player has consumed his 3 lives
        bool isDead(){
            if(health==0){
                return true;
            }
            return false;
        }
    };

}

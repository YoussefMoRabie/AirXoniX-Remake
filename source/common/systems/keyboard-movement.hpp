#pragma once

#include "../ecs/world.hpp"
#include "../components/keyboard-movement.hpp"
#include "../systems/area-coverage.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{
    // The free camera controller system is responsible for moving every entity which contains a KeyboardMovementComponent.

    class KeyboardMovementSystem {
        Application *app; // The application in which the state runs

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }

        // This should be called every frame to update all entities
        void update(World *world, float deltaTime, AreaCoverageSystem *areaCoverageSystem) {
            // if the time between 2 calls is too high, it means the game was paused
            if(deltaTime > 0.1) return;
            // We search for an entity containing both a KeyboardMovementComponent
            // As soon as we find one, we break
            KeyboardMovementComponent *move = nullptr;
            for(auto entity : world->getEntities()){
                move = entity->getComponent<KeyboardMovementComponent>();
                if(move) break;
            }
            if(!move) return;
            // Get the entity that we found via getOwner of move
            Entity* player = move->getOwner();
            // We get a reference to the entity's position
            glm::vec3& position = player->localTransform.position;
            // get position sensitivity of the entity from the keyboard-movement component
            glm::vec3 sensitivity = move->positionSensitivity;

            // we also get the camera entity since we need to move it with the player
            CameraComponent* camera = nullptr;
            for(auto cameraEntity : world->getEntities()){
                camera = cameraEntity->getComponent<CameraComponent>();
                if(camera) break;
            }
            Entity* cameraEntity = camera->getOwner();
            glm::vec3& cameraPosition = cameraEntity->localTransform.position;
            auto playerMovement = player->getComponent<MovementComponent>();
            auto cameraMovement = cameraEntity->getComponent<MovementComponent>();
            // determine whether the player is moving on their own or automatic (while building blocks)
            if(areaCoverageSystem->isBuilding())
            {
                if(app->getKeyboard().isPressed(GLFW_KEY_W) || app->getKeyboard().justReleased(GLFW_KEY_W))
                {
                    if(playerMovement->linearVelocity != glm::vec3{0, 0, AUTO_MOVEMENT_SPEED}){
                        playerMovement->linearVelocity = glm::vec3{0, 0, -AUTO_MOVEMENT_SPEED};
                        cameraMovement->linearVelocity = glm::vec3{0, 0, -0.5 * AUTO_MOVEMENT_SPEED};
                    }
                }
                else if(app->getKeyboard().isPressed(GLFW_KEY_S) || app->getKeyboard().justReleased(GLFW_KEY_S))
                {
                    if(playerMovement->linearVelocity != glm::vec3{0, 0, -AUTO_MOVEMENT_SPEED}){
                        playerMovement->linearVelocity = glm::vec3{0, 0, AUTO_MOVEMENT_SPEED};
                        cameraMovement->linearVelocity = glm::vec3{0, 0, 0.5 * AUTO_MOVEMENT_SPEED};
                    }
                }
                else if(app->getKeyboard().isPressed(GLFW_KEY_A) || app->getKeyboard().justReleased(GLFW_KEY_A))
                {
                    if(playerMovement->linearVelocity != glm::vec3{AUTO_MOVEMENT_SPEED, 0, 0}){
                        playerMovement->linearVelocity = glm::vec3{-AUTO_MOVEMENT_SPEED, 0, 0};
                        cameraMovement->linearVelocity = glm::vec3{-0.5 * AUTO_MOVEMENT_SPEED, 0, 0};                    }
                }
                else if(app->getKeyboard().isPressed(GLFW_KEY_D) || app->getKeyboard().justReleased(GLFW_KEY_D))
                {
                    if(playerMovement->linearVelocity != glm::vec3{-AUTO_MOVEMENT_SPEED, 0, 0}){
                        playerMovement->linearVelocity = glm::vec3{AUTO_MOVEMENT_SPEED, 0, 0};
                        cameraMovement->linearVelocity = glm::vec3{0.5 * AUTO_MOVEMENT_SPEED, 0, 0};                    }
                }
                app->soundPlayer.loopSound("player_drawWall");
            }
            else{
                playerMovement->linearVelocity = glm::vec3{0, 0, 0};
                cameraMovement->linearVelocity = glm::vec3{0, 0, 0};
                // We change the camera position based on the keys WASD
                // S & W moves the player back and forth
                if(app->getKeyboard().isPressed(GLFW_KEY_W) && position.z >= -ARENA_LENGTH)
                {
                    position.z -= deltaTime * sensitivity.z;
                    cameraPosition.z -= deltaTime * sensitivity.z * 0.5;
                }
                if(app->getKeyboard().isPressed(GLFW_KEY_S) && position.z <= ARENA_LENGTH)
                {
                    position.z += deltaTime * sensitivity.z;
                    cameraPosition.z += deltaTime * sensitivity.z * 0.5;
                }

                // A & D moves the player left or right (prioritize vertical movement)
                if(!(app->getKeyboard().isPressed(GLFW_KEY_W) || app->getKeyboard().isPressed(GLFW_KEY_S)))
                {
                    if(app->getKeyboard().isPressed(GLFW_KEY_D) && position.x <= ARENA_LENGTH) {
                        position.x += deltaTime * sensitivity.x;
                        cameraPosition.x += deltaTime * sensitivity.x * 0.5;
                    }
                    if(app->getKeyboard().isPressed(GLFW_KEY_A) && position.x >= -ARENA_LENGTH) {
                        position.x -= deltaTime * sensitivity.x;
                        cameraPosition.x -= deltaTime * sensitivity.x * 0.5;
                    }
                }

                app->soundPlayer.stopSound("player_drawWall");
            }
        };
    };
}

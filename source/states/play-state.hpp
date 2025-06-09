#pragma once

#include <application.hpp>
#include <asset-loader.hpp>
#include <entities/world.hpp>
#include <iostream>
#include <systems/Input.hpp>
#include <systems/enemy-ai.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/health.hpp>
#include <systems/light.hpp>
#include <systems/movement.hpp>
#include <systems/physics.hpp>
#include <systems/player.hpp>
#include <systems/shooting.hpp>
#include <systems/sound.hpp>

#include <components/health.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State {
    our::World world;
    our::ForwardRenderer renderer;
    our::InputSystem inputSystem;
    our::MovementSystem movementSystem;
    our::LightSystem lightingSystem;
    our::PhysicsSystem physicsSystem;
    our::PlayerSystem playerSystem;
    our::ShootingSystem shootingSystem;
    our::HealthSystem healthSystem;
    our::EnemyAISystem enemyAISystem;
    our::SoundSystem* soundSystem = nullptr;

    ALuint backgroundMusicSource = 0;
    bool isMusicPlaying = false;

    void onInitialize() override {
        soundSystem = &getApp()->getSound();
        // Reset music state
        isMusicPlaying = false;
        backgroundMusicSource = 0;

        // Stop menu soundtrack
        soundSystem->stopSound("intro-music");
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets")) {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our
        // world
        if (config.contains("world")) {
            world.initializePhysics();
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer
        // to the app
        inputSystem.enter(getApp());
        playerSystem.enter(getApp());
        shootingSystem.enter(getApp());
        healthSystem.enter(getApp());

        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);

        // Play background music
        soundSystem->loadSound("background-music",
                               "assets/sounds/background-music.wav");
        backgroundMusicSource =
            soundSystem->createLoopingSource("background-music");

        // Start playing once
        if (!isMusicPlaying) {
            soundSystem->playSource(backgroundMusicSource);
            isMusicPlaying = true;
        }
    }

    void onImmediateGui() override {
        // get the heart texture id from the asset loader
        GLuint heartTexture = our::AssetLoader<our::Texture2D>::get("heart")->getOpenGLName();
        GLuint heartDarkTexture = our::AssetLoader<our::Texture2D>::get("heart-dark")->getOpenGLName();

        // get the player entity
        auto playerEntity = world.getEntitiesByTag("Player").front();
        // get the player component
        auto playerHealthComponent = playerEntity->getComponent<our::HealthComponent>();
        // get the player health
        int playerHealth = playerHealthComponent->getCurrentHealth();
        int playerDefaultHealth = playerHealthComponent->getDefaultHealth();

        // Here we can draw the immediate GUI (if any)
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
        ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
        for (int i = 0; i < playerDefaultHealth - playerHealth; ++i) {
            ImGui::Image((void*)(intptr_t)heartDarkTexture, ImVec2(32, 32));
            ImGui::SameLine();
        }
        for (int i = 0; i < playerHealth; ++i) {
            ImGui::Image((void*)(intptr_t)heartTexture, ImVec2(32, 32));
            ImGui::SameLine();
        }
        ImGui::End();
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        inputSystem.update(&world, (float)deltaTime);
        enemyAISystem.update(&world, (float)deltaTime);
        shootingSystem.update(&world, (float)deltaTime);
        lightingSystem.update(&world, (float)deltaTime);

        // Apply physics to the world
        physicsSystem.update(&world, (float)deltaTime);

        playerSystem.update(&world, (float)deltaTime);

        // Check for collisions and apply damage to the entities
        healthSystem.update(&world, (float)deltaTime);

        // And finally we use the renderer system to draw the scene
        renderer.update(&world, (float)deltaTime);

        world.deleteMarkedEntities();

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if (keyboard.isPressed(GLFW_KEY_V)) {
            std::cout << "location: ";
            std::cout << world.getEntitiesByTag("Player")[0]
                             ->localTransform.position.x
                      << " ";
            std::cout << world.getEntitiesByTag("Player")[0]
                             ->localTransform.position.y
                      << " ";
            std::cout << world.getEntitiesByTag("Player")[0]
                             ->localTransform.position.z
                      << " ";
            std::cout << std::endl;
        }

        if (keyboard.justPressed(GLFW_KEY_ESCAPE)) {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onRestart() override {
        world.clear();
        world.shutdownPhysics();
        // Reinitialize the physics world
        world.initializePhysics();

        
        auto& config = getApp()->getConfig()["scene"];

        // If we have a world in the scene config, we use it to populate our
        // world
        if (config.contains("world")) {
            world.initializePhysics();
            world.deserialize(config["world"]);
        }
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure
        // that the mouse is unlocked
        inputSystem.exit();
        // Clear the world
        world.clear();
        // Stop the music
        if (backgroundMusicSource) {
            soundSystem->stopSource(backgroundMusicSource);
            alDeleteSources(1, &backgroundMusicSource);
        }
        // and we delete all the loaded assets to free memory on the RAM and the
        // VRAM
        our::clearAllAssets();
    }
};

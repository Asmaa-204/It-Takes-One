#include <texture/texture-utils.hpp>
#include <systems/forward-renderer.hpp>
#include <mesh/mesh-utils.hpp>
#include <components/light.hpp>
#include <components/player.hpp>
#include <entities/entity.hpp>


#include <iostream>

namespace our {

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json& config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if(config.contains("sky")){
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));
            
            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram* skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();
            
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.frontFace = GL_CCW;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            
            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D* skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky 
            Sampler* skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if(config.contains("postprocess")){

            glGenFramebuffers(1, &postprocessFrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);

            colorTarget = our::texture_utils::empty(GL_RGBA, this->windowSize, GL_UNSIGNED_BYTE);
            depthTarget = our::texture_utils::empty(GL_DEPTH_COMPONENT, this->windowSize, GL_FLOAT);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // Specify the draw buffers
            GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, drawBuffers); 

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer is not complete!" << std::endl;
                std::exit(1);
            }
        
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler* postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram* postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy(){
        // Delete all objects related to the sky
        if(skyMaterial){
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if(postprocessMaterial){
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::update(World* world, float deltaTime){
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
        }

    
        // If there is no camera, we return (we cannot render without a camera)
        if(camera == nullptr) return;

        glm::vec3 cameraForward = glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0));
        
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
            float firstDepth = glm::dot(first.center, cameraForward);
            float secondDepth = glm::dot(second.center, cameraForward);
            return firstDepth > secondDepth;
        });

        glm::mat4 PV = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        glViewport(0, 0, windowSize.x, windowSize.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        if(postprocessMaterial){
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command            
        for(auto& command : opaqueCommands){
            command.material->setup();
            command.material->shader->set("model", command.localToWorld);
            command.material->shader->set("PV", PV);
            command.material->shader->set("cameraPos", camera->getOwner()->localTransform.position);
            command.mesh->draw();
        }   

        // If there is a sky material, draw the sky
        if(this->skyMaterial){

            skyMaterial->setup();
            glm::vec3 cameraPos = camera->getOwner()->localTransform.position;
            glm::mat4 skyModelMatrix = glm::translate(glm::mat4(1.0f), cameraPos);
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f
            );
            skyMaterial->shader->set("transform", alwaysBehindTransform * PV * skyModelMatrix);
            skySphere->draw();
        }

        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for(auto& command : transparentCommands){
            command.material->setup();
            command.material->shader->set("model", command.localToWorld);
            command.material->shader->set("PV", PV);
            command.material->shader->set("cameraPos", camera->getOwner()->localTransform.position);
            command.mesh->draw();
        }

        // world->getPhysicsWorld()->debugDrawWorld();

        // get the player entity
        Entity* player = world->getEntitiesByTag("Player").front();
        if (!player) return;

        // get the player's player component
        PlayerComponent* playerComponent = player->getComponent<PlayerComponent>();
        bool isShooting = playerComponent->getShooting();
        bool isDamaged = playerComponent->getDamaged();

        // If there is a postprocess material, apply postprocessing
        if(postprocessMaterial){
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
            postprocessMaterial->setup();
            postprocessMaterial->shader->set("flashColor", isShooting ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
            postprocessMaterial->shader->set("flashIntensity", (isShooting || isDamaged) ? 0.6f : 0.0f);
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        playerComponent->setDamaged(false);
    }

}
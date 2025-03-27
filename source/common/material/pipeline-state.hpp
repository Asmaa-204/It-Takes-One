#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our {
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true; // To know how to use it, check glDepthMask


        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const {
            setup_faceCulling();
            setup_depthTesting();
            setup_blending();
            glColorMask(colorMask.x, colorMask.y, colorMask.z, colorMask.w);
            glDepthMask(depthMask);
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json& data);

    private:
        void setup_faceCulling() const {
            if (faceCulling.enabled)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);

            glCullFace(faceCulling.culledFace);
            glFrontFace(faceCulling.frontFace);
        }

        void setup_depthTesting() const {
            if (depthTesting.enabled)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);

            glDepthFunc(depthTesting.function);
        }

        void setup_blending() const {
            if (blending.enabled)
                glEnable(GL_BLEND);
            else
                glDisable(GL_BLEND);

            glBlendEquation(blending.equation);
            glBlendFunc(blending.sourceFactor, blending.destinationFactor);
            glBlendColor(blending.constantColor.x, blending.constantColor.y, 
                    blending.constantColor.z, blending.constantColor.w);
        }
    };

}

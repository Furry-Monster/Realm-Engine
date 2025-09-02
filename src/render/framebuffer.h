#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <map>

namespace RealmEngine
{
    enum class FramebufferType : uint8_t
    {
        DirectionalShadowMap, // 方向光阴影
        PointShadowCubeMap,   // 点光源阴影
        GBuffer,              // GBuffer
        PostProcess_A,        // 后处理A(ping-pong缓冲区)
        PostProcess_B,        // 后处理B
        Default,              // glfw默认
    };

    enum class AttachmentType : uint8_t
    {
        GBuffer_Albedo, // RGB: 反照率, A: 金属度
        GBuffer_Normal, // RGB: 世界空间法线, A: 粗糙度
        GBuffer_Motion, // RG: 运动向量, B: 着色模型ID, A: 未使用
        GBuffer_Depth,  // 深度缓冲
        HDR_Color,      // HDR颜色缓冲,后处理
        LDR_Color,      // LDR颜色缓冲,后处理
        Shadow_Depth,   // 阴影深度
    };

    class FramebufferManager
    {
    public:
        bool initialize(int width, int height);
        void terminate();

        void bindFrameBuffer(FramebufferType type);
        void bindDefaultFrameBuffer();

        void   bindTexture(AttachmentType attachment, int textureUnit);
        GLuint getTexture(AttachmentType attachment);

        void resize(int newWidth, int newHeight);

        void clearFrameBuffer(FramebufferType type, bool color = true, bool depth = true);

    private:
        struct FramebufferData
        {
            GLuint                           fbo {0};
            std::map<AttachmentType, GLuint> attachments;
            int                              width {0};
            int                              height {0};
        };

        std::map<FramebufferType, FramebufferData> m_framebuffers;
        int                                        m_width {0};
        int                                        m_height {0};

        void createGBuffer();
        void createShadowMaps();
        void createPostProcessBuffers();

        GLuint createColorTexture(int width, int height, GLenum internalFormat);
        GLuint createDepthTexture(int width, int height);
        GLuint createCubeMapTexture(int size);
    };
} // namespace RealmEngine
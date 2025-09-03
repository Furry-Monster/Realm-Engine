#include "framebuffer.h"

#include "global.h"

namespace RealmEngine
{
    bool FramebufferManager::initialize(int width, int height)
    {
        resize(width, height);

        LOG_INFO("FramebufferManager initialized");
        return true;
    }

    void FramebufferManager::terminate()
    {
        for (auto& [type, data] : m_framebuffers)
        {
            if (data.fbo != 0)
                glDeleteFramebuffers(1, &data.fbo);

            for (auto& [attachment, texture] : data.attachments)
            {
                if (texture != 0)
                    glDeleteTextures(1, &texture);
            }
        }

        m_framebuffers.clear();
        LOG_INFO("FramebufferManager terminated");
    }

    void FramebufferManager::bindFrameBuffer(FramebufferType type)
    {
        auto it = m_framebuffers.find(type);
        if (it != m_framebuffers.end())
        {
            glBindFramebuffer(GL_FRAMEBUFFER, it->second.fbo);
            glViewport(0, 0, it->second.width, it->second.height);
        }
    }

    void FramebufferManager::bindDefaultFrameBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_width, m_height);
    }

    void FramebufferManager::bindAttachment(AttachmentType attachment, int textureUnit)
    {
        GLuint texture = getAttachment(attachment);
        if (texture != 0)
        {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
    }

    GLuint FramebufferManager::getAttachment(AttachmentType attachment)
    {
        for (const auto& [type, data] : m_framebuffers)
        {
            auto it = data.attachments.find(attachment);
            if (it != data.attachments.end())
                return it->second;
        }
        return 0;
    }

    void FramebufferManager::resize(int width, int height)
    {
        if (m_width == width && m_height == height)
            return;

        // clean all framebuffers
        for (auto& [type, data] : m_framebuffers)
        {
            if (data.fbo != 0)
                glDeleteFramebuffers(1, &data.fbo);

            for (auto& [attachment, texture] : data.attachments)
            {
                if (texture != 0)
                    glDeleteTextures(1, &texture);
            }
        }

        // set new width and height
        m_width  = width;
        m_height = height;

        // re-create framebuffers we need
        createGBuffer();
        createShadowMaps();
        createPostProcessBuffers();

        m_framebuffers.clear();
    }

    void FramebufferManager::clearFrameBuffer(FramebufferType type, bool color, bool depth, bool stencil)
    {
        bindFrameBuffer(type);

        GLbitfield mask = 0;
        if (color)
            mask |= GL_COLOR_BUFFER_BIT;
        if (depth)
            mask |= GL_DEPTH_BUFFER_BIT;
        if (stencil)
            mask |= GL_STENCIL_BUFFER_BIT;

        if (mask != 0)
            glClear(mask);
    }

    void FramebufferManager::createGBuffer()
    {
        FramebufferData g_buffer;
        g_buffer.width  = m_width;
        g_buffer.height = m_height;

        glGenFramebuffers(1, &g_buffer.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.fbo);

        // Albedo + Metallic (RGBA8)
        g_buffer.attachments[AttachmentType::GBuffer_Albedo] = createColorTexture(m_width, m_height, GL_RGBA8);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               g_buffer.attachments[AttachmentType::GBuffer_Albedo],
                               0);

        // Normal + Roughness (RGBA16F)
        g_buffer.attachments[AttachmentType::GBuffer_Normal] = createColorTexture(m_width, m_height, GL_RGBA16F);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D,
                               g_buffer.attachments[AttachmentType::GBuffer_Normal],
                               0);

        // Motion + Shading Model ID (RGBA8)
        g_buffer.attachments[AttachmentType::GBuffer_Motion] = createColorTexture(m_width, m_height, GL_RGBA8);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT2,
                               GL_TEXTURE_2D,
                               g_buffer.attachments[AttachmentType::GBuffer_Motion],
                               0);

        // Depth buffer
        g_buffer.attachments[AttachmentType::GBuffer_Depth] = createDepthTexture(m_width, m_height);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_buffer.attachments[AttachmentType::GBuffer_Depth], 0);

        // Set draw buffers
        GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, draw_buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("GBuffer framebuffer not complete!");
        }

        m_framebuffers[FramebufferType::GBuffer] = g_buffer;
        LOG_INFO("GBuffer created");
    }

    void FramebufferManager::createShadowMaps()
    {
        // Directional shadow map
        FramebufferData shadow_map;
        shadow_map.width  = 2048;
        shadow_map.height = 2048;

        glGenFramebuffers(1, &shadow_map.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map.fbo);

        shadow_map.attachments[AttachmentType::Shadow_Depth] = createDepthTexture(shadow_map.width, shadow_map.height);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               shadow_map.attachments[AttachmentType::Shadow_Depth],
                               0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Shadow map framebuffer not complete!");
        }

        m_framebuffers[FramebufferType::DirectionalShadowMap] = shadow_map;
        LOG_INFO("Shadow maps created");
    }

    void FramebufferManager::createPostProcessBuffers()
    {
        // Post process buffer A
        FramebufferData post_process_a;
        post_process_a.width  = m_width;
        post_process_a.height = m_height;

        glGenFramebuffers(1, &post_process_a.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, post_process_a.fbo);

        post_process_a.attachments[AttachmentType::HDR_Color] = createColorTexture(m_width, m_height, GL_RGBA16F);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               post_process_a.attachments[AttachmentType::HDR_Color],
                               0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Post process A framebuffer not complete!");
        }

        m_framebuffers[FramebufferType::PostProcess_A] = post_process_a;

        // Post process buffer B (similar structure)
        FramebufferData post_process_b = post_process_a;
        glGenFramebuffers(1, &post_process_b.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, post_process_b.fbo);

        post_process_b.attachments[AttachmentType::LDR_Color] = createColorTexture(m_width, m_height, GL_RGBA8);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               post_process_b.attachments[AttachmentType::LDR_Color],
                               0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Post process B framebuffer not complete!");
        }

        m_framebuffers[FramebufferType::PostProcess_B] = post_process_b;
        LOG_INFO("Post process buffers created");
    }

    GLuint FramebufferManager::createColorTexture(int width, int height, GLenum internalFormat)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLenum format = (internalFormat == GL_RGBA8 || internalFormat == GL_RGBA16F) ? GL_RGBA : GL_RGB;
        GLenum type   = (internalFormat == GL_RGBA16F) ? GL_FLOAT : GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        return texture;
    }

    GLuint FramebufferManager::createDepthTexture(int width, int height)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

        return texture;
    }

    GLuint FramebufferManager::createCubeMapTexture(int size)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        for (int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_DEPTH_COMPONENT24,
                         size,
                         size,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_FLOAT,
                         nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture;
    }
} // namespace RealmEngine
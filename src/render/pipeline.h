#pragma once

#include <cstdint>

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

    class Pipeline
    {
    public:
        virtual ~Pipeline() = default;

        virtual void initialize() = 0;
        virtual void render()     = 0;
        virtual void terminate()  = 0;
    };

    class ForwardPipeline : public Pipeline
    {
    public:
        void render() override
        {
            renderShadowMaps();
            renderForward();
            renderPostProcess();
            renderUI();
        }

    protected:
        void renderShadowMaps();
        void renderForward();
        void renderPostProcess();
        void renderUI();
    };

    class DefferdPipeline : public Pipeline
    {
    public:
        void render() override
        {
            renderShadowMaps();
            renderGBuffer();
            renderLighting();
            renderForwardObjects();
            renderPostProcess();
            renderUI();
        }

    protected:
        void renderShadowMaps();
        void renderGBuffer();
        void renderLighting();
        void renderForwardObjects(); // 透明物体
        void renderPostProcess();
        void renderUI();
    };
} // namespace RealmEngine
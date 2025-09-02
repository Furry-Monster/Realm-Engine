#pragma once

namespace RealmEngine
{
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
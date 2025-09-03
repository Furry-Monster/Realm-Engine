#include "engine.h"
#include "global.h"

namespace RealmEngine
{
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void Engine::boot()
    {
        g_context.create();
        g_context.m_window->setVisible(true);
        LOG_INFO("Boot Engine ...");
    }

    void Engine::run()
    {
        // initialize camera
        m_camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
        g_context.m_input->setCamera(m_camera);
        // load model
        m_model = new Model("../assets/model/backpack/backpack.obj");
        // load shader
        m_shader = new Shader("../shader/default_raster.vert", "../shader/default_raster.frag");

        while (!g_context.m_window->shouldClose())
        {
            // update timing
            float current_frame = glfwGetTime();
            m_delta_time        = current_frame - m_last_frame;
            m_last_frame        = current_frame;

            tick();
        }

        // clean assets
        delete m_shader;
        delete m_model;
        delete m_camera;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void Engine::terminate()
    {
        LOG_INFO("Terminate Engine ...");
        g_context.destroy();
    }

    void Engine::tick()
    {
        logicalTick();
        renderTick();
    }

    void Engine::logicalTick() const
    {
        // event handling
        g_context.m_window->tick();
        g_context.m_input->tick(m_delta_time);
    }

    void Engine::renderTick()
    {
        // update imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render scene
        glm::mat4 projection =
            m_camera->getProjectionMatrix(static_cast<float>(g_context.m_window->getFramebufferWidth()) /
                                          static_cast<float>(g_context.m_window->getFramebufferHeight()));
        glm::mat4 view       = m_camera->getViewMatrix();
        glm::vec3 camera_pos = m_camera->getPosition();

        g_context.m_renderer->setMainCamera(view, projection, camera_pos);

        g_context.m_renderer->addDirectionalLight(glm::vec3(0.2f, -1.0f, 0.3f), // 方向
                                                  glm::vec3(1.0f, 0.9f, 0.8f),  // 颜色
                                                  2.0f                          // 强度
        );

        g_context.m_renderer->addPointLight(glm::vec3(2.0f, 3.0f, 1.0f), // 位置
                                            glm::vec3(1.0f, 0.5f, 0.2f), // 颜色
                                            5.0f                         // 强度
        );

        glm::mat4 model_matrix = glm::mat4(1.0f);
        g_context.m_renderer->addRenderObject(m_model, model_matrix);

        g_context.m_renderer->renderFrame();

        // debug ui
        drawDebugUI();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // buffering...
        g_context.m_window->swapBuffers();
    }

    void Engine::drawDebugUI()
    {
        // general info
        ImGui::Begin("Debug Info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Text("Window size: %d x %d", g_context.m_window->getWidth(), g_context.m_window->getHeight());
        ImGui::Text("Framebuffer size: %d x %d",
                    g_context.m_window->getFramebufferWidth(),
                    g_context.m_window->getFramebufferHeight());
        ImGui::Text("OpenGL Version: %s", glfwGetVersionString());
        ImGui::End();
    }
} // namespace RealmEngine
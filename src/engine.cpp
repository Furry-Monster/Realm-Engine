#include "engine.h"
#include "engine_context.h"

static struct RasterState
{
    GLenum polygon_mode {0};
    bool   enable_depth_test {false};
    bool   enable_culling {false};
    GLenum cull_face {0};
    bool   enable_wireframe {false};
    float  line_width {0};
    float  point_size {0};
} g_raster_state {GL_FILL, true, false, GL_BACK, false, 1.0f, 1.0f};

static void applyRasterizationState()
{
    // depth test
    if (g_raster_state.enable_depth_test)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    // culling
    if (g_raster_state.enable_culling)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(g_raster_state.cull_face);
        glFrontFace(GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    // draw mode
    glPolygonMode(GL_FRONT_AND_BACK, g_raster_state.polygon_mode);
    glLineWidth(g_raster_state.line_width);
    glPointSize(g_raster_state.point_size);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Engine::boot()
{
    // create context
    g_context.create();

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(g_context.m_window->getGLFWwindow(), GLFW_TRUE);
    ImGui_ImplOpenGL3_Init();

    LOG_INFO("Boot Engine ...");
}

void Engine::run()
{
    // TODO: 分离render部分初始化

    // initialize camera system
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
    g_context.m_input->setCamera(m_camera);
    // load model
    m_model = new Model("../assets/model/backpack/backpack.obj");
    // load shader
    m_shader = new Shader("../shader/test.vert", "../shader/test.frag");
    // enable depth testing
    glEnable(GL_DEPTH_TEST);

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

    // clean imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // terminate window
    if (g_context.m_window)
    {
        g_context.m_window.reset();
        g_context.m_window = nullptr;
    }

    // destroy context
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
    g_context.m_window->pollEvents();
    g_context.m_input->setDeltaTime(m_delta_time);
    g_context.m_input->processKeyboard(g_context.m_window->getGLFWwindow());
}

void Engine::renderTick()
{
    // update imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawDebugUI();

    // background
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // apply rasterization state
    applyRasterizationState();

    m_shader->use();

    // set matrices
    glm::mat4 projection =
        m_camera->getProjectionMatrix(static_cast<float>(g_context.m_window->getFramebufferWidth()) /
                                      static_cast<float>(g_context.m_window->getFramebufferHeight()));
    glm::mat4 view  = m_camera->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model           = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);
    m_shader->setMat4("model", model);

    // set lighting uniforms
    glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
    glm::vec3 light_color(1.0f, 1.0f, 1.0f);
    m_shader->setVec3("lightPos", light_pos);
    m_shader->setVec3("lightColor", light_color);
    m_shader->setVec3("viewPos", m_camera->m_position);

    // render model or fallback triangle
    if (m_model)
    {
        m_model->draw(m_shader->id);
    }

    // ui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // buffering...
    g_context.m_window->swapBuffers();
}

void Engine::drawDebugUI()
{
    ImGui::Begin("Debug Info");
    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Window size: %d x %d", g_context.m_window->getWidth(), g_context.m_window->getHeight());
    ImGui::Text("Framebuffer size: %d x %d",
                g_context.m_window->getFramebufferWidth(),
                g_context.m_window->getFramebufferHeight());
    ImGui::Text("OpenGL Version: %s", glfwGetVersionString());

    ImGui::Separator();
    ImGui::Text("Rasterization Controls:");

    const char* polygon_modes[] = {"Fill", "Line", "Point"};
    int current_mode = (g_raster_state.polygon_mode == GL_FILL) ? 0 : (g_raster_state.polygon_mode == GL_LINE) ? 1 : 2;
    if (ImGui::Combo("Polygon Mode", &current_mode, polygon_modes, 3))
    {
        g_raster_state.polygon_mode = (current_mode == 0) ? GL_FILL : (current_mode == 1) ? GL_LINE : GL_POINT;
    }

    ImGui::Checkbox("Enable Depth Test", &g_raster_state.enable_depth_test);
    ImGui::Checkbox("Enable Face Culling", &g_raster_state.enable_culling);

    if (g_raster_state.enable_culling)
    {
        const char* cull_modes[] = {"Back", "Front", "Front and Back"};
        int current_cull = (g_raster_state.cull_face == GL_BACK) ? 0 : (g_raster_state.cull_face == GL_FRONT) ? 1 : 2;
        if (ImGui::Combo("Cull Face", &current_cull, cull_modes, 3))
        {
            g_raster_state.cull_face = (current_cull == 0) ? GL_BACK :
                                       (current_cull == 1) ? GL_FRONT :
                                                             GL_FRONT_AND_BACK;
        }
    }

    ImGui::SliderFloat("Line Width", &g_raster_state.line_width, 0.1f, 10.0f);
    ImGui::SliderFloat("Point Size", &g_raster_state.point_size, 1.0f, 20.0f);

    ImGui::Separator();
    ImGui::Text("Camera Controls:");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_camera->m_position.x, m_camera->m_position.y, m_camera->m_position.z);
    ImGui::Text("Yaw: %.2f, Pitch: %.2f", m_camera->m_yaw, m_camera->m_pitch);
    ImGui::SliderFloat("FOV", &m_camera->m_zoom, 1.0f, 45.0f);
    ImGui::SliderFloat("Speed", &m_camera->m_movement_speed, 0.1f, 10.0f);
    ImGui::Text("Mouse: %s (Tab to toggle)", g_context.m_input->isMouseCaptured() ? "Captured" : "Free");
    ImGui::Text("Controls: WASD to move, Mouse to look, Scroll to zoom");

    static char model_path[256] = "../assets/model/";
    ImGui::InputText("Model Path", model_path, sizeof(model_path));
    if (ImGui::Button("Load Model"))
    {
        delete m_model;

        try
        {
            m_model = new Model(model_path);
        }
        catch (...)
        {
            m_model = nullptr;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Model"))
    {
        if (m_model)
        {
            delete m_model;
            m_model = nullptr;
        }
    }

    ImGui::Separator();
    ImGui::Text("Model Info:");
    if (m_model)
    {
        ImGui::Text("Meshes: %zu", m_model->m_meshes.size());
        ImGui::Text("Textures: %zu", m_model->m_textures.size());
    }
    else
    {
        ImGui::Text("No model loaded");
    }
    ImGui::End();
}

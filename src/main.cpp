#include "Application.h"
#include "WindowEventPublisher.h"
#include "helpers.h"

int main()
{
    auto app = Application();

    app.window = createGlfwWindow(SCR_WIDTH, SCR_HEIGHT, "CPSoS", false);
    if(app.window == NULL) return -1;
    glfwSetKeyCallback(app.window, [](GLFWwindow* window, int key, int scancode, int action, int mods){ WindowEventPublisher::keyboardCallback(window, key,scancode,action,mods);});
    glfwSetCursorPosCallback(app.window, [](GLFWwindow* window, double xpos, double ypos){ WindowEventPublisher::mouseCallback(window, xpos, ypos);});
    glfwSetScrollCallback(app.window, [](GLFWwindow* window, double xoffset, double yoffset){ WindowEventPublisher::scrollCallback(window, xoffset, yoffset);});
    glfwSetFramebufferSizeCallback(app.window, [](GLFWwindow* window, int width, int height){glViewport(0, 0, width, height);});

    glfwMakeContextCurrent(app.window);

    //glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context /////////////////////
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    app.Initialization();
    app.setUpWindowEventHandlers();

    return app.AppMain();
}
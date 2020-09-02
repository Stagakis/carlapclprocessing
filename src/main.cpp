#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <CarlaImuParser.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ShaderLoader.h>
#include <Camera.h>
#include "PointCloud.h"
#include "Window.h"

#include <iostream>

glm::mat4 Carla_to_Opengl_coordinates = glm::mat4(1.0f,0.0f,0.0f,0.0f,
                                                0.0f,0.0f,1.0f,0.0f,
                                                0.0f,-1.0f,0.0f,0.0f,
                                                0.0f,0.0f,0.0f,1.0f);

glm::mat4 imu_carla_to_opengl_coords = glm::mat4(0.0f,1.0f,0.0f,0.0f,
                                                 0.0f,0.0f,1.0f,0.0f,
                                                 -1.0f,0.0f,0.0f,0.0f,
                                                 0.0f,0.0f,0.0f,1.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void assign_key_callbacks();
void processInput();
std::vector<std::string> glob(const std::string& pattern);
// settings
const unsigned int SCR_WIDTH =  1024; // 1920;//
const unsigned int SCR_HEIGHT = 768; // 1080;//
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraIsActive = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int pcl_index = 1;
int pcl_list_length = 0;
glm::mat4 world_to_lidar(1.0f);
glm::vec3 velocity(0.0f);
CarlaImuParser imu_data;
int main()
{

    Window::CreateNewWindow(SCR_WIDTH, SCR_HEIGHT, "MY_WINDOW_CLASS");
    glfwSetCursorPosCallback(Window::window, mouse_callback);
    glfwSetScrollCallback(Window::window, scroll_callback);
    assign_key_callbacks();


    {
        glm::mat2 test = {{2.0f, 3.0f},
                          {4.0f, 5.0f}};
        glm::vec2 test_vec = {1, 0};
        std::cout << "GLM translation Matrix" <<
                  glm::to_string(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f))) << std::endl;
        std::cout << "Test_vec" << glm::to_string(test_vec) << std::endl;
        std::cout << "Test" << glm::to_string(test) << std::endl;
        std::cout << "Test*Test_vec" << glm::to_string(test * test_vec) << std::endl;
        std::cout << "Test_vec*Test" << glm::to_string(test_vec * test) << std::endl;
        std::cout << "How Test is laid out in memory: " << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << *((float *) (&test[0][0]) + i) << " ";
        }
        std::cout << std::endl;

    }
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    ShaderLoader ourShader("vertexShader.shader", "fragmentShader.shader");

    auto filenames = glob("../resources/*.ply");
    std::vector<PointCloud> pointcloud_list;
    for(const auto& file : filenames){
        pointcloud_list.emplace_back(file);
    }
    pcl_list_length = pointcloud_list.size();

    PointCloud pcl("../resources/001681.ply");
    imu_data = CarlaImuParser("../resources/imu.txt");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(Window::window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();

    // render loop
    // -----------
    bool show_demo_window = true;
    bool show_another_window = false;
    float hole_radius = 2.0f, hole_depth = 1.5f;
    glm::vec3 hole_center = glm::vec3(0.0f, -2.4f, -18.0f);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    while (!glfwWindowShouldClose(Window::window))
    {


        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput();

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //IMGUI render
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);


        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("Radius", &hole_radius, 3.0f, 10.0f);
            ImGui::SliderFloat("Depth", &hole_depth, 3.0f, 10.0f);

            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("FrameNumber = %d", pcl_index);
            ImGui::Text("CameraPos: %f %f %f ", camera.Position[0], camera.Position[1], camera.Position[2]);
            ImGui::Text("Velocity:  %f %f %f ", velocity[0], velocity[1], velocity[2]);
            ImGui::Text("Accel:  %f %f %f    ",  (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pcl_index+1], 1.0f))[0],
                        (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pcl_index + 1 ], 1.0f))[1],
                        (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pcl_index + 1], 1.0f))[2]);

            ImGui::Text("CameraView: %f %f %f ", view[3][0], view[3][1], view[3][2]);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ourShader.setMat4("model", world_to_lidar*Carla_to_Opengl_coordinates);

        pointcloud_list[pcl_index].draw();

        /*// //OUTPUT FILE WRITTING FOR PYTHON VISUALIZATION
        for(int k = 1; k<pcl_list_length ; k++) {
            pcl_index = k;
            glm::vec4 accel_carla = glm::vec4(imu_data.accel[pcl_index].x, imu_data.accel[pcl_index].y, 0.0f,1.0f);//(8.108274, 0.061310, 0.0, 1.0f);

            glm::vec3 accel_opengl = glm::transpose(imu_carla_to_opengl_coords) * accel_carla  ;

            float dt = imu_data.timestamp[pcl_index] - imu_data.timestamp[pcl_index-1];
            velocity += accel_opengl*dt;

            camera.Position += velocity + 0.5f*accel_opengl*(dt*dt);
            world_to_lidar = glm::translate(world_to_lidar, velocity + 0.5f*accel_opengl*(dt*dt));
            std::ofstream myfile;
            myfile.open(filenames[k]+".txt");
            myfile.write((glm::to_string(camera.Position) + "\n").c_str(), (glm::to_string(camera.Position) + "\n").length());

            std::cout << filenames[k] << "Has: " << pointcloud_list[pcl_index].points.size() << " points"<< std::endl;

            for (int i = 0; i < pointcloud_list[pcl_index].points.size(); i++) {
                //std::cout << "===" << std::endl;
                //if(i!=pcl_index) continue;
                glm::vec3 aPos(pointcloud_list[pcl_index].points[i].x, pointcloud_list[pcl_index].points[i].y,
                               pointcloud_list[pcl_index].points[i].z);
                glm::vec4 world_pos = world_to_lidar * Carla_to_Opengl_coordinates * glm::vec4(aPos, 1.0f);


                if (glm::distance(glm::vec3(world_pos), hole_center) <= hole_radius) {
                    //std::cout << glm::to_string(world_pos) << std::endl;
                    myfile.write((glm::to_string(world_pos) + "\n").c_str(), (glm::to_string(world_pos) + "\n").length());

                    world_pos.y -=
                            hole_depth - (glm::distance(glm::vec3(world_pos), hole_center) / hole_radius) * hole_depth;


                    glm::vec3 camera_to_point_ray = glm::vec3(world_pos) - camera.Position;
                    float modifier = (-2.4 - world_pos.y) / camera_to_point_ray.y;
                    //world_pos.y + x*camera_to_point.y = -2.4 => x = (-2.4 -wolrd_pos.y)/camera_to_point.y;
                    glm::vec3 intersection_point = glm::vec3(world_pos) + camera_to_point_ray * modifier;
                    if (distance(intersection_point, hole_center) > hole_radius) continue;

                }
            }
            myfile.close();
        }
        return 0;//*/

        ourShader.setFloat("hole_radius", hole_radius);
        ourShader.setFloat("hole_depth", hole_depth);
        ourShader.setVec3("hole_center", hole_center);
        ourShader.setVec3("cameraPos", camera.Position);

        Window::FinishFrame();
    }

    glfwTerminate();
    return 0;
}

void assign_key_callbacks(){

    Window::AddKeyCallback(GLFW_KEY_GRAVE_ACCENT, GLFW_PRESS, [](){
        if(glfwGetInputMode(Window::window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cameraIsActive = true;
        }
        else {
            glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cameraIsActive = false;
        }
    });

    Window::AddKeyCallback(GLFW_KEY_ESCAPE, GLFW_PRESS, [](){glfwSetWindowShouldClose(Window::window, true);});

    Window::AddKeyCallback(GLFW_KEY_O, GLFW_PRESS, [](){pcl_index = std::max(0, pcl_index - 1);});

    Window::AddKeyCallback(GLFW_KEY_P, GLFW_PRESS, [](){
        pcl_index= std::min(pcl_list_length, pcl_index + 1);
        glm::vec4 accel_carla = glm::vec4(imu_data.accel[pcl_index].x, imu_data.accel[pcl_index].y, 0.0f,1.0f);//(8.108274, 0.061310, 0.0, 1.0f);

        glm::vec3 accel_opengl = glm::transpose(imu_carla_to_opengl_coords) * accel_carla  ;

        float dt = imu_data.timestamp[pcl_index] - imu_data.timestamp[pcl_index-1];
        velocity += accel_opengl*dt;

        camera.Position += velocity + 0.5f*accel_opengl*(dt*dt);
        world_to_lidar = glm::translate(world_to_lidar, velocity + 0.5f*accel_opengl*(dt*dt));

    });

    Window::AddKeyCallback(GLFW_KEY_GRAVE_ACCENT, GLFW_PRESS, [](){
        if(glfwGetInputMode(Window::window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cameraIsActive = true;
        }
        else {
            glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cameraIsActive = false;
        }
    });

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput()
{

    if(cameraIsActive) {
        if (Window::CheckKey(GLFW_KEY_W, GLFW_PRESS))
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (Window::CheckKey(GLFW_KEY_A, GLFW_PRESS))
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (Window::CheckKey(GLFW_KEY_S, GLFW_PRESS))
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (Window::CheckKey(GLFW_KEY_D, GLFW_PRESS))
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }


}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(cameraIsActive) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


std::vector<std::string> glob(const std::string& pattern) {
    using namespace std;

    // glob struct resides on the stack
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    // do the glob operation
    int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value != 0) {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        throw std::runtime_error(ss.str());
    }

    // collect all the filenames into a std::list<std::string>
    vector<string> filenames;
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }

    // cleanup
    globfree(&glob_result);

    // done
    return filenames;
}

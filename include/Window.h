#ifndef MYOPENGL_WINDOW_H
#define MYOPENGL_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>


struct key_comb {
    int key;
    int mod;
    key_comb(int _key, int _mod){ key = _key; mod = _mod;};
    inline bool operator<(key_comb other) const{
        return key + mod <other.key + other.mod;
    }
};
typedef void (*callback_function)();

class Window {
public:
    static int CreateNewWindow(int width, int height, const std::string& windowName, bool fullscreen = false);

    static void add_key_callback(int key, callback_function func, int mods = 0);
    static void swap_buffers();

private:

    //static std::vector<callback_function> key_callbacks;
    static std::map<key_comb, callback_function> key_callbacks;
    static GLFWwindow* window;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

};


#endif //MYOPENGL_WINDOW_H

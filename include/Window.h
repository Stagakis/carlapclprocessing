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
    int action;
    std::string id;
    key_comb(int _key, int _action, int _mod){
        key = _key; mod = _mod; action = _action;
        id = std::to_string(key) + std::to_string(mod) +std::to_string(action);
    };
    inline bool operator<(const key_comb& other) const{
        return id < other.id;
    }
};

typedef void (*callback_function)();

class Window {
public:

    static GLFWwindow* window;
    static int CreateNewWindow(int width, int height, const std::string& windowName, bool fullscreen = false);
    static void AddKeyCallback(int key, int action, callback_function func, int mods = 0);
    static bool CheckKey(int key, int state);
    static void FinishFrame();

private:

    static std::map<key_comb, callback_function> key_callbacks;
    static void AllKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};


#endif //MYOPENGL_WINDOW_H

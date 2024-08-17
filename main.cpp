#include <iostream>
#include <GLFW/glfw3.h>

int main(){
    std::cout << "hey, Zues\n";

    GLFWwindow *window;

    if (!glfwInit()){
        fprintf(stderr, "failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(300,300, "Gears", NULL, NULL);
    if(!window){
        fprintf(stderr, "failed to open GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    while (!glfwWindowShouldClose(window)){
        // draw();

        // animate();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();

}
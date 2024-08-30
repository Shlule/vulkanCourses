#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer(){

};

VulkanRenderer::~VulkanRenderer(){
    
};

int VulkanRenderer::init(GLFWwindow* windowP)
{

    window = windowP;
    try
    {

        createInstance();

    }
    catch (const std::runtime_error& e)
    {

        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;

    }

    return 0;

}
void VulkanRenderer::createInstance()
{

}
#include "gl_framework.hpp"

extern GLfloat xrot,yrot,zrot,c_xrot,c_yrot,c_zrot;
extern bool enable_perspective;
extern int key_pressed;
namespace csX75
{
  //! Initialize GL State
  void initGL(void)
  {
    //Set framebuffer clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Set depth buffer furthest depth
    glClearDepth(1.0);
    //Set depth test to less-than
    glDepthFunc(GL_LESS);
    //Enable depth testing
    glEnable(GL_DEPTH_TEST);
  }
  
  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }
  
  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
  }
  
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
      yrot -= 5.0;
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
      yrot += 5.0;
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
      xrot -= 5.0;
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
      xrot += 5.0;
    else if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
      zrot -= 5.0;
    else if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
      zrot += 5.0;
    else if (key == GLFW_KEY_A  && action == GLFW_PRESS)
      c_yrot -= 5.0;
    else if (key == GLFW_KEY_D  && action == GLFW_PRESS)
      c_yrot += 5.0;
    else if (key == GLFW_KEY_W  && action == GLFW_PRESS)
      c_xrot -= 5.0;
    else if (key == GLFW_KEY_S  && action == GLFW_PRESS)
      c_xrot += 5.0;        
    else if (key == GLFW_KEY_Q  && action == GLFW_PRESS)
      c_zrot -= 5.0;
    else if (key == GLFW_KEY_E  && action == GLFW_PRESS)
      c_zrot += 5.0;
    else if (key == GLFW_KEY_R  && action == GLFW_PRESS)
    {
      c_yrot = 0.0;
      c_zrot = 0.0;
      c_xrot = 0.0;
    }
    else if (key == GLFW_KEY_1  && action == GLFW_PRESS)
      key_pressed = 1;
    else if (key == GLFW_KEY_2  && action == GLFW_PRESS)
      key_pressed = 2;
    else if (key == GLFW_KEY_3  && action == GLFW_PRESS)
      key_pressed = 3;
    else if (key == GLFW_KEY_4  && action == GLFW_PRESS)
      key_pressed = 4;
    else if (key == GLFW_KEY_5  && action == GLFW_PRESS)
      key_pressed = 5;
    else if (key == GLFW_KEY_6  && action == GLFW_PRESS)
      key_pressed = 6;
    else if (key == GLFW_KEY_7  && action == GLFW_PRESS)
      key_pressed = 7;
    else if (key == GLFW_KEY_0  && action == GLFW_PRESS)
      key_pressed = 0;
  }
};  
  



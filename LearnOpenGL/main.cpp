#include <math.h>
#include <iostream>

/* 
 * Here, we choose to use the static version of the GLEW library.
 * For more information on static vs. dynamic linkage, refer to the
 * end of the section titled "Creating a Window" at learnopengl.com.
 * Also, note that the order here matters: we need to include GLEW
 * before GLFW. The include file for GLEW contains the correct
 * OpenGL header includes (like GL/gl.h), so including GLEW before other
 * header files that require OpenGL does the trick. 
 */
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW and OpenGL
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom headers
#include "GlslProgram.h"
#include "Image.h"
#include "Camera.h"

GLFWwindow *window;
const GLuint WINDOW_WIDTH = 800;
const GLuint WINDOW_HEIGHT = 600;
bool keys[1024];
GLfloat deltaTime = 0.0f;                                           // Time between current frame and last frame
GLfloat lastFrame = 0.0f;                                           // Time of last frame
GLfloat lastX = WINDOW_WIDTH / 2;
GLfloat lastY = WINDOW_HEIGHT / 2;
bool firstMouse = true;
float uCross = 0.2f;

Camera cam;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)                                                  // This ensures that the first time we move the mouse, we don't see a huge jump
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;                                 // Reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;
    
    cam.processMouse(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    /*
     * The trick is to only keep track of what keys are pressed/released in the callback function. 
     * In the game loop we then read these values to check what keys are active and react accordingly.
     * So we're basically storing state information about what keys are pressed/released and react 
     * upon that state in the game loop.
     */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        /*
         * When a user presses the escape key, we set the WindowShouldClose property to true,
         * closing the application.
         */
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.processScroll(yoffset);
}

void calculateCameraMovement()
{
    if (keys[GLFW_KEY_LEFT])
        uCross -= 0.1f;
        if (uCross < 0.0f) uCross = 0.0f;
    if (keys[GLFW_KEY_RIGHT])
        uCross += 0.1f;
        if (uCross > 1.0f) uCross = 1.0f;
    if (keys[GLFW_KEY_W])
        cam.processKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        cam.processKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        cam.processKeyboard(LEFT, deltaTime);
     if (keys[GLFW_KEY_D])
        cam.processKeyboard(RIGHT, deltaTime);
}

int main(int argc, const char * argv[])
{
    glfwInit();                                                     // Instantiate GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // Tell GLFW that we want to use version 3.3 of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Use the core profile
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);                       // Don't let the user resize the window
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, key_callback);                       // Register our callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = GL_TRUE;                                     // We want to use more modern techniques for managing OpenGL
    if (glewInit() != GLEW_OK)                                      // Initialize GLEW, which manages function pointers for OpenGL

    {
        std::cout << "Failed to initialize GLEW." << std::endl;
        return -1;
    }
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);                  // Tell OpenGL the size of the rendering window
    glEnable(GL_DEPTH_TEST);
    
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    /*
     * A vertex array object (VAO) can be bound just like a vertex buffer object and any subsequent
     * vertex attribute calls from that point on will be stored inside the VAO. This has the advantage
     * that when configuring vertex attribute pointers you only have to make those calls once and
     * whenever we want to draw the object, we can just bind the corresponding VAO. This makes switching
     * between different vertex data and attribute configurations as easy as binding a different VAO.
     * All the state we just set is stored inside the VAO.
     * In particular, a VAO stores:
     * 1) Calls to glEnableVertexAttribArray or glDisableVertexAttribArray
     * 2) Vertex attribute configurations via glVertexAttribPointer
     * 3) Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer
     */
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                             // Any future buffer calls we make will be used to configure VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    /*
     * The parameters of glVertexAttribPointer are as follows:
     * 1) The vertex attribute that we want to configure (corresponds to the layout qualifier in the shader)
     * 2) The size of the vertex attribute
     * 3) The type of the data
     * 4) Do we want the data normalized?
     * 5) The stride (we could just use 0 since our values are tightly packed)
     * 6) The offset
     * The data is pulled from the buffer that is currently bound to the GL_ARRAY_BUFFER targer (i.e. VBO)
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);                               // Unbind the VBO (good practice); note that we must leave the EBO bound
    glBindVertexArray(0);
    
    GlslProgram program;
    program.setupProgramFromFile("shaders/shader.vert", "shaders/shader.frag");

    Image tex0;
    tex0.loadImage("assets/container.jpg", 512, 512);
    
    Image tex1;
    tex1.loadImage("assets/awesomeface.png", 512, 512);
    
    /*
     * Everything that follows is our "game" or "rendering" loop. This will keep executing
     * until GLFW has been instructed to close. The glfwPollEvents function checks if any
     * events are triggered and calls the corresponding functions. The glfwSwapBuffers function
     * does exactly what it says. This technique is known as "double buffering." The front buffer 
     * contains the final output image that is shown at the screen, while all the rendering 
     * commands draw to the back buffer. As soon as all the rendering commands are finished we swap 
     * the back buffer to the front buffer so the image is instantly displayed to the user, removing 
     * any would-be drawing artifacts.
     */
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        /* 
         * Currently we used a constant value for movement speed when walking around. 
         * In theory this seems fine, but in practice people have different processing 
         * powers and the result of that is that some people are able to draw much more 
         * frames than others each second. Whenever a user draws more frames than another 
         * user he also calls calculateCameraMovement() more often. The result is that some 
         * people move really fast and some really slow depending on their setup. When 
         * shipping your application you want to make sure it runs the same on all kinds 
         * of hardware.
         */
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        calculateCameraMovement();
        
        // ===============================
        // Rendering starts here
        // ===============================
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);                       // A state-setting function that sets the clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                               // A state-using function that clears the active buffer
        program.begin();
        
        /* The default texture unit for a texture is 0, which is the default active texture unit so we
         * did not need to assign a location to this texture before binding it. If, however, we want to bind
         * multiple textures simultaneously, we will need to manually assign texture units.
         * To use the second texture (and the first texture) we have to change the rendering procedure 
         * a bit by binding both textures to the corresponding texture unit and specifying which uniform 
         * sampler corresponds to which texture unit.
         */
        glActiveTexture(GL_TEXTURE0);                               // Activate the texture unit first before binding texture
        tex0.bind();
        program.setUniformSampler2D("tex0", 0);
        
        glActiveTexture(GL_TEXTURE1);
        tex1.bind();
        program.setUniformSampler2D("tex1", 1);
        
        /*
         * The parameters of glm::perspective are as follows
         * 1) The FOV (in radians)
         * 2) The aspect ratio
         * 3 / 4) The near and far clipping planes
         */
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        
        program.setUniform1f("uCross", uCross);
        
        glBindVertexArray(VAO);
        for(GLuint i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glm::mat4 uModelViewProjection = projection * cam.getViewMatrix() * model;
            
            program.setUniform4x4Matrix("uModelViewProjection", uModelViewProjection);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        tex0.unbind();
        tex1.unbind();
        
        program.end();
        
        // ===============================
        // Rendering ends here
        // ===============================
        
        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();                                                // Clear any resources we've used
    std::cout << "Terminating the application." << std::endl;
    return 0;
}
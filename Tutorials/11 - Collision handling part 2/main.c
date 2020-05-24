
#ifndef __APPLE__
    #include "../../Libraries/glad/glad.h"
#endif

#include <stdio.h>
#include <math.h>

#include "../../Libraries/GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../Libraries/stb/stb_image.h"
#include "headers/shader.h"

#define FALSE 0
#define TRUE 1
#define FULLSCREEN FALSE

GLuint screenWidth = 600, screenHeight = 600;
const GLFWvidmode* mode;
GLFWwindow* window;

void setupVAO();
GLuint getTextureHandle(char* path);
unsigned int VAO;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int keys[1024];

float currentTime, deltaTime, lastFrame;

typedef struct Color {
    float r, g, b, a;
} Color;

typedef struct Rectangle {
    float x, y;
    float width;
    float height;
    Color color;
} Rectangle;

int check_collision(Rectangle r1, Rectangle r2);
GLuint quad_shader;
void draw_rect(Rectangle r1);

int main (){

    // Window setup
    GLint glfwStatus = glfwInit();

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_SAMPLES, 4);

    if (FULLSCREEN)
    {
        mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, "My Title", glfwGetPrimaryMonitor(), NULL);
    }
    else
    {
        window = glfwCreateWindow(screenWidth, screenHeight, "My Title", NULL, NULL);
    }

    if (window == NULL)
    {
        printf("Window failed to create");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
        // If Windows or Linux: load all OpenGL function pointers with GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            printf("Failed to initialize GLAD");
            return -1;
        }
    #endif

    // END Window setup

    glfwSetKeyCallback(window, key_callback);

    // set up Vertex Array Object that contains our vertices and bind it
    setupVAO();
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to unbind in the setupVertexArray function and then bind here, but we'll do so for clarity, organization, and avoiding possible bugs in future
    
    quad_shader = glCreateProgram();
    buildShaders(quad_shader, "shaders/generic.vs", "shaders/quad.fs");
    glUseProgram(quad_shader);

    GLuint channel_logo = getTextureHandle("assets/logo.png");
    glBindTexture(GL_TEXTURE_2D, channel_logo);

    // for alpha (opacity)
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int window_width, window_height;

    Rectangle r1, r2, r3, r4;
    r1.x = -1.0; r1.y = -1.0; r1.width = 0.1; r1.height = 0.1; r1.color.r = 0.7; r1.color.g = 0.2; r1.color.b = 1.0; r1.color.a = 0.9;
    r2.x = 0.0; r2.y = 0.0; r2.width = 0.1; r2.height = 0.1; r2.color.r = 0.3; r2.color.g = 0.2; r2.color.b = 1.0; r2.color.a = 0.9;
    r3.x = -0.4; r3.y = -0.4; r3.width = 0.1; r3.height = 0.1; r3.color.r = 0.3; r3.color.g = 0.2; r3.color.b = 0.8; r3.color.a = 0.9;
    r4.x = -0.1; r4.y = 0.6; r4.width = 0.1; r4.height = 0.1; r4.color.r = 0.3; r4.color.g = 0.2; r4.color.b = 0.2; r4.color.a = 0.9;

    while (!glfwWindowShouldClose(window))
    {

        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);
        // printf("%d\n", window_width);
        
        float previous_x = r1.x;
        float previous_y = r1.y;

        if(keys[GLFW_KEY_RIGHT]) r1.x = r1.x + 0.01;

        if(check_collision(r1, r2)){
            r1.x = previous_x;
        }

        if(keys[GLFW_KEY_LEFT]) r1.x = r1.x - 0.01;

        if(check_collision(r1, r2)){
            r1.x = r2.x + r2.width;
        }

        if(keys[GLFW_KEY_UP]) r1.y = r1.y + 0.01;

        if(check_collision(r1, r2)){
            r1.y = previous_y;
        }

        if(keys[GLFW_KEY_DOWN]) r1.y = r1.y - 0.01;

        if(check_collision(r1, r2)){
            r1.y = r2.y + r2.height;
        }

        glClearColor(0.3f, 0.9f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_rect(r1);
        draw_rect(r2);
        draw_rect(r3);
        draw_rect(r4);

        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();


    }

    // Optional cleaning up bc OS will likely do it for us, but is a good practice. Note that shaders are deleted in shader.h
    glDeleteProgram(quad_shader);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();

    return 0;

}

void draw_rect(Rectangle r1){
    glUniform2f(glGetUniformLocation(quad_shader, "trans"), r1.x, r1.y);
    glUniform2f(glGetUniformLocation(quad_shader, "scale"), r1.width, r1.height);
    glUniform4f(glGetUniformLocation(quad_shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
}

int check_collision(Rectangle r1, Rectangle r2){
    if (r1.x < r2.x + r2.width &&
        r1.x + r1.width > r2.x &&
        r1.y < r2.y + r2.height &&
        r1.y + r1.height > r2.y) {
            return 1; // collision
    } else {
        return 0;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = TRUE;
        else if(action == GLFW_RELEASE)
            keys[key] = FALSE;
    }
}


void setupVAO(){
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions            // textures
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // top right
         1.0f, 0.0f, 0.0f,     1.0f, 0.0f, // bottom right
        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, // bottom left
        0.0f,  1.0f, 0.0f,     0.0f, 1.0f // top left 
    };

    unsigned int indices[] = {  
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

GLuint getTextureHandle(char* path)
{
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    
    // Set our texture parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Set texture wrapping to GL_REPEAT
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);   // Set texture wrapping to GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load, create texture and generate mipmaps; 
    //
    // Note: image loaders usually think of top left as being (0,0) while in OpenGL I would rather think of bottom left as being (0,0) as OpenGL does that already, so that is why I set the stb library to flip image vertically. There are other workarounds like flipping our texCoords upside down or flipping things in the vs or fs, but that would mean that we are choosing in OpenGL to work with two different coordinate systems, one upside-down from the other. I would rather choose not to do that and simply flip images when loading in. It is a matter of personal choice.
    // 

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *image = stbi_load(path, &width, &height, &nrChannels, 0);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
   
    // free memory 
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind so that we can deal with other textures

    return textureHandle;
}



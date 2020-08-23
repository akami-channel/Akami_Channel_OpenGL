
#ifndef __APPLE__
    #include "Libraries/glad/glad.h"
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Libraries/GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Libraries/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Libraries/stb/stb_image_write.h"
#include "headers/shader.h"
#include <time.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1

#define FULLSCREEN FALSE
#define WRITING_TO_DISK FALSE

GLuint initial_screen_width = 600, initial_screen_height = 600;
const GLFWvidmode* mode;
GLFWwindow* window;

// for writing frame data to image
unsigned char frame_data[8294400]; // 4 * Width * Height = 4 * 1920 * 1080 = 8294400

// framebuffer stuff
unsigned int framebuffer;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int framebuffer_width = 1920, framebuffer_height = 1080;


int total_prints_this_batch = 0;
int num_frames_to_print = 1;

void setupVAO();
GLuint getTextureHandle(char* path);
unsigned int VAO; // There is also a line_VAO, but declared in main

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int keys[1024];
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

GLfloat mouse_last_x = 0.0, mouse_last_y = 0.0;
GLfloat mouse_click_x = 0.0, mouse_click_y = 0.0;
GLfloat mouse_release_y = 0.0, mouse_release_x = 0.0;
int firstMouse = TRUE;
int is_drawing_selection_box_bool = FALSE;

float currentTime, deltaTime, lastFrame;
float scr_width_float;
float scr_height_float;

typedef struct Color {
    float r, g, b, a;
} Color;

typedef struct Rectangle {
    float x, y;
    float width;
    float height;
    Color color;
} Rectangle;

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct ivec2 {
    int x, y;
} ivec2;

typedef struct Cell {
    int x, y;
    int is_alive; // bool; 0 or 1
    ivec2 trans;
} Cell;

int check_collision(Rectangle* r1, Rectangle* r2);
GLuint textured_quad_shader; // defined here bc used in the draw_rect function
void draw_rect(Rectangle r1, GLuint shader);
void draw_window_shader(GLuint shader, float scr_width_float, float scr_height_float);

int screen_width_int = 60;
int screen_height_int = 60;
int screen_total_num_tiles_int;
float screen_width_float = 0.0;
float screen_height_float = 0.0;

Cell *cells;
Cell *cells_copy;

#include "headers/cgol_functions.h"

int main (int argc, char* argv[]){

    srand(time(0)); 

    screen_total_num_tiles_int = screen_width_int * screen_height_int;
    screen_width_float = (float) screen_width_int;
    screen_height_float = (float) screen_height_int;

    cells = malloc(sizeof(Cell) * screen_total_num_tiles_int);
    for(int i = 0; i < screen_total_num_tiles_int; i++){
        cells[i].is_alive = rand() % 2;
    }

    for(int i = 0; i < screen_total_num_tiles_int; i++){
        cells[i].x = get_x_from_i(i);
        cells[i].y = get_y_from_i(i);
        cells[i].trans.x = cells[i].x;
        cells[i].trans.y = cells[i].y;
    }

    cells_copy = malloc(sizeof(Cell) * screen_total_num_tiles_int);
    for(int i = 0; i < screen_total_num_tiles_int; i++){
        cells_copy[i].is_alive = cells[i].is_alive;
        cells_copy[i].x = cells[i].x;
        cells_copy[i].y = cells[i].y;
        cells_copy[i].trans.x = cells[i].trans.x;
        cells_copy[i].trans.y = cells[i].trans.y;
    }

    // for debugging the cgol_functions:
    // for(int i = 0; i < screen_total_num_tiles_int; i++){
    //     printf("x: %d, y: %d\n", cells[i].x, cells[i].y);
    //     ivec2 input_vec = {cells[i].x, cells[i].y};
    //     printf("get_i_from_x_and_y: %d\n", get_i_from_x_and_y(input_vec) );
    // }

    if(argc >= 2) total_prints_this_batch = atoi(argv[1]);
    if(argc >= 3) num_frames_to_print = atoi(argv[2]);

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
        window = glfwCreateWindow(initial_screen_width, initial_screen_height, "My Title", NULL, NULL);
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    //glEnable(GL_FRAMEBUFFER_SRGB);

    setupVAO();
    glBindVertexArray(VAO); 
    // textured_quad_shader = glCreateProgram();
    // buildShaders(textured_quad_shader, "shaders/textured_quad.vs", "shaders/textured_quad.fs");

    // GLuint sprite_shader = glCreateProgram();
    // buildShaders(sprite_shader, "shaders/sprite.vs", "shaders/sprite.fs");

    // GLuint resting_sprite_shader = glCreateProgram();
    // buildShaders(resting_sprite_shader, "shaders/resting_sprite.vs", "shaders/resting_sprite.fs");

    // GLuint walking_zombie_shader = glCreateProgram();
    // buildShaders(walking_zombie_shader, "shaders/walking_zombie.vs", "shaders/walking_zombie.fs");

    GLuint colored_quad_shader = glCreateProgram();
    buildShaders(colored_quad_shader, "shaders/colored_quad.vs", "shaders/colored_quad.fs");

    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int window_width, window_height;

    // framebuffer stuff

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    unsigned int textureColorbuffer2;
    glGenTextures(1, &textureColorbuffer2);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer_width, framebuffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    // unsigned int rbo;
    // glGenRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // end framebuffer stuff


    // not sure if needed
    // glEnable(GL_DEPTH_TEST);

    glBindVertexArray(VAO);
    glUseProgram(colored_quad_shader);

    ivec2 current_cell = {0, 0};

    while (!glfwWindowShouldClose(window))
    {

        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // printf("Printing cells[i].is_alive for all cells \n");
        // for(int i = 0; i < screen_total_num_tiles_int; i++){
        //     if( i % screen_width_int == 0) printf("\n");
        //     printf("%d, ", cells[i].is_alive);         
        // }
        // printf("\n");

        
        // printf("starting\n");
        for(int i = 0; i < screen_total_num_tiles_int; i++){
            
            int neighbor_count = 0;

            cells_copy[i].is_alive = 0;
            if(get_right(cells[i].trans) == 1) neighbor_count++;
            if(get_left(cells[i].trans) == 1) neighbor_count++;
            if(get_down(cells[i].trans) == 1) neighbor_count++;
            if(get_up(cells[i].trans) == 1) neighbor_count++;
            if(get_down_right(cells[i].trans) == 1) neighbor_count++;
            if(get_up_right(cells[i].trans) == 1) neighbor_count++;
            if(get_down_left(cells[i].trans) == 1) neighbor_count++;
            if(get_up_left(cells[i].trans) == 1) neighbor_count++;
            // cells_copy[i].is_alive = cells[new_i].is_alive;

            // printf("cells[i].x: %d, cells[i].y: %d\n", cells[i].x, cells[i].y);
            // printf("input_vec x: %d, corrected x: %d\n", input_vec.x, corrected_vec.x);
            // printf("input_vec y: %d, corrected y: %d\n", input_vec.y, corrected_vec.y);
            // printf("cells[i].alive: %d\n", cells[i].is_alive);
            // printf("cells[new_i].is_alive: %d\n", cells[new_i].is_alive);
            // printf("i: %d\n", i);
            // printf("new_i: %d\n", new_i);
            // printf("\n");

            if (cells[i].is_alive && neighbor_count == 2) cells_copy[i].is_alive = 1;
            if (neighbor_count == 3) cells_copy[i].is_alive = 1;
        }

        for(int i = 0; i < screen_total_num_tiles_int; i++){
            cells[i].is_alive = cells_copy[i].is_alive;
        }

        // printf("Printing cells[i].is_alive again for all cells \n");
        // for(int i = 0; i < screen_total_num_tiles_int; i++){
        //     if( i % screen_width_int == 0) printf("\n");
        //     printf("%d, ", cells[i].is_alive);         
        // }
        // printf("\n");

        for(int i = 0; i < screen_total_num_tiles_int; i++){ 
            int x = i % screen_width_int;
            int y = floor(i / screen_width_float);
            float width = 2.0 / (screen_width_float);
            float height = 2.0 / screen_height_float; 
            glUniform2f(glGetUniformLocation(colored_quad_shader, "trans"), (float)x * width - 1.0, (float)y * height - 1.0);
            glUniform2f(glGetUniformLocation(colored_quad_shader, "scale"), width, height);
            glUniform4f(glGetUniformLocation(colored_quad_shader, "color"), (float)x * width / 2.0, (float)y * height / 2.0, 1.0, 1.0); // 0.0, 0.0, 1.0, 1.0
            if(cells[i].is_alive == 1) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // if(x == 1 && y == 1) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    
        }

// int accepted_x[2] = {3,4};
        // int accepted_y[2] = {5,6};
        // for(int i = 0; i < 31000; i++){ // 230 x 135 = 31050
        //     int x = i % 230;
        //     int y = floor(i / 230.0);
        //     float width = 2.0 / 230.0;
        //     float height = 2.0 / 135.0; 
        //     glUniform2f(glGetUniformLocation(colored_quad_shader, "trans"), (float)x * width - 1.0, (float)y * height - 1.0);
        //     glUniform2f(glGetUniformLocation(colored_quad_shader, "scale"), width, height);
        //     glUniform4f(glGetUniformLocation(colored_quad_shader, "color"), (float)x * width / 2.0, (float)y * height / 2.0, r1.color.b, r1.color.a);
        //     // if(x == accepted_x[0] && (int) y == accepted_y[1]) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    
        // }
        

        // printf("deltaTime: %f\n", deltaTime);
        // printf(".01666 - deltaTime: %f\n", .01666 - deltaTime);
        // printf("framerate: %f\n", 1.0 / deltaTime);

        // manually setting framerate to about 60
        // float framerate = 60.0;
        // usleep( (int) ( .01666 - deltaTime) * 1000000.0 );

        usleep(100000);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Optional cleaning up bc OS will likely do it for us, but is a good practice. Note that shaders are deleted in shader.h
    // glDeleteProgram(textured_quad_shader);
    // glDeleteProgram(sprite_shader);
    // glDeleteProgram(resting_sprite_shader);
    // glDeleteProgram(walking_zombie_shader);
    glDeleteProgram(colored_quad_shader);
    free(cells);
    free(cells_copy);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();

    return 0;

}

void draw_rect(Rectangle r1, GLuint shader){
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "trans"), r1.x, r1.y);
    glUniform2f(glGetUniformLocation(shader, "scale"), r1.width, r1.height);
    // glUniform4f(glGetUniformLocation(shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
}

void draw_window_shader(GLuint shader, float scr_width_float, float scr_height_float){
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "mouse"), mouse_last_x/ scr_width_float, mouse_last_y / scr_height_float);
    // glUniform1f(glGetUniformLocation(test_shader, "time"), currentTime * 1.);
    glUniform1f(glGetUniformLocation(shader, "time"), (float) total_prints_this_batch * 0.005);
    // glUniform1f(glGetUniformLocation(shader_glsl_sandbox_copy_paste6, "my_var"), my_var1);
    // glUniform1f(glGetUniformLocation(shader_glsl_sandbox_copy_paste6, "my_var2"), my_var2);
    // glUniform2f(glGetUniformLocation(shader_glsl_sandbox_copy_paste6, "surfacePosition"), scr_width_float, scr_height_float);
    //glUniform1f(glGetUniformLocation(shader_glsl_sandbox_copy_paste, "to_text"), currentTime);
    glUniform2f(glGetUniformLocation(shader, "resolution"), scr_width_float, scr_height_float);
    glUniform2f(glGetUniformLocation(shader, "trans"), -1.0, -1.0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 

}


int check_collision(Rectangle* r1, Rectangle* r2){
    if (r1->x < r2->x + r2->width &&
        r1->x + r1->width > r2->x &&
        r1->y < r2->y + r2->height &&
        r1->y + r1->height > r2->y) {
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_last_x = xpos;
    mouse_last_y = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        mouse_click_y = mouse_last_y;
        mouse_click_x = mouse_last_x; 
        printf("user has clicked mouse button left\n");
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        mouse_release_y = mouse_last_x;
        mouse_release_x = mouse_last_y; 
        printf("user has released mouse button left\n");
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(yoffset);
}

void setupVAO(){
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
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

GLuint getTextureHandle(char* path)
{
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    
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
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureHandle;
}



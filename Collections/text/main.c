
#ifndef __APPLE__
    #include "../../Libraries/glad/glad.h"
#endif

#include <stdio.h>
#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../../Libraries/GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../Libraries/stb/stb_image.h"
#include "headers/shader.h"

#define FULLSCREEN 0

GLuint screenWidth = 600, screenHeight = 600;
const GLFWvidmode* mode;
GLFWwindow* window;

void setupVAO();
GLuint getTextureHandle(char* path);
unsigned int VAO;

typedef struct ivec2 {
    int x;
    int y;
} ivec2;

/// Holds all state information relevant to a character as loaded using FreeType
typedef struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    ivec2   Size;      // Size of glyph
    ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
} Character;

float currentTime, deltaTime, lastFrame;

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
    glfwSwapInterval(1); // To my knowledge, this turns on vsync on macOS

    #ifndef __APPLE__
        // If Windows or Linux: load all OpenGL function pointers with GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            printf("Failed to initialize GLAD");
            return -1;
        }
    #endif

    // END Window setup

    // SETUP FOR TEXT WITH FREETYPE2 LIBRARY
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/lw2.ttf", 0, &face))
    {
        printf("ERROR::FREETYPE: Failed to load font; Please change path to a font that exists on your system.\n");  
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        // for (unsigned char c = 0; c < 128; c++)
        // {

        unsigned char c = 65; // Testing: setting to 65 for letter A

            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                printf("ERROR::FREETYTPE: Failed to load Glyph");
                return 4;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            ivec2 foo = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
            ivec2 bar = {face->glyph->bitmap_left, face->glyph->bitmap_top};
            int ifoo = face->glyph->advance.x;
            Character test_character = {
               texture,
               // ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
               // ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
               foo,
               bar,
               ifoo // for testing
               // static_cast<unsigned int>(face->glyph->advance.x)
            };
            //Characters.insert(std::pair<char, Character>(c, character));
            // test_character = <char, Character>(c, character);
        // }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // set up Vertex Array Object that contains our vertices and bind it
    setupVAO();
    glBindVertexArray(VAO);     

    GLuint quad_shader = glCreateProgram();
    buildShaders(quad_shader, "shaders/generic.vs", "shaders/quad.fs");

    GLuint text_shader = glCreateProgram();
    buildShaders(text_shader, "shaders/text.vs", "shaders/text.fs");

    GLuint channel_logo = getTextureHandle("assets/logo.png");
    glBindTexture(GL_TEXTURE_2D, channel_logo);

    // for alpha (opacity)
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_CULL_FACE);

    int window_width, window_height;

    while (!glfwWindowShouldClose(window))
    {

        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        glfwGetWindowSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);
        // printf("%d\n", window_width);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawArrays(GL_TRIANGLES, 0, 6);

// void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
// {
    // // activate corresponding render state  
    glUseProgram(text_shader);
    // glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    // glActiveTexture(GL_TEXTURE0);
    // glBindVertexArray(VAO);

    // // iterate through all characters
    // std::string::const_iterator c;
    // for (c = text.begin(); c != text.end(); c++) 
    // {
    //     Character ch = Characters[*c];

    //     float xpos = x + ch.Bearing.x * scale;
    //     float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    //     float w = ch.Size.x * scale;
    //     float h = ch.Size.y * scale;
    //     // update VBO for each character
    //     float vertices[6][4] = {
    //         { xpos,     ypos + h,   0.0f, 0.0f },            
    //         { xpos,     ypos,       0.0f, 1.0f },
    //         { xpos + w, ypos,       1.0f, 1.0f },

    //         { xpos,     ypos + h,   0.0f, 0.0f },
    //         { xpos + w, ypos,       1.0f, 1.0f },
    //         { xpos + w, ypos + h,   1.0f, 0.0f }           
    //     };
    //     // render glyph texture over quad
    //     glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    //     // update content of VBO memory
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     // render quad
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    //     // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    //     x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    // }
    // glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
// }


        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glUniform1f(glGetUniformLocation(quad_shader, "transX"), cos(currentTime));

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

void setupVAO(){
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions            // textures
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // top left 
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



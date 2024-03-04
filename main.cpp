#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

#include "shader_m.h"
#include "Numbers/NumShader.h"
#include "camera.h"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
//void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, FT_Face face);
void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color, FT_Face face);

void use3DMode();
void use2DMode();

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

std::map<GLchar, Character> Characters;
unsigned int VBO, cubeVAO, VAO;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CST-301 Project 6", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    use3DMode();
    //use2DMode();
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader5("2.2.basic_lighting.vs", "2.basic_lighting.fs");
    Shader lightingShader6("2.2.basic_lighting.vs", "4.basic_lighting.fs");
    Shader lightingShader7("2.2.basic_lighting.vs", "8.basic_lighting.fs");
    Shader lightingShader8("2.2.basic_lighting.vs", "16.basic_lighting.fs");
    Shader lightingShader1("2.2.basic_lighting.vs", "32.basic_lighting.fs");
    Shader lightingShader2("2.2.basic_lighting.vs", "64.basic_lighting.fs");
    Shader lightingShader3("2.2.basic_lighting.vs", "128.basic_lighting.fs");
    Shader lightingShader4("2.2.basic_lighting.vs", "256.basic_lighting.fs");
    Shader lightCubeShader("2.2.light_cube.vs", "2.2.light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // first, configure the cube's VAO (and VBO)
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create a shader program for rendering text
    Shader textShader("text.vs", "text.fs");
    //Shader textShader("Numbers/text.vs", "Numbers/text.fs"); // Create the text shader
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType setup (provided by learnopengl.com)
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

	// find path to font
    //std::string font_name = FileSystem::getPath("resources/fonts/Antonio-Bold.ttf");
    std::string font_name = "Numbers/Roboto-Regular.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }
	
	// load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
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
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    
    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Set font size (adjust as needed)
    //FT_Set_Pixel_Sizes(face, 0, 48);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT);

        //---------------------
        // CUBE 1
        //---------------------

        // lighting
        glm::vec3 lightPos0(-2.0f, -1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader1.use();
        lightingShader1.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader1.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader1.setVec3("lightPos", lightPos0);
        lightingShader1.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader1.setMat4("projection", projection);
        lightingShader1.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation0(-3.0f, -1.0f, 0.0f);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        float angle = glm::radians(30.0f); // 10 degrees
        glm::vec3 axis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation0); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader1.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //---------------------
        // CUBE Creation
        //---------------------

        // lighting
        glm::vec3 lightPos1(-0.4f, -1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader2.use();
        lightingShader2.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader2.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader2.setVec3("lightPos", lightPos1);
        lightingShader2.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader2.setMat4("projection", projection);
        lightingShader2.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation1(-1.0f, -1.0f, 0.0f);

        angle = glm::radians(17.5f); // 5 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation1); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader2.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //---------------------
        // CUBE Creation
        //---------------------

        // lighting
        glm::vec3 lightPos2(1.3f, -1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader3.use();
        lightingShader3.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader3.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader3.setVec3("lightPos", lightPos2);
        lightingShader3.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader3.setMat4("projection", projection);
        lightingShader3.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation2(1.0f, -1.0f, 0.0f);

        angle = glm::radians(8.0f); // 2.5 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation2); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader3.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //---------------------
        // CUBE Creation
        //---------------------

        // lighting
        glm::vec3 lightPos3(3.1f, -1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader4.use();
        lightingShader4.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader4.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader4.setVec3("lightPos", lightPos3);
        lightingShader4.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader4.setMat4("projection", projection);
        lightingShader4.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation3(3.0f, -1.0f, 0.0f);

        angle = glm::radians(0.0f); // 1.25 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation3); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader4.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //---------------------
        // CUBE 1
        //---------------------

        // lighting
        glm::vec3 lightPos4(-2.0f, 1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader5.use();
        lightingShader5.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader5.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader5.setVec3("lightPos", lightPos4);
        lightingShader5.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader5.setMat4("projection", projection);
        lightingShader5.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation4(-3.0f, 1.0f, 0.0f);

        angle = glm::radians(30.0f); // 25.0 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation4); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader5.setMat4("model", model);


        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //---------------------
        // CUBE Creation
        //---------------------

        // lighting
        glm::vec3 lightPos5(-0.4f, 1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader6.use();
        lightingShader6.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader6.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader6.setVec3("lightPos", lightPos5);
        lightingShader6.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader6.setMat4("projection", projection);
        lightingShader6.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation5(-1.0f, 1.0f, 0.0f);

        angle = glm::radians(17.5f); // 1.25 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation5); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader6.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //---------------------
        // CUBE Creation
        //---------------------

        // lighting
        glm::vec3 lightPos6(1.3f, 1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader7.use();
        lightingShader7.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader7.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader7.setVec3("lightPos", lightPos6);
        lightingShader7.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader7.setMat4("projection", projection);
        lightingShader7.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation6(1.0f, 1.0f, 0.0f);

        angle = glm::radians(8.0f); // 1.25 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation6); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader7.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //---------------------
        // CUBE Creation
        //---------------------

        // lighting
        glm::vec3 lightPos7(3.1f, 1.0f, 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader8.use();
        lightingShader8.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader8.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader8.setVec3("lightPos", lightPos7);
        lightingShader8.setVec3("viewPos", camera.Position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader8.setMat4("projection", projection);
        lightingShader8.setMat4("view", view);

        // Define the translation vector
        glm::vec3 translation7(3.0f, 1.0f, 0.0f);

        angle = glm::radians(0.0f); // 1.25 degrees

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, translation7); // Apply the translation
        model = glm::rotate(model, angle, axis); // Apply the rotation transformation to the model matrix
        lightingShader8.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render text
        //textShader.use(); //Unnecessary, already covered by renderText()
        renderText(textShader, "2",   175.0f, 300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "4",   320.0f, 300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "8",   465.0f, 300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "16",  610.0f, 300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "32",  175.0f, 150.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "64",  320.0f, 150.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "128", 465.0f, 150.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);
        renderText(textShader, "256", 610.0f, 150.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), face);

        // Set up vertex data (and buffer(s)) and attribute pointers
        // GLfloat verticesNum[] = {
        //     // Positions          // Colors           // Texture Coords
        //     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
        //     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        //     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
        // };
        // GLuint indicesNum[] = {  // Note that we start from 0!
        //     0, 1, 3, // First Triangle
        //     1, 2, 3  // Second Triangle
        // };
        // GLuint VBO, VAO, EBO;
        // glGenVertexArrays(1, &VAO);
        // glGenBuffers(1, &VBO);
        // glGenBuffers(1, &EBO);

        // glBindVertexArray(VAO);

        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(verticesNum), verticesNum, GL_STATIC_DRAW);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesNum), indicesNum, GL_STATIC_DRAW);

        

        // Position attribute
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        // glEnableVertexAttribArray(0);
        // // Color attribute
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        // glEnableVertexAttribArray(1);
        // // TexCoord attribute
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        // glEnableVertexAttribArray(2);

        // glBindVertexArray(0); // Unbind VAO

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // Cleanup FreeType resources
    // FT_Done_Face(face);
    // FT_Done_FreeType(ft);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Function to render text
// render line of text
// -------------------
void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color, FT_Face face)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, FT_Face face)
// {
//     float textVertices[] = {
//         // Vertex attributes for a quad (x, y, s, t)
//         // positions           // texture coordinates
//         0.0f, 1.0f, 0.0f, 1.0f,
//         1.0f, 0.0f, 1.0f, 0.0f,
//         0.0f, 0.0f, 0.0f, 0.0f,

//         0.0f, 1.0f, 0.0f, 1.0f,
//         1.0f, 1.0f, 1.0f, 1.0f,
//         1.0f, 0.0f, 1.0f, 0.0f
//     };

//     float vertices[] = {
//         // Vertex attributes for a quad (x, y, s, t)
//         // positions           // texture coordinates
//         0.0f, 1.0f, 0.0f, 1.0f,
//         1.0f, 0.0f, 1.0f, 0.0f,
//         0.0f, 0.0f, 0.0f, 0.0f,

//         0.0f, 1.0f, 0.0f, 1.0f,
//         1.0f, 1.0f, 1.0f, 1.0f,
//         1.0f, 0.0f, 1.0f, 0.0f
//     };

//     for (unsigned int i = 0; i < text.length(); i++)
//     {
//         unsigned int textVBO, textVAO;
//         glGenVertexArrays(1, &textVAO);
//         glGenBuffers(1, &textVBO);

//         // Bind the VAO and VBO
//         glBindVertexArray(textVAO);
//         glBindBuffer(GL_ARRAY_BUFFER, textVBO);

//         // Allocate memory for the text VBO and provide data
//         glBufferData(GL_ARRAY_BUFFER, sizeof(textVertices), textVertices, GL_DYNAMIC_DRAW);

//         // Set the vertex attribute pointers for the text
//         glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//         glEnableVertexAttribArray(0);

//         // Unbind the VBO and VAO
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);

//         char c = text[i];

//         // Load character glyph 
//         if (FT_Load_Char(face, c, FT_LOAD_RENDER))
//         {
//             std::cerr << "Error: Failed to load character glyph" << std::endl;
//             continue;
//         }

//         // Generate texture
//         GLuint texture;
//         glGenTextures(1, &texture);
//         glBindTexture(GL_TEXTURE_2D, texture);
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

//         // Set texture options
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         // Render glyph texture over quad
//         glBindVertexArray(textVAO);
//         // Update content of VBO memory
//         glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

//         // Render quad
//         glDrawArrays(GL_TRIANGLES, 0, 6);
//     }
// }

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset/10, yoffset/10);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void use3DMode()
{
    glEnable(GL_DEPTH_TEST);
}

void use2DMode()
{
    glDisable(GL_DEPTH_TEST);

}

void render3DScene()
{
    
}
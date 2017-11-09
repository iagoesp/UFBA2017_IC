#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#define MESH_SIZE 8

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// definição da janela
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = "#version 330 core\n"
   "layout (location = 0) in vec3 aPos;\n"
   "uniform mat4 MVP;\n"
    "void main()\n"
    "{\n"
    "     gl_Position =  MVP * vec4(aPos,1);\n"
    "}\0";

//declaração do Fragment Shader
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n\0";

int main(){
    // inicialização do GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criação da Janela
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ativ 2 - Sinewave", NULL, NULL);
    if (window == NULL){
        cout << "Erro em criar a janela" <<endl;
        glfwTerminate();
        return -1;
    }

    // função que "vai para a janela"
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // inicialização do glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // cosntrução do vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);            //Criação do VertexShader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);                                  //Compilação do VertexShader

    // verificação do shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // verificação do fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);        //Criação do FragmentShader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);                                //Compilação do FragmentShader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Vínculo entre shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog <<endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");

    //glm::vec2 mesh[MESH_SIZE][MESH_SIZE];
    //const GLuint indexesSize = pow(MESH_SIZE,2)*2-1;
    //const GLuint indexesSize = pow(MESH_SIZE,2)*2;
   // glm::vec3 index[indexesSize];

    glm::mat4 Projection = glm::perspective(glm::radians(0.0f), (float) SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 View = glm::lookAt(
        glm::vec3(0,8,0), // Camera is at (4,3,3), in World Space
        glm::vec3(0,0,0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model;

    vector <GLfloat> vert;
    vert.clear();
    float index=4;
    float tamAmostra = float(MESH_SIZE)/index;
    //declaração dos vértices
    for(int i=0; i<index; i++)
        for(int j=0; j<index; j++){
            float auxx=i*tamAmostra;
            float auxy=j*tamAmostra;
            vert.push_back(auxx);
            vert.push_back(auxy);
        }

    vector<GLfloat> indices;
    indices.clear();
    for(int i=0; i<index; i++)
        for(int j=0; j<index; j++){
            indices.push_back(vert[i+j+0]);
            indices.push_back(vert[i+j+1]);
            indices.push_back(vert[i+j+2]);
            indices.push_back(vert[i+j+3]);
            indices.push_back(vert[i+j+4]);
            indices.push_back(vert[i+j+5]);
        }

//    float vertices[] = {
//         0.3f,  0.3f, -1.0f,  // 0
//         0.3f, -0.3f, 0.0f,  // 1
//        -0.3f, -0.3f, 1.0f,  // 2
//        -0.3f,  0.3f, 0.0f   // 3
//    };

//    definição de cada triângulo
//    unsigned int indices[] = {
//        0, 1, 3,
//        1, 2, 3
//    };

    //declaração do Elements Buffer Object
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    //declaração do Vertex Buffer e Vertex Array Objects
    unsigned int VBOv, VBOi, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOi);
    glGenBuffers(1, &VBOv);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOv);
    glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   // glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), &vert[0][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOi);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLfloat), &indices[0], GL_STATIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), &index[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // renderização
    while (!glfwWindowShouldClose(window))    {
        // Janela
        processInput(window);

        // renderização dos objetos - bg
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //utilização do Shader
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glBindVertexArray(VBOi);                                                                                                          // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_POINTS, indices.size()*sizeof(GLfloat), GL_FLOAT, 0);

        glBindVertexArray(0); // no need to unbind it every time

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBOv);
    glDeleteBuffers(1, &VBOi);
    glDeleteVertexArrays(1, &VAO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

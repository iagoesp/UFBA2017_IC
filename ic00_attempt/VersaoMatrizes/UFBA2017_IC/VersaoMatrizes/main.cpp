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
    "layout (location = 0) in vec2 aPos;\n"
    "uniform mat4 MVP;\n"
    "void main()\n"
    "{\n"
    "     gl_Position =  MVP * vec4(aPos.x, 0.0, aPos.y, 1.0);\n"
    "}\0";

//declaração do Fragment Shader
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n\0";

    int teste=0;

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

    glm::mat4 Projection = glm::perspective(glm::radians(0.0f), (float) SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 View = glm::lookAt(
        glm::vec3(8,8,0), // Camera is at (4,3,3), in World Space
        glm::vec3(0,0,0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model;

    glUseProgram(shaderProgram);
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(mvp));


    vector <GLfloat> vert;
    vector<GLint> indices;
    vert.clear(); indices.clear();
    int nVert=0, nTri=0;
    float index=4.0;
    float tamAmostra = float(MESH_SIZE)/index;
    //declaração dos vértices
    for(int i=0; i<index; i++)
        for(int j=0; j<index; j++){
            float auxx=i*tamAmostra;
            float auxy=j*tamAmostra;
            vert.push_back(auxx);
            vert.push_back(auxy);
            nVert++;
        }

    for(int i=0; i<MESH_SIZE; i++)
        for(int j=0; j<MESH_SIZE; j++){
            indices.push_back( i*(MESH_SIZE+1)+j);
            indices.push_back( (i+1)*(MESH_SIZE+1)+j);
            indices.push_back( i*(MESH_SIZE+1)+j+1);
            nTri++;
            indices.push_back( i*(MESH_SIZE+1)+j+1);
            indices.push_back( (i+1)*(MESH_SIZE+1)+j);
            indices.push_back( (i+1)*(MESH_SIZE+1)+j+1);
            nTri++;
        }

//  declaração do Elements Buffer Object
//    unsigned int EBO;
//    glGenBuffers(1, &EBO);

    //declaração do Vertex Buffer e Vertex Array Objects
    unsigned int VBOv, VBOi;
//    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOv);
    glBindBuffer(GL_ARRAY_BUFFER, VBOv);
    glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(GLfloat), vert.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &VBOi);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOi);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // renderização
    while (!glfwWindowShouldClose(window))    {
        // Janela
        processInput(window);
        //glBindBuffer(GL_ARRAY_BUFFER, VBOv);

        // renderização dos objetos - bg
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
       // glBindBuffer(GL_ARRAY_BUFFER, VBOi);

        //utilização do Shader
        glUseProgram(shaderProgram);
        //glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOi);
                                                  cout<<"ok3"<<endl;
                                                              // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
       // glDrawElements(GL_TRIANGLES, indices.size()*sizeof(GLfloat), GL_INT, 0);
        glDrawElements(GL_TRIANGLES, nTri*3, GL_UNSIGNED_INT, 0);
    cout<<"ok4"<<endl;

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    cout<<"ok"<<endl;

    glDeleteBuffers(1, &VBOv);
    glDeleteBuffers(1, &VBOi);
//    glDeleteVertexArrays(1, &VAO);

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

#include <vector>
#include <iostream>
#include <string>
#include "vectormath/vectormath.h"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

#include "LoadShaders.hpp"
#include "controls.hpp"

static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static float TessLevelInner;
static float TessLevelOuter;

int main(int argv, char** argc){
	// Initialise GLFW
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1280, 1024, "IPSinewave_v6", NULL, NULL);
	if( window == NULL ){
    cout << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		cout<<"Failed to initialize GLEW\n";
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1280/2, 1024/2);

	// Dark blue background
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "Geodesic.Vertex",  "Geodesic.TessControl", "Geodesic.TessEval", "Geodesic.Geometry", "Geodesic.Fragment");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID             = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixID        = glGetUniformLocation(programID, "M");
	GLuint ViewMatrixID         = glGetUniformLocation(programID, "V");
	GLuint ProjectionMatrixID   = glGetUniformLocation(programID, "P");
    GLuint TessLevelInnerID     = glGetUniformLocation(programID, "TessLevelInner" );// Inner tessellation paramter
    GLuint TessLevelOuterID     = glGetUniformLocation(programID, "TessLevelOuter" );  // TessLevelOuter tessellation paramter

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const int Faces[] = {
              2, 1, 0,
              3, 2, 0,
              4, 3, 0,
              5, 4, 0,
              1, 5, 0,

              11, 6,  7,
              11, 7,  8,
              11, 8,  9,
              11, 9,  10,
              11, 10, 6,

              1, 2, 6,
              2, 3, 7,
              3, 4, 8,
              4, 5, 9,
              5, 1, 10,

              2,  7, 6,
              3,  8, 7,
              4,  9, 8,
              5, 10, 9,
              1, 6, 10 };

    const float Verts[] = {
               0.000f,  0.000f,  1.000f,
               0.894f,  0.000f,  0.447f,
               0.276f,  0.851f,  0.447f,
              -0.724f,  0.526f,  0.447f,
              -0.724f, -0.526f,  0.447f,
               0.276f, -0.851f,  0.447f,
               0.724f,  0.526f, -0.447f,
              -0.276f,  0.851f, -0.447f,
              -0.894f,  0.000f, -0.447f,
              -0.276f, -0.851f, -0.447f,
               0.724f, -0.526f, -0.447f,
               0.000f,  0.000f, -1.000f };

    IndexCount = sizeof(Faces) / sizeof(Faces[0]);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 3 * sizeof(float);

    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);


    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
    // For speed computation
    //double lastTime = glfwGetTime(); int nbFrames = 0;
    TessLevelInner = 1.0f;
    TessLevelOuter = 1.0f;

    do{
        // Measure speed
        // double currentTime = glfwGetTime();//		nbFrames++; //		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago //			// printf and reset //			cout<<1000.0/double(nbFrames)<<"ms/frame\n";    //			nbFrames = 0;   //			lastTime += 1.0;    //}

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform

        if (glfwGetKey( window, GLFW_KEY_C ) == GLFW_PRESS){
           TessLevelInner+=1.0;
        }
        if (glfwGetKey( window, GLFW_KEY_V ) == GLFW_PRESS){
            TessLevelInner = TessLevelInner > 1 ? TessLevelInner - 1 : 1;
        }
        if (glfwGetKey( window, GLFW_KEY_B ) == GLFW_PRESS){
            TessLevelOuter+=1.0;
        }
        if (glfwGetKey( window, GLFW_KEY_N ) == GLFW_PRESS){
            TessLevelOuter = TessLevelOuter > 1 ? TessLevelOuter - 1 : 1;
        }
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
        glUniform1f( TessLevelInnerID, TessLevelInner );
        glUniform1f( TessLevelOuterID, TessLevelOuter );

        glPatchParameteri(GL_PATCH_VERTICES, 3);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, positions);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

        // Draw the triangles !
        glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 );

        // Cleanup VBO and shader
    glDeleteBuffers(1, &positions);
    glDeleteBuffers(1, &indices);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &vao);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

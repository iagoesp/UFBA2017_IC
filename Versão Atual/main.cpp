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
extern glm::vec3 position;

bool adapt = true;
bool unif  = false;
bool geom = false;
static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static float TessLevelInner;
static float TessLevelOuter;

int main(int argv, char** argc){
	// Initialise GLFW
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
	//glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);
//	if()
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
    GLuint programAdaptID = LoadShaders( "terrenofBm.vert",  "tessAdapt.tesc", "tessAdapt.tese", "tessAdapt.frag");
    GLuint programGeomID  = LoadShaders( "Geodesic.vert", "Geodesic.geom","Geodesic.frag");
    GLuint programUnifID  = LoadShaders( "terrenofBm.vert",  "tessUnif.tesc", "tessUnif.tese", "tessUnif.frag");
	// Get a handle for all uniforms
	GLuint MatrixID             = glGetUniformLocation(programAdaptID, "MVP");
	GLuint ModelMatrixID        = glGetUniformLocation(programAdaptID, "M");
	GLuint ViewMatrixID         = glGetUniformLocation(programAdaptID, "V");
	GLuint ProjectionMatrixID   = glGetUniformLocation(programAdaptID, "P");
    GLuint cameraPosIDX         = glGetUniformLocation(programAdaptID, "px");
    GLuint cameraPosIDY         = glGetUniformLocation(programAdaptID, "py");
    GLuint cameraPosIDZ         = glGetUniformLocation(programAdaptID, "pz");
	GLuint ampValue             = glGetUniformLocation(programAdaptID, "amp");

	// Get a handle for all uniforms
    GLuint MatrixUID             = glGetUniformLocation(programUnifID, "MVP");
	GLuint ModelMatrixUID        = glGetUniformLocation(programUnifID, "M");
	GLuint ViewMatrixUID         = glGetUniformLocation(programUnifID, "V");
	GLuint ProjectionMatrixUID   = glGetUniformLocation(programUnifID, "P");
    GLuint cameraPosUIDX         = glGetUniformLocation(programUnifID, "px");
    GLuint cameraPosUIDY         = glGetUniformLocation(programUnifID, "py");
    GLuint cameraPosUIDZ         = glGetUniformLocation(programUnifID, "pz");
	GLuint ampUValue             = glGetUniformLocation(programUnifID, "amp");
    GLuint TessLevelInnerUID     = glGetUniformLocation(programUnifID, "TessLevelInner" );// Inner tessellation paramter
    GLuint TessLevelOuterUID     = glGetUniformLocation(programUnifID, "TessLevelOuter" );  // TessLevelOuter tessellation paramter

    //GLuint programGeomID = LoadShaders("terrenofBm_vG.vert", "tessGeom.geom","tessUnif.frag");
	// Get a handle for all uniforms
    GLuint MatrixGID             = glGetUniformLocation(programGeomID, "MVP");
	GLuint ModelMatrixGID        = glGetUniformLocation(programGeomID, "M");
	GLuint ViewMatrixGID         = glGetUniformLocation(programGeomID, "V");
	GLuint ProjectionMatrixGID   = glGetUniformLocation(programGeomID, "P");
    GLuint cameraPosGIDX         = glGetUniformLocation(programGeomID, "px");
    GLuint cameraPosGIDY         = glGetUniformLocation(programGeomID, "py");
    GLuint cameraPosGIDZ         = glGetUniformLocation(programGeomID, "pz");
	GLuint ampGValue             = glGetUniformLocation(programGeomID, "amp");
    GLuint TessLevelInnerGID     = glGetUniformLocation(programGeomID, "TessLevelInner" );// Inner tessellation paramter
    GLuint TessLevelOuterGID     = glGetUniformLocation(programGeomID, "TessLevelOuter" );  // TessLevelOuter tessellation paramter

    vector<unsigned short> indices;
    const GLuint index = 40.0;
    const GLfloat meshSize = 40.0;
    float tamAmostra = meshSize / (float)index;
    for (GLuint i = 0 ; i < index ; i++){
		for (GLuint j = 0 ; j < index ; j++) {
			indices.push_back( i*(index+1) 		+ j);		// V0
			indices.push_back( i*(index+1) 		+ (j+1));	// V1
			indices.push_back( (i+1)*(index+1) 	+ j);		// V2

			indices.push_back( i*(index+1) 		+ (j+1));	// V1
			indices.push_back( (i+1)*(index+1) 	+ (j+1));	// V3
			indices.push_back( (i+1)*(index+1) 	+ j);		// V2
		}
	}

    float freq = 3.0, amp = 4.0;
    vector<glm::vec3> vec_y;
    vector<GLfloat> vertices;
    float minn=100000000, maxx=-1000000000;
    for (GLfloat i = 0 ; i <= index ; i+=1.0){
		for (GLfloat j = 0 ; j <= index ; j+=1.0) {
            float y = cos((i*tamAmostra)) * sin((j*tamAmostra));
            y = y*amp;
            //cout<<"y = "<< y<<endl;
            if(y<minn) minn=y;
            if(y>maxx) maxx=y;
            glm::vec3 aux;
            aux.x = i*tamAmostra;
            aux.y = y;
            aux.z = j*tamAmostra;
            vec_y.push_back(aux);
            vertices.push_back((float)(i*tamAmostra));
            vertices.push_back((float)(j*tamAmostra));
        }
	}

    IndexCount = sizeof(indices) / sizeof(indices[0]);

    // Create the VBO for positions:
    GLuint vertexbuffer;
    //GLsizei stride = 2 * sizeof(float);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Create the VBO for indices:
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    //GLuint normalbuffer;
    //glGenBuffers(1, &normalbuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
//	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);


    // For speed computation
    TessLevelInner = 1.0f;
    TessLevelOuter = 4.0f;
    float distance;
    glm::vec3 camerapos = position;
    float minnn=100000000, maxxx=-1000000000;

    do{

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        //glUseProgram(programAdaptID);
        if (glfwGetKey( window, GLFW_KEY_J ) == GLFW_PRESS){
           glUseProgram(programAdaptID);
           adapt = true;
           unif = false;
           geom = false;
        }
        if (glfwGetKey( window, GLFW_KEY_K ) == GLFW_PRESS){
           glUseProgram(programUnifID);
           adapt = false;
           unif = true;
           geom = false;
        }

        if (glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS){
           glUseProgram(programUnifID);
           adapt = false;
           unif = false;
           geom = true;
        }

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        float px = position.x; float py = position.y; float pz = position.z;
               //cout<<"     min = "<<minnn<<" e max = "<<maxxx<<endl;
        if (glfwGetKey( window, GLFW_KEY_U ) == GLFW_PRESS){
           glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (glfwGetKey( window, GLFW_KEY_I ) == GLFW_PRESS){
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
            glEnable(GL_CULL_FACE);
        }
        if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
            glDisable(GL_CULL_FACE);
        }

        if(adapt){
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
            glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
            glUniform1f(cameraPosIDX, px);
            glUniform1f(cameraPosIDY, py);
            glUniform1f(cameraPosIDZ, pz);
            glUniform1f(ampValue,amp);
        }
        else if(unif){
            glUniformMatrix4fv(MatrixUID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixUID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixUID, 1, GL_FALSE, &ViewMatrix[0][0]);
            glUniformMatrix4fv(ProjectionMatrixUID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
            glUniform1f( TessLevelInnerUID, TessLevelInner );
            glUniform1f( TessLevelOuterUID, TessLevelOuter );
            glUniform1f(cameraPosUIDX, px);
            glUniform1f(cameraPosUIDY, py);
            glUniform1f(cameraPosUIDZ, pz);
            glUniform1f(ampUValue,amp);
        }
        else if(geom){
            glUniformMatrix4fv(MatrixGID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixGID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixGID, 1, GL_FALSE, &ViewMatrix[0][0]);
            glUniformMatrix4fv(ProjectionMatrixGID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
            glUniform1f(cameraPosGIDX, px);
            glUniform1f(cameraPosGIDY, py);
            glUniform1f(cameraPosGIDZ, pz);
            glUniform1f(ampGValue,amp);
        }

        if(adapt || unif)
            glPatchParameteri(GL_PATCH_VERTICES, 3);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        if(adapt || unif)
            glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        else
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 );

        // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    if(adapt)
        glDeleteProgram(programAdaptID);
    else if(unif)
        glDeleteProgram(programUnifID);
    else if(geom)
        glDeleteProgram(programGeomID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

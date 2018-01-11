#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "LoadShaders.hpp"

GLuint LoadShaders(const char * vertex_file_path, const char * control_file_path, const char * evaluation_file_path, const char * fragment_file_path){
//GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint ControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint EvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
//    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
   	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);




	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}


	std::string TessControlShaderCode;
	std::ifstream TessControlStream(control_file_path, std::ios::in);
	if(TessControlStream.is_open()){
		std::string Line = "";
		while(getline(TessControlStream, Line))
			TessControlShaderCode += "\n" + Line;
		TessControlStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", control_file_path);
		getchar();
		return 0;
	}


    std::string TessEvaluationShaderCode;
	std::ifstream TessEvaluationStream(evaluation_file_path, std::ios::in);
	if(TessEvaluationStream.is_open()){
		std::string Line = "";
		while(getline(TessEvaluationStream, Line))
			TessEvaluationShaderCode += "\n" + Line;
		TessEvaluationStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", evaluation_file_path);
		getchar();
		return 0;
	}
//
//    // Read the Geometry Shader code from the file
//	std::string GeometryShaderCode;
//	std::ifstream GeometryShaderStream(geometry_file_path, std::ios::in);
//	if(GeometryShaderStream.is_open()){
//		std::string Line = "";
//		while(getline(GeometryShaderStream, Line))
//			GeometryShaderCode += "\n" + Line;
//		GeometryShaderStream.close();
//	}else{
//		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", geometry_file_path);
//		getchar();
//		return 0;
//	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", fragment_file_path);
		getchar();
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



// Compile Tesselation Control Shader
	printf("Compiling shader : %s\n", control_file_path);
	char const * TessControlSourcePointer = TessControlShaderCode.c_str();
	glShaderSource(ControlShaderID, 1, &TessControlSourcePointer , NULL);
	glCompileShader(ControlShaderID);

	// Check Vertex Shader
	glGetShaderiv(ControlShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ControlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> TessControlErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(ControlShaderID, InfoLogLength, NULL, &TessControlErrorMessage[0]);
		printf("%s\n", &TessControlErrorMessage[0]);
	}

// Compile Tesselation Evaluation Shader
	printf("Compiling shader : %s\n", evaluation_file_path);
	char const * EvaluationSourcePointer = TessEvaluationShaderCode.c_str();
	glShaderSource(EvaluationShaderID, 1, &EvaluationSourcePointer , NULL);
	glCompileShader(EvaluationShaderID);

	// Check Vertex Shader
	glGetShaderiv(EvaluationShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(EvaluationShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> EvaluationShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(EvaluationShaderID, InfoLogLength, NULL, &EvaluationShaderErrorMessage[0]);
		printf("%s\n", &EvaluationShaderErrorMessage[0]);
	}
//
//// Compile Geometry Shader
//	printf("Compiling shader : %s\n", geometry_file_path);
//	char const * GeometrySourcePointer = GeometryShaderCode.c_str();
//	glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer , NULL);
//	glCompileShader(GeometryShaderID);
//
//	// Check Geometry Shader
//	glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if ( InfoLogLength > 0 ){
//		std::vector<char> GeometryShaderErrorMessage(InfoLogLength+1);
//		glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
//		printf("%s\n", &GeometryShaderErrorMessage[0]);
//	}

// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, ControlShaderID);
	glAttachShader(ProgramID, EvaluationShaderID);
    //glAttachShader(ProgramID, GeometryShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, ControlShaderID);
	glDetachShader(ProgramID, EvaluationShaderID);
    //glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
    glDeleteShader(ControlShaderID);
	glDeleteShader(EvaluationShaderID);
    //glDeleteShader(GeometryShaderID);
	glDeleteShader(FragmentShaderID);


	return ProgramID;
}



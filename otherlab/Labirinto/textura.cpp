// OpenGL Utility Toolkit
#include <GL/glut.h>
#include "load_texture.h"

void Inicializa(){
glutInitWindowSize(1512, 1512);

	glMatrixMode(GL_PROJECTION);

	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);

}

void Desenha(){
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint textureID = loadBMP("img_test_128x128.bmp");
	glBegin(GL_TRIANGLES);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(0.5, 0.5, 0.0);

		glTexCoord2f(0.0, 1.0);
		glVertex3f(-0.5, 0.5, 0.0);

		glTexCoord2f(0.5, 0.0);
		glVertex3f(0.0, -0.5, 0.0);
	glEnd();

  glFlush();
}

// Programa principal
int main(int argc, char** argv){

  // Inicia GLUT e processa argumentos passados por linha de comandos
  glutInit(&argc, argv);

  // Avisa a GLUT que tipo de modo de exibição deve ser usado quando a janela é criada
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  // Cria uma janela GLUT que permite a execução de comandos OpenGL
  glutCreateWindow("Olá Mundo!");

  // Define a funçãoo respons�vel por redesenhar a janela OpenGL sempre que necessário
  glutDisplayFunc(Desenha);

  // Inicializações de OpenGL executadas antes da exibição do desenho
  Inicializa();

  // Inicia o processamento de eventos de GLUT. O controle do programa
  // passa a GLUT, que inicia o gerenciamento dos eventos
  glutMainLoop();

  return 0;
}

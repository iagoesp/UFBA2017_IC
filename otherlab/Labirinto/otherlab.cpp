/*
------------ PROJETO GENESIS --------------
Autor : Marcos Romero (RH Games - Beljogos)
Email : romero1978@gmail.com
Página: http://romerogames.blogspot.com
-------------------------------------------
*/


/*

lab3d.c

Comentários :

Este programa desenha um simples labirinto 3D.
O usuario pode se movimentar atraves das setas do teclado.
É verificada a colisao entre o usuario e as paredes.
Ha uma esfera que se movimenta pelo labirinto desviando das paredes.
Para visualizar o labirinto em modo wireframe pressione a tecla 'W'.

O programa foi feito usando OpenGL e Glut.

*/

//INCLUDES --------------------
#include <windows.h>     //biblioteca do windows...
#include <GL/gl.h>       //biblioteca gl.h
#include <GL/glut.h>     //biblioteca glut - ferramentas adicionais
#include <math.h>
#include <iostream>

using namespace std;

//DEFINES--------------------------------------------

#define TAM_BLOCO 100
#define PASSO	20

#define NORTE  0
#define LESTE  1
#define SUL    2
#define OESTE  3

/* ESTRUTURAS------------ */

typedef struct _esf
{
	GLint  x_pos, z_pos;
	GLint dir;

} ESFERA;

int pessoa3=1;

//GLOBALS--------------------------------------------

GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_amarelo[] = {0.8, 0.8, 0.1, 1.0};
GLfloat mat_verde[] = { 0.1, 0.6, 0.1, 1.0 };
GLfloat mat_vermelho[] = { 0.7, 0.1, 0.1, 1.0 };
GLfloat light_position[] = { 0.0, 60.0, 0.0, 1.0 };
GLfloat luz_branca[] = {1.0,1.0,1.0,1.0};
GLfloat lmodel_ambient[] = {0.6,0.6,0.6,1.0};


GLfloat jog_x= TAM_BLOCO , jog_z=TAM_BLOCO ;
GLfloat mov_x=PASSO, mov_z=0;
GLint angulo=0;
GLint wire = 0;
int posInicial=0;

ESFERA g_esfera;


/*labirinto */
GLint mapa[100][100];// = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
//                      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


//FUNCOES--------------------------------------------

void fimDeJogo(float win){
    win=win;
}

int verificarMovimento(float pos_x, float pos_z, float vet_x, float vet_z){
	float mundo_x = pos_x + vet_x ;
	float mundo_z = pos_z + vet_z ;

	int ind_x = (int) ((mundo_x + TAM_BLOCO/2) / TAM_BLOCO);
	int ind_z = (int) ((mundo_z + TAM_BLOCO/2) / TAM_BLOCO);

	if(mapa[ind_x][ind_z]==4 || mapa[ind_x][ind_z]==3){
        if(mapa[ind_x][ind_z]==3){
            glutDestroyWindow(1);
        }
        return 1;
	}
	else{
        return 0;
	}
}

//---------------------------------------------

void display(void){
    int x, z;
    int x_mun, z_mun;

    //limpa todos os pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    if(pessoa3)
        gluLookAt(750, 1000, 700, 750, 250, 700.01, 0,1,0);
    else
        gluLookAt(jog_x, 15, jog_z, jog_x+mov_x, 15, jog_z+mov_z, 0,1,0);


    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_verde);

    glBegin ( GL_QUADS);
        glVertex3f(-10000, -TAM_BLOCO/2, -10000);
        glVertex3f(-10000, -TAM_BLOCO/2, 10000);
        glVertex3f(10000, -TAM_BLOCO/2, 10000);
        glVertex3f(10000, -TAM_BLOCO/2, -10000);
    glEnd();

    glPopMatrix();

    //desenha esfera
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_vermelho);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

        glTranslatef(g_esfera.x_pos, 40 ,g_esfera.z_pos);

        glutSolidSphere(16,16,16);
    glPopMatrix();


    for(x=0; x < 15; x++){
        for(z=0; z < 15; z++){
            if(mapa[x][z]==1){
                x_mun = x * TAM_BLOCO;
                z_mun = z * TAM_BLOCO;
                glPushMatrix();

                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amarelo);
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
                glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

                glTranslatef(x_mun, 5 ,z_mun);

                if(wire)
                    glutWireCube(TAM_BLOCO);
                else
                    glutSolidCube(TAM_BLOCO);

                glPopMatrix();

            }
        }
    }

    glutSwapBuffers();
}

//-----------------------------------------------------------------

void Keyboard_Function(unsigned char key, int x, int y){
    switch (key){

    case  27: exit(0);break; //ESC -> encerra aplicativo...
    case 'w':
    case 'W': wire =!wire;
        glutPostRedisplay();
        break;
    case 'z':
        pessoa3=0;
        glutPostRedisplay();
        break;
    case 'x':
        pessoa3=1;
        glutPostRedisplay();
        break;
    }

}
//-----------------------------------------------------------------

void Special_Function(int key, int x, int y){
    float rad;

    switch (key){
        case GLUT_KEY_DOWN:
            if(verificarMovimento(jog_x, jog_z, - mov_x, -mov_z)){
                jog_x -= mov_x;
                jog_z -= mov_z;
                g_esfera.x_pos=jog_x;
                g_esfera.z_pos=jog_z;
                light_position[0] = jog_x;
                light_position[2] = jog_z;
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
                glutPostRedisplay();
            }
            break;


        case GLUT_KEY_UP:
            if(verificarMovimento(jog_x, jog_z,  mov_x, mov_z)){
                jog_x += mov_x;
                jog_z += mov_z;
                g_esfera.x_pos=jog_x;
                g_esfera.z_pos=jog_z;
                light_position[0] = jog_x;
                light_position[2] = jog_z;
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
                glutPostRedisplay();
            }
            break;

        case GLUT_KEY_LEFT:
            angulo -= 10;
            if(angulo < 0)
                angulo +=360;
            rad = (float) (3.14159 * angulo / 180.0f);
            mov_x =  cos(rad) * PASSO;
            mov_z =  sin(rad) * PASSO;
            glutPostRedisplay();
            break;


        case GLUT_KEY_RIGHT:
            angulo += 10;
            if(angulo >= 360) angulo -=360;
            rad =  (float) (3.14159 * angulo / 180.0f);
            mov_x = cos(rad) * PASSO;
            mov_z = sin(rad) * PASSO;
            glutPostRedisplay();
            break;
        }//fim do switch

}

//-----------------------------------------------------------------

void Inicializa(void){
    glShadeModel(GL_SMOOTH);

    glClearColor(0.3, 0.6, 0.8, 0.0);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_branca);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_branca);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //Especifica sistema de coordenadas de projecao
    glMatrixMode(GL_PROJECTION);
    // Inicializa sistema de coordenadas de projecao
    glLoadIdentity();

    //Especifica a projecao perspectiva
    gluPerspective(90,1,0.1,3000);

    //Especifica sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);

    // Inicializa sistema de coordenadas de projecao
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    // inicializa numeros aleatorios
    srand(GetTickCount());

    //posicao inicial da esfera

    g_esfera.dir = LESTE;

}

//--------------------------------------------------------------

int main(int argc, char **argv){
    for(int i = 0 ; i < 100 ; i++ ){
        for(int j = 0 ; j < 100 ; j++){
            mapa[i][j]=0;
        }
    }
    int n, m;
    cin>>n>>m;
    for(int i = 0 ; i < n ; i++ ){
        for(int j = 0 ; j < m ; j++){
            cin>>mapa[i][j];
            if(mapa[i][j]==2){
                g_esfera.x_pos = i*TAM_BLOCO;
                g_esfera.z_pos = j*TAM_BLOCO;
                jog_x = i*TAM_BLOCO;
                jog_z = j*TAM_BLOCO;//*TAM_BLOCO;
                mapa[i][j]=4;
            }
        }
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Labirinto 3D");
    glutFullScreen();
    //glutGetWindow();

    Inicializa();

    glutDisplayFunc(display);
    glutKeyboardFunc(Keyboard_Function);
    glutSpecialFunc(Special_Function);
//    glutIdleFunc(Move_Esfera);

    glutMainLoop();
}



/*1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1*/

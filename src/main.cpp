//	Includes
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
//-----------------

//	Methods Signatures
void displayCallback(void);
void idleCallback(void);
//-----------------

void onKey(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'q':
  case '\e':
    exit(0);
    break;
  default:
    break; // Any other button will fire a bullet.
  }
  glutPostRedisplay();
}

int main(int argc, char **argr)
{
  glutInit(&argc, argr);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(800, 500);
  glutCreateWindow("Space thingy!");
  glutDisplayFunc(displayCallback);
  glutKeyboardFunc(onKey);
  glutIdleFunc(idleCallback);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  gluOrtho2D(0.0, 500.0, 0.0, 500.0);
  glutMainLoop();
  return 0;
}

void displayCallback(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

void idleCallback()
{
  glutPostRedisplay();
}

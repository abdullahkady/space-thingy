//	Includes
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sstream>
//-----------------

//	Methods Signatures
void displayCallback(void);
void idleCallback(void);
//-----------------
bool gameOver = false;
//	Global Variables
double bg1Y = 0;
double bg2Y = 0;
double spaceshipX = 200, spaceshipY = 70;
double missileX = 0, missileY = -100; // To be invisible by default
double enemyShotX = 0, enemyShotY = -100;
double enemyX = 0, enemyY = 380, enemyHealth = 100, enemyWIDTH = 35, enemyHEIGHT = 20, enemySpeed = 1, enemySpeedCounter = 0;
double speedPowerUpX = 0, speedPowerUpY = -100, speedPowerUpTimer = 0;
double playerSpeed = 4;
bool enemyIsHit = false;
//----------------

void writeToScreen(std::string text, float x, float y)
{
  glColor3f(0.0, 0.0, 0.0);
  glRasterPos3f(x, y, 0);

  for (int i = 0; i < text.length(); i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void drawPowerUps()
{
  // Speeds the player movement with a random factor, for a random period.
  // Thrusts turn green during boost time
  glPushMatrix();
  glPointSize(10.0);
  glColor3f(0, 1, 0);
  glBegin(GL_POINTS);
  glVertex2f(speedPowerUpX, speedPowerUpY);
  glEnd();
  glPopMatrix();
}

void drawBackground()
{
  glPushMatrix();

  glPushMatrix();
  glTranslated(0, bg1Y, 0);
  glColor3f(0.24, 0, 0.54);
  glBegin(GL_QUADS);
  glVertex2f(0, 0);
  glVertex2f(800, 0);
  glVertex2f(800, 500);
  glVertex2f(0, 500);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glTranslated(0, bg2Y, 0);
  glColor3f(0.24, 0.24, 0.54);
  glBegin(GL_QUADS);
  glVertex2f(0, -500);
  glVertex2f(800, -500);
  glVertex2f(800, 0);
  glVertex2f(0, 0);
  glEnd();
  glPopMatrix();

  // for (int i = 0; i < rand() % 30; i++)
  // {
  //   glBegin(GL_POINTS);
  //   glPointSize(rand() % 10);
  //   glColor3f(rand(), rand(), rand());
  //   glVertex2f(rand() % 800, rand() % 500);
  //   glEnd();
  // }
  glPopMatrix();
}

void drawHealthBar()
{
  glPushMatrix();
  // Workaround for to_string() error.
  // https://stackoverflow.com/a/12975966
  std::ostringstream tmp;
  tmp << enemyHealth;

  writeToScreen("Enemy health: " + tmp.str() + "\%", 10, 450);
  // The damage dealt (red base bar)
  glBegin(GL_QUADS);
  glColor3f(1, 0, 0);
  glVertex2f(10, 470);
  glVertex2f(10 + 100, 470);
  glVertex2f(10 + 100, 490);
  glVertex2f(10, 490);
  glEnd();

  // Remaining health (green bar)
  glBegin(GL_QUADS);
  glColor3f(0, 1, 0);
  glVertex2f(10, 470);
  glVertex2f(10 + enemyHealth, 470);
  glVertex2f(10 + enemyHealth, 490);
  glVertex2f(10, 490);
  glEnd();
  glPopMatrix();
}

void drawSpaceship()
{
  glPushMatrix();
  glPointSize(10.0);
  glLineWidth(8.0);

  // FIRE BARREL THINGY?
  glBegin(GL_LINES);
  glColor3f(1, 0, 1);
  glVertex2f(spaceshipX - 10, spaceshipY + 40);
  glVertex2f(spaceshipX + 10, spaceshipY + 40);
  glEnd();

  // BODY
  glBegin(GL_QUADS);
  glColor3f(0.14, 0.42, 0.55);
  glVertex2f(spaceshipX - 25, spaceshipY + 25);
  glVertex2f(spaceshipX - 25, spaceshipY - 25);
  glVertex2f(spaceshipX + 25, spaceshipY - 25);
  glVertex2f(spaceshipX + 25, spaceshipY + 25);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0, 0, 0);
  glVertex2f(spaceshipX - 15, spaceshipY - 5);
  glVertex2f(spaceshipX + 15, spaceshipY - 5);
  glEnd();

  // EXHAUST
  glBegin(GL_POINTS);
  // Color the exhaust dynamically dependong on boost state
  glColor3f(speedPowerUpTimer > 0 ? 0 : 1, speedPowerUpTimer > 0 ? 1 : 0, 0);
  glVertex2f(spaceshipX - 10, spaceshipY - 30);
  glVertex2f(spaceshipX + 10, spaceshipY - 30);
  glEnd();

  // WINGS
  glBegin(GL_TRIANGLES);
  glColor3f(0.2, 0.8, 0.2);
  glVertex2f(spaceshipX + 25, spaceshipY + 25);
  glVertex2f(spaceshipX + 25, spaceshipY - 25);
  glVertex2f(spaceshipX + 45, spaceshipY - 35);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(0.2, 0.8, 0.2);
  glVertex2f(spaceshipX - 25, spaceshipY + 25);
  glVertex2f(spaceshipX - 25, spaceshipY - 25);
  glVertex2f(spaceshipX - 45, spaceshipY - 35);
  glEnd();

  // UPPER BODY
  glBegin(GL_QUADS);
  glColor3f(0.99, 0.84, 0);
  glVertex2f(spaceshipX - 25, spaceshipY + 25);
  glVertex2f(spaceshipX - 18, spaceshipY + 40);
  glVertex2f(spaceshipX + 18, spaceshipY + 40);
  glVertex2f(spaceshipX + 25, spaceshipY + 25);
  glEnd();
  glPopMatrix();
}

void drawEnemy()
{
  glColor3f(1, 0, 0);
  // Body //
  glPushMatrix();
  glTranslated(enemyX, enemyY, 0);
  glPushMatrix();
  glScalef(35, 10, 1);
  glutSolidSphere(1, 50, 50);
  glPopMatrix();
  // Body //

  glPushMatrix(); // Lights / Windows / whatever they are \_0_/
  glScalef(3, 3, 1);
  glTranslated(-20, 0, 0); // Base translation
  glColor3f(1, 1, 0);
  glTranslated(5, 0, 0);
  glColor3f(1, 1, 0);
  glTranslated(5, 0, 0);
  glColor3f(1, 0, 1);
  glutSolidSphere(1, 10, 10);
  glTranslated(5, 0, 0);
  glColor3f(0, 1, 1);
  glutSolidSphere(1, 10, 10);
  glTranslated(5, 0, 0);
  glColor3f(1, 1, 0);
  glutSolidSphere(1, 10, 10);
  glTranslated(5, 0, 0);
  glColor3f(0, 1, 0);
  glutSolidSphere(1, 10, 10);
  glTranslated(5, 0, 0);
  glColor3f(0, 1, 1);
  glutSolidSphere(1, 10, 10);
  glPopMatrix();
  glPopMatrix();

  glPushMatrix();
  glLineWidth(3.0);
  glColor3f(0, 0, 0);
  glBegin(GL_QUADS);
  glVertex2f(enemyX - 2, enemyY + 8);
  glVertex2f(enemyX + 2, enemyY + 8);
  glVertex2f(enemyX + 2, enemyY + 12);
  glVertex2f(enemyX - 2, enemyY + 12);
  glEnd();

  glColor3f(0.5, 0.5, 1);
  glBegin(GL_LINES);
  glVertex2f(enemyX, enemyY + 10);
  glVertex2f(enemyX - 25, enemyY + 25);

  glVertex2f(enemyX, enemyY + 10);
  glVertex2f(enemyX + 25, enemyY + 25);
  glEnd();
  glPopMatrix();
}

void drawMissile()
{
  glPushMatrix();
  glPointSize(10.0);
  glBegin(GL_QUADS);
  glColor3f(0.8, 0.8, 0.8);
  glVertex2f(missileX - 3, missileY - 10);
  glVertex2f(missileX + 5, missileY - 10);
  glVertex2f(missileX + 5, missileY + 10);
  glVertex2f(missileX - 3, missileY + 10);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3f(1, 0, 0);
  glVertex2f(missileX - 3, missileY + 10);
  glVertex2f(missileX + 5, missileY + 10);
  glVertex2f(missileX + 1, missileY + 18);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(0, 0, 1);
  glVertex2f(missileX - 2, missileY - 8);
  glVertex2f(missileX - 2, missileY - 15);
  glVertex2f(missileX - 10, missileY - 20);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(0, 0, 1);
  glVertex2f(missileX + 4, missileY - 8);
  glVertex2f(missileX + 4, missileY - 15);
  glVertex2f(missileX + 12, missileY - 20);
  glEnd();

  glPopMatrix();
  glutPostRedisplay();
}

void resetGame()
{
  spaceshipX = 200, spaceshipY = 70;
  missileX = 0, missileY = -100;
  enemyShotX = 0, enemyShotY = -100;
  enemyX = 0, enemyY = 380, enemyHealth = 100, enemyWIDTH = 35, enemyHEIGHT = 20, enemySpeed = 1, enemySpeedCounter = 0;
  enemyIsHit = false;
  bg1Y = 0;
  bg2Y = 0;
  speedPowerUpX = 0, speedPowerUpY = -100, speedPowerUpTimer = 0;
  playerSpeed = 4;
  gameOver = false;
}

void onKey(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'a':
    if (spaceshipX > 40)
      spaceshipX -= playerSpeed;
    break;
  case 'd':
    if (spaceshipX < 460)
      spaceshipX += playerSpeed;
    break;
  case 'r':
    if (gameOver)
      resetGame();
    break;
  case 'q':
  case '\e':
    exit(0);
    break;
  default: // Any other button will fire a bullet.
    missileY = spaceshipY + 55;
    missileX = spaceshipX;
  }
  glutPostRedisplay();
}

void drawEnemyShot()
{
  glPushMatrix();
  glPointSize(8.0);
  glBegin(GL_POINTS);
  glColor3f(1, 0, 0);
  glVertex2f(enemyShotX, enemyShotY);
  glColor3f(0, 0, 1);
  glVertex2f(enemyShotX, enemyShotY + 10);
  glEnd();
  glPopMatrix();
}

void dropTheBall() // Classic .. ¯\_(ツ)_/¯
{
  enemyShotX = enemyX;
  enemyShotY = enemyY;
}

void startInterval(int val)
{
  if (rand() % 50 < 5 && speedPowerUpY == -100 && speedPowerUpTimer <= 0)
  {
    speedPowerUpX = 150 + rand() % 300;
    speedPowerUpY = 300 + rand() % 150;
  }
  if (enemyShotY == -100) // Don't draw the shot if it's already being shot.
    dropTheBall();
  glutTimerFunc((rand() % 2) * 1000, startInterval, 0);
}

int main(int argc, char **argr)
{
  srand(time(NULL));
  glutInit(&argc, argr);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(800, 500);
  glutCreateWindow("Space thingy!");
  glutDisplayFunc(displayCallback);
  glutKeyboardFunc(onKey);
  glutIdleFunc(idleCallback);
  glutTimerFunc(2000, startInterval, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  gluOrtho2D(0.0, 500.0, 0.0, 500.0);
  glutMainLoop();
  return 0;
}

void displayCallback(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  if (!gameOver)
  {
    drawBackground();
    drawSpaceship();
    drawMissile();
    drawEnemy();
    drawEnemyShot();
    drawHealthBar();
    drawPowerUps();
  }
  else
  {
    if (enemyHealth <= 0)
      writeToScreen("You WON !", 200, 170);
    else
      writeToScreen("You lost in a 2D game, misrable just like your life", 200, 170);

    writeToScreen("Press R to restart, Q to quit", 200, 200);
  }

  glFlush();
}

void handlePowerUpsCollision()
{
  speedPowerUpY = (speedPowerUpY <= 0) ? -100 : speedPowerUpY - 0.1;
  if (speedPowerUpY != -100)
  {
    if (abs(speedPowerUpY - spaceshipY) < 20 && abs(speedPowerUpX - spaceshipX) < 40)
    {
      playerSpeed = 4 + rand() % 10;
      speedPowerUpTimer = 5 + rand() % 10;
      speedPowerUpY = -100;
    }
  }

  if (playerSpeed != 4)
  {
    speedPowerUpTimer -= 0.001;
    if (speedPowerUpTimer <= 0)
    {
      speedPowerUpTimer = 0;
      playerSpeed = 4;
    }
  }
}

void idleCallback()
{
  handlePowerUpsCollision();

  bg1Y = (bg1Y >= 500) ? 0 : bg1Y + 0.1;
  bg2Y = (bg2Y >= 500) ? 0 : bg2Y + 0.1;

  if (enemyX > 460 || enemyX < 20)
    enemyX = 20;
  if (enemySpeedCounter > 100)
  {
    enemySpeedCounter = 0;
    enemySpeed = (rand() % 100) * 0.0001;
  }
  enemySpeedCounter++;
  enemyX += enemySpeed;
  if (missileY > 0 && missileY < 600)
  {
    if (abs(missileY - enemyY) < enemyHEIGHT && abs(missileX - enemyX) < enemyWIDTH)
    {
      if (!enemyIsHit)
      {
        missileY = -100;
        enemyIsHit = true;
        enemyHealth -= 10;
        if (enemyHealth == 0)
          gameOver = true;
      }
    }
    else
    {
      enemyIsHit = false;
    }
    missileY += 0.1;
  }

  if (enemyShotY > 0)
  {
    enemyShotY -= 0.1;
    if (abs(enemyShotY - spaceshipY) < 20)
    {
      if (abs(enemyShotX - spaceshipX) < 40)
        gameOver = true;
    }
  }
  else // Clear the shot from the ground.
    enemyShotY = -100;
  glutPostRedisplay();
}

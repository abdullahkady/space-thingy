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
// Quick macro for rounding decimals
#define ROUNDF(f, c) (((float)((int)((f) * (c))) / (c)))

//	Methods Signatures
void displayCallback(void);
void idleCallback(void);
//-----------------
bool gameOver = false;
int gameScore = 0;
//	Global Variables
double bg1Y = 0;
double bg2Y = 0;
double bg1Points[30][2];
double bg2Points[30][2];

double spaceshipX = 200, spaceshipY = 70, spaceshipRotation = 0;
double missileX = 0, missileY = -100, missileSpeed = 4; // To be invisible by default
double enemyShotX = 0, enemyShotY = -100;
double enemyX = 0, enemyY = 380, enemyHealth = 100, enemyWIDTH = 35, enemyHEIGHT = 20;
double enemyDefenderX = -100, enemyDefenderY = 300, enemyDefenderShotX = 0, enemyDefenderShotY = -100;
double speedPowerUpX = 0, speedPowerUpY = -100, speedPowerUpTimer = 0;
double missileSpeedPowerUpX = 0, missileSpeedPowerUpY = -100, missileSpeedPowerUpTimer = 0;
double playerSpeed = 5;
//----------------

// BEZIER //
int p0[2] = {30, 380};
int p1[2];
int p2[2];
int p3[2] = {550, 380};
float t = 0;
// BEZIER //

int *bezier(float t, int *p0, int *p1, int *p2, int *p3)
{
  int *res = (int *)(malloc(sizeof(int) * 2));
  res[0] = pow((1 - t), 3) * p0[0] + 3 * t * pow((1 - t), 2) * p1[0] + 3 * pow(t, 2) * (1 - t) * p2[0] + pow(t, 3) * p3[0];
  res[1] = pow((1 - t), 3) * p0[1] + 3 * t * pow((1 - t), 2) * p1[1] + 3 * pow(t, 2) * (1 - t) * p2[1] + pow(t, 3) * p3[1];
  return res;
}

void writeToScreen(std::string text, float x, float y, float r, float g, float b)
{
  glColor3f(r, g, b);
  glRasterPos3f(x, y, 0);

  for (int i = 0; i < text.length(); i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void weirdCircleFailureShape(float cx, float cy, float r)
{
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < 10; i++)
  {
    float theta = 2.0f * 3.1415926f * float(i) / 10;
    float x = r * cosf(theta);
    float y = r * sinf(theta);
    glVertex2f(x + cx, y + cy);
  }
  glEnd();
}

void actualKindOfCircle(float cx, float cy, float r)
{
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i < 15; i++)
  {
    float theta = 2.0f * 3.1415926f * float(i) / 15;
    float x = r * cosf(theta);
    float y = r * sinf(theta);
    glVertex2f(x + cx, y + cy);
  }
  glEnd();
}

void drawPowerUpStatus()
{
  glColor3f(0, 1, 0);
  weirdCircleFailureShape(345, 475, 5);
  writeToScreen("Player Speed Boost:", 355, 470, 1, 1, 1);
  if (playerSpeed == 5)
  {
    writeToScreen("OFF", 465, 470, 1, 0, 0);
  }
  else
  {
    std::ostringstream tmp;
    tmp << speedPowerUpTimer;
    writeToScreen(tmp.str(), 465, 470, 0, 1, 0);
  }

  glColor3f(0, 1, 1);
  actualKindOfCircle(345, 445, 7);
  glPointSize(7);
  glBegin(GL_POINTS);
  glColor3f(1, 0, 0);
  glVertex2f(345, 445);
  glEnd();
  writeToScreen("Missile Speed Boost:", 355, 440, 1, 1, 1);
  if (missileSpeed == 4)
  {
    writeToScreen("OFF", 465, 440, 1, 0, 0);
  }
  else
  {
    std::ostringstream tmp;
    tmp << missileSpeedPowerUpTimer;
    writeToScreen(tmp.str(), 465, 440, 0, 1, 0);
  }
}

void drawPowerUps()
{
  // Speeds the player movement, for a random period.
  // Thrusts turn green during boost time
  glPushMatrix();
  glColor3f(0, 1, 0);
  weirdCircleFailureShape(speedPowerUpX, speedPowerUpY, 5);
  glPopMatrix();

  // Speeds the player missile shooting.
  // Missile tip turns green while being boosted.
  glPushMatrix();
  glColor3f(0, 1, 1);
  actualKindOfCircle(missileSpeedPowerUpX, missileSpeedPowerUpY, 7);
  glPointSize(7);
  glBegin(GL_POINTS);
  glColor3f(1, 0, 0);
  glVertex2f(missileSpeedPowerUpX, missileSpeedPowerUpY);
  glEnd();
  glPopMatrix();

  drawPowerUpStatus();
}

void generateBG1Points()
{
  for (int i = 0; i < 30; i++)
  {
    bg1Points[i][0] = rand() % 500;
    bg1Points[i][1] = rand() % 500;
  }
}
void generateBG2Points()
{
  for (int i = 0; i < 30; i++)
  {
    bg2Points[i][0] = rand() % 500;
    bg2Points[i][1] = -500 + rand() % 500;
  }
}

void drawBackground()
{
  glPushMatrix();

  glPushMatrix();
  glTranslated(0, bg1Y, 0);
  glColor3f(0.16, 0.11, 0.24);
  glBegin(GL_QUADS);
  glVertex2f(0, 0);
  glVertex2f(800, 0);
  glVertex2f(800, 501);
  glVertex2f(0, 501);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glTranslated(0, bg2Y, 0);
  glColor3f(0.11, 0.15, 0.24);
  glBegin(GL_QUADS);
  glVertex2f(0, -500);
  glVertex2f(800, -500);
  glVertex2f(800, 0);
  glVertex2f(0, 0);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glPointSize(3);
  glTranslated(0, bg1Y, 0);
  glBegin(GL_POINTS);
  glColor3f(0.65, 0.65, 0.65);
  for (int i = 0; i < 30; i++)
  {
    glVertex2f(bg1Points[i][0], bg1Points[i][1]);
  }
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glTranslated(0, bg2Y, 0);
  glBegin(GL_POINTS);
  glColor3f(0.65, 0.65, 0.65);
  for (int i = 0; i < 30; i++)
  {
    glVertex2f(bg2Points[i][0], bg2Points[i][1]);
  }
  glEnd();
  glPopMatrix();

  glPopMatrix();
}

void drawScore()
{
  glPushMatrix();
  std::ostringstream tmp;
  tmp << gameScore;
  writeToScreen("Current score: " + tmp.str(), 10, 430, 1, 1, 1);
  glPopMatrix();
}

void drawHealthBar()
{
  glPushMatrix();
  // Workaround for to_string() error.
  // https://stackoverflow.com/a/12975966
  std::ostringstream tmp;
  tmp << ROUNDF(enemyHealth, 100);

  writeToScreen("Enemy health: " + tmp.str() + "\%", 10, 450, 1, 1, 1);
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
  glTranslatef(spaceshipX, spaceshipY, 0);
  glRotatef(spaceshipRotation, 0, 0, 1);
  glScaled(0.5, 0.5, 1);
  glTranslatef(-spaceshipX, -spaceshipY, 0);

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

void drawEnemyDefender()
{
  // DEFENDER SHOT //
  glPushMatrix();
  glPointSize(7.0);
  glBegin(GL_POINTS);
  glColor3f(0, 0, 0);
  glVertex2f(enemyDefenderShotX, enemyDefenderShotY);
  glColor3f(1, 1, 1);
  glVertex2f(enemyDefenderShotX, enemyDefenderShotY - 10);
  glEnd();
  glPopMatrix();
  // DEFENDER SHOT //

  // THE DEFENDER //
  glPushMatrix();
  glColor3f(0, 0, 0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(enemyDefenderX, enemyDefenderY);
  glVertex2f(enemyDefenderX + 20, enemyDefenderY + 30);
  glVertex2f(enemyDefenderX - 25, enemyDefenderY - 20);
  glVertex2f(enemyDefenderX + 30, enemyDefenderY + 20);
  glVertex2f(enemyDefenderX - 10, enemyDefenderY + 25);
  glEnd();
  glPopMatrix();
  // THE DEFENDER //
}

void drawEnemy()
{
  if (enemyY < -50) // Enemy is currently respawning.
    writeToScreen("Nice job, wait for it ...", 170, 250, 1, 1, 1);

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
  glColor3f((missileSpeed == 4) ? 1 : 0, (missileSpeed == 4) ? 0 : 1, 0);
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
  gameOver = false;
  gameScore = 0;
  bg1Y = 0;
  bg2Y = 0;
  spaceshipX = 200, spaceshipY = 70, spaceshipRotation = 0;
  missileX = 0, missileY = -100, missileSpeed = 4; // To be invisible by default
  enemyShotX = 0, enemyShotY = -100;
  enemyX = 0, enemyY = 380, enemyHealth = 100, enemyWIDTH = 35, enemyHEIGHT = 20;
  enemyDefenderX = -100, enemyDefenderY = 300, enemyDefenderShotX = 0, enemyDefenderShotY = -100;
  speedPowerUpX = 0, speedPowerUpY = -100, speedPowerUpTimer = 0;
  missileSpeedPowerUpX = 0, missileSpeedPowerUpY = -100, missileSpeedPowerUpTimer = 0;
  playerSpeed = 5;
  t = 0;
}

void onKey(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'a':
    if (spaceshipX > 20)
    {
      spaceshipRotation = 15;
      spaceshipX -= playerSpeed;
    }
    break;
  case 'd':
    if (spaceshipX < 480)
    {
      spaceshipRotation = -15;
      spaceshipX += playerSpeed;
    }
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
    if (missileY < 0)
    {
      missileY = spaceshipY + 55;
      missileX = spaceshipX;
    }
  }
  glutPostRedisplay();
}

void drawEnemyShot()
{
  glPushMatrix();
  glLineWidth(7.0);
  glBegin(GL_LINES);
  glColor3f(0, 0, 0);
  glVertex2f(enemyShotX, enemyShotY);
  glVertex2f(enemyShotX, enemyShotY - 10);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3f(1, 1, 1);
  glVertex2f(enemyShotX, enemyShotY);
  glVertex2f(enemyShotX + 5, enemyShotY + 20);
  glVertex2f(enemyShotX - 5, enemyShotY + 20);
  glEnd();
  glPopMatrix();
}

void dropTheBall() // Classic .. ¯\_(ツ)_/¯
{
  enemyShotX = enemyX;
  enemyShotY = enemyY;
}

void powerUpInterval(int val)
{
  if ((rand() % 30) > 15 && speedPowerUpY == -100 && speedPowerUpTimer <= 0)
  {
    speedPowerUpX = 150 + rand() % 300;
    speedPowerUpY = 450;
  }
  if ((rand() % 35) < 15 && missileSpeedPowerUpY == -100 && missileSpeedPowerUpTimer <= 0)
  {
    missileSpeedPowerUpX = 150 + rand() % 300;
    missileSpeedPowerUpY = 450;
  }
  glutTimerFunc(1000, powerUpInterval, 0);
}

void enemyDefenderInterval(int val)
{
  if (enemyDefenderX == -100)
    enemyDefenderX = 0;
  glutTimerFunc((rand() % 5) * 1000, enemyDefenderInterval, 0);
}

void enemyDefenderShotInterval(int val)
{
  if (enemyDefenderY > 0 && enemyDefenderShotY < 0)
  {
    enemyDefenderShotX = enemyDefenderX;
    enemyDefenderShotY = enemyDefenderY;
  }
  glutTimerFunc(500, enemyDefenderShotInterval, 0);
  // Will drop a new shot whenever the first one hits the bottom
}

void enemyShotInterval(int val)
{
  if (enemyShotY == -100) // Don't draw the shot if it's already being shot.
    dropTheBall();
  glutTimerFunc(500, enemyShotInterval, 0);
  // Will drop a new shot whenever the first one hits the bottom
}

int main(int argc, char **argr)
{
  srand(time(NULL));
  generateBG1Points();
  generateBG2Points();
  glutInit(&argc, argr);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(800, 500);
  glutCreateWindow("Space thingy!");
  glutDisplayFunc(displayCallback);
  glutKeyboardFunc(onKey);
  glutIdleFunc(idleCallback);
  glutTimerFunc(1000, enemyShotInterval, 0);
  glutTimerFunc(2000, powerUpInterval, 0);
  glutTimerFunc(2000, enemyDefenderInterval, 0);
  glutTimerFunc(2000, enemyDefenderShotInterval, 0);
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
    drawEnemyDefender();
    drawEnemyShot();
    drawHealthBar();
    drawScore();
    drawPowerUps();
  }
  else
  {
    std::ostringstream tmp;
    tmp << gameScore;
    writeToScreen("Playing a 2d game, how lifeless? \\_(-.-)_/", 150, 220, 0, 0, 0);
    writeToScreen("Total score: " + tmp.str(), 150, 250, 0, 0, 0);
    writeToScreen("Press R to restart, Q to quit", 150, 280, 0, 0, 0);
  }

  glFlush();
}

void handlePowerUpsCollision()
{
  speedPowerUpY = (speedPowerUpY <= 0) ? -100 : speedPowerUpY - 0.1;
  missileSpeedPowerUpY = (missileSpeedPowerUpY <= 0) ? -100 : missileSpeedPowerUpY - 0.1;

  if (missileSpeedPowerUpY != -100)
  {
    if (abs(missileSpeedPowerUpY - spaceshipY) < 20 && abs(missileSpeedPowerUpX - spaceshipX) < 40)
    {
      missileSpeed = 7;
      missileSpeedPowerUpTimer = 4 + rand() % 10;
      missileSpeedPowerUpY = -100;
    }
  }

  if (missileSpeed != 4)
  {
    missileSpeedPowerUpTimer -= 0.001;
    if (missileSpeedPowerUpTimer <= 0)
    {
      missileSpeedPowerUpTimer = 0;
      missileSpeed = 4;
    }
  }

  if (speedPowerUpY != -100)
  {
    if (abs(speedPowerUpY - spaceshipY) < 20 && abs(speedPowerUpX - spaceshipX) < 40)
    {
      playerSpeed = 10;
      speedPowerUpTimer = 5 + rand() % 10;
      speedPowerUpY = -100;
    }
  }

  if (playerSpeed != 5)
  {
    speedPowerUpTimer -= 0.001;
    if (speedPowerUpTimer <= 0)
    {
      speedPowerUpTimer = 0;
      playerSpeed = 5;
    }
  }
}

void handleEnemyMovement()
{

  int *point = bezier(t, p0, p1, p2, p3);
  t += 0.0002;

  if (t >= 1)
  {
    t = 0;
    p0[0] = p3[0];
    p0[1] = p3[1];

    p3[0] = rand() % 500;
    p3[1] = rand() % 400;
    p1[0] = rand() % 500;
    p1[1] = rand() % 400;
    p2[0] = rand() % 500;
    p2[1] = rand() % 400;
  }

  if (p3[1] < 250)
  {
    p3[1] += 220;
  }
  if (p1[1] < 250)
  {
    p1[1] += 220;
  }
  if (p2[1] < 250)
  {
    p2[1] += 220;
  }

  enemyX = point[0];
  if (enemyY < -50) // Enemy is currently respawning
  {
    enemyY += 0.01;
  }
  else
    enemyY = point[1];

  // Lock him to the borders.
  if (enemyX > 460)
    enemyX = 460;
  else if (enemyX < 20)
    enemyX = 20;
}

void handleEnemyDefender()
{
  // MOVEMENT //
  if (enemyDefenderX > 480)
    enemyDefenderX = -100;
  if (enemyDefenderX != -100)
    enemyDefenderX += 0.15;
  // MOVEMENT //

  // SHOTS BLOCKING //
  if (missileY > 0)
  {
    if (abs(missileY - enemyDefenderY) < 20 && abs(missileX - enemyDefenderX) < 35)
    {
      missileY = -100;
    }
  }
  // SHOTS BLOCKING //

  // SHOTS COLLISON //
  if (enemyDefenderY > 0)
  {
    enemyDefenderShotY -= 0.15;
    if (abs(enemyDefenderShotY - spaceshipY) < 10)
    {
      if (abs(enemyDefenderShotX - spaceshipX) < 20)
        gameOver = true;
    }
  }
  else // Clear the shot from the ground.
    enemyDefenderShotY = -100;
  // SHOTS COLLISON //
}

void idleCallback()
{
  handlePowerUpsCollision();
  handleEnemyDefender();

  if (bg1Y >= 500)
  {
    bg1Y = -500;
    generateBG1Points();
  }
  else
    bg1Y += 0.1;

  if (bg2Y >= 1000)
  {
    bg2Y = 0;
    generateBG2Points();
  }
  else
    bg2Y += 0.1;

  handleEnemyMovement();

  if (missileY > 520)
    missileY = -100;
  if (missileY > 0)
  {
    if (abs(missileY - enemyY) < enemyHEIGHT && abs(missileX - enemyX) < enemyWIDTH)
    {
      missileY = -100;
      double factorOfSpawns = (gameScore + 100) / 100;
      enemyHealth -= 10 / factorOfSpawns;
      gameScore += 10;
      if (enemyHealth <= 0)
      {
        // The enemy is killed this way, and will be respawned
        // when enemyY reaches -50 (in the handleMovement)
        enemyY = -100;
        enemyHealth = 100;
      }
    }
    missileY += (missileSpeed / 10);
  }

  if (enemyShotY > 0)
  {
    enemyShotY -= 0.2;
    if (abs(enemyShotY - spaceshipY) < 10)
    {
      if (abs(enemyShotX - spaceshipX) < 20)
        gameOver = true;
    }
  }
  else // Clear the shot from the ground.
    enemyShotY = -100;
  glutPostRedisplay();
}

#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <stdio.h>
#include "noise.h"

void terre_init(void);
//static void resize(int width, int height);
void terre_draw(void);
void terre_sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _sphereId = 0;
static GLuint _pId[4] = {0};
static GLuint _texId_terre[4] = {0};
// 0 : texture ciel étoilé
// 1 : texture terre
// 2 : texture lune
// 3 : texture étoiles

void terre_init(void) {

  initNoiseTextures();
  glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // couleur d'effacement

  // création des objets
  _sphereId = gl4dgGenSpheref(20, 20);
  // pid shader
  _pId[0] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/etoiles.fs", NULL); // ciel
  _pId[1] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/soleil.fs", NULL); // soleil
  _pId[2] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/terre.fs", NULL); // terre 
  _pId[3] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/lune.fs", NULL); // lune

  // activation du test de profondeur
  glEnable(GL_DEPTH_TEST);

  // matrice projection, model view projection
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "proj");
  //resize(_wW, _wH); // on resize car sinon faut bouger la fenêtre pour que l'écran apparaisse
}

static void resize(void) {
  GLfloat ratio;
  GLint data[4];
  glGetIntegerv(GL_VIEWPORT, data);
  _wW = data[2];
  _wH = data[3];
  ratio = _wW / ((GLfloat)_wH); // ratio d'écran
  gl4duBindMatrix("proj"); // active (met en current) la matrice liée au nom
  gl4duLoadIdentityf();
  // adaptation de l'écran au ratio
  gl4duFrustumf(-1 * ratio, 1 * ratio, -1 , 1 , 2, 100);  
}


static void draw_with_perlin(int pid_number, float object_scale, float zoom) {

  gl4duRotatef(90, 0, 1, 0);
  gl4duScalef(object_scale, object_scale, object_scale);   // le diamètre de la lune est d'environ 1/3 de celui
  gl4duSendMatrices(); // envoie les matrices de translation et rotation

  useNoiseTextures(_pId[pid_number], 0);
  gl4dgDraw(_sphereId);
  unuseNoiseTextures(0);

  float ancienne_taille = 1 / object_scale;
  gl4duScalef(ancienne_taille, ancienne_taille,
              ancienne_taille);
  
  gl4duRotatef(-90, 0, 1, 0);
}
static void scene(GLfloat a) {
  GLfloat blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
  gl4duBindMatrix("mod"); // model, car on veut agir sur l\'objet
  gl4duLoadIdentityf();   // chargement de matrice identité dans la matrice en cours càd model

  // ciel étoilé
  glUseProgram(_pId[0]);
  draw_with_perlin(0, 20.0f, 5.5f);

  // soleil
  glUseProgram(_pId[1]);
  draw_with_perlin(1, 1.0, 35.5f);

  // terre
  glUseProgram(_pId[2]);
  gl4duRotatef(a, 0, 1, 0);
  gl4duScalef(0.3f, 0.3f, 0.3f);
  gl4duTranslatef(10.0f, 0, 0);
  draw_with_perlin(2, 1, 7.5f);
  glUseProgram(_pId[3]);

  // lune
  gl4duScalef(0.3f, 0.3f, 0.3f);
  gl4duRotatef(a * 10, 0, 1, 0);
  gl4duRotatef(a, 1, 0, 0);
  gl4duTranslatef(5.0f, 0, 0);
  draw_with_perlin(3, 1.0, 3.5);

  glUseProgram(0);
}

  void terre_draw(void) {

    // permet d'obtenir le décalage de temps depuis la dernière frame
    static double t0 = 0.0;
    double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
    t0 = t;

    // a -> angle
    // xyz -> position de l'oeil
    static GLfloat a = 0.0f,z = 0.01f;

    glEnable(GL_DEPTH_TEST);
    resize();

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT); // on clear le buffer de couleur, et de profondeur

    glViewport(0, 0, _wW, _wH); // rend la page adaptative

    // on positionne la caméra
    gl4duBindMatrix("view");
    gl4duLoadIdentityf();
    gl4duLookAtf(5.6f, 0.0f, z, 0, 0, 0, 0, 1.0f, 0);

    // on dessine la scène, en fonction de l'angle
    scene(a);
    a += 0.5f * dt;
    z += 0.025f * dt;
    glViewport(0, 0, _wW, _wH);
  }

  void terre_sortie(void) {
    // libération de la mémoire
    glDeleteTextures(4, _texId_terre);
  }

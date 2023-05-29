#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <stdio.h>

void saturne_init(void);
void saturne_draw(void);
void saturne_sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _sphereId = 0;
static GLuint _anneauId = 0;
static GLuint _pId[3] = {0};
static GLuint _texId_saturne[4] = {0};

void saturne_init(void) {
  initNoiseTextures();
  glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // couleur d'effacement

  // création des objets
  _sphereId = gl4dgGenSpheref(20, 20);
  _anneauId = gl4dgGenTorusf(60, 2, 0.1f);
  // pid shader
  _pId[0] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/etoiles.fs", NULL); // ciel
  _pId[1] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/saturne.fs", NULL);
  _pId[2] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/anneau.fs", NULL);

  // activation du test de profondeur
  glEnable(GL_DEPTH_TEST);

  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
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
  gl4duFrustumf(-1 * ratio, 1 * ratio, -1, 1, 2, 100);
}

// supprimer le deuxième argument

void draw_sphere_with_perlin(float object_scale, int pid) {

  gl4duRotatef(90, 0, 1, 0);
  gl4duScalef(object_scale, object_scale, object_scale);   // le diamètre de la lune est d'environ 1/3 de celui
  gl4duSendMatrices(); // envoie les matrices de translation et rotation
  useNoiseTextures(_pId[pid], 0);
  gl4dgDraw(_sphereId); 
  unuseNoiseTextures(0);

  float ancienne_taille = 1 / object_scale;
  gl4duScalef(ancienne_taille, ancienne_taille,
              ancienne_taille);
  
  gl4duRotatef(-90, 0, 1, 0);
}
void draw_anneau_with_perlin() {
  int pid_number = 2;
  gl4duRotatef(90, 0, 1, 0);
  float object_scale = 2.5f;
  gl4duScalef(object_scale, object_scale, object_scale);   // le diamètre de la lune est d'environ 1/3 de celui
  gl4duSendMatrices(); // envoie les matrices de translation et rotation
  useNoiseTextures(_pId[0], 0);
  gl4dgDraw(_anneauId); // dessine le tore
  unuseNoiseTextures(0);

  float ancienne_taille = 1 / object_scale;
  gl4duScalef(ancienne_taille, ancienne_taille,
              ancienne_taille);
  
  gl4duRotatef(-90, 0, 1, 0);
}

void scene() {
  GLfloat blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
  gl4duBindMatrix("mod"); // model, car on veut agir sur l\'objet
  gl4duLoadIdentityf();   // chargement de matrice identité dans la matrice en
                          // cours càd model
  glUseProgram(_pId[0]); // quel programme va être utilisé ? pId -> .vs fs

  /* glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, */
  /*              blanc); */
  // création du ciel étoilé
  draw_sphere_with_perlin(20.0f, 0);

  glUseProgram(_pId[1]); // quel programme va être utilisé ? pId -> .vs fs
  // création de saturne
  draw_sphere_with_perlin(1.0f, 1);

  glUseProgram(_pId[2]); // quel programme va être utilisé ? pId -> .vs fs
  // création de l'anneeau de saturne
  draw_anneau_with_perlin();


  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void saturne_draw(void) {

  // permet d'obtenir le décalage de temps depuis la dernière frame
  static double t0;

  static int t0_init = 0;
  if (!(t0_init)) { // permet de réinitialisé le temps, pour pas qu'on soit dans le futur quand on démarre l'animation
    t0_init = 1;
    t0 = (gl4dGetElapsedTime() / 1000.0);
  }
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  static GLfloat  x = 3.0f, y = -3.0f, z = 0.01f;
  
  glEnable(GL_DEPTH_TEST);
  resize();
  glClear(
      GL_COLOR_BUFFER_BIT |
      GL_DEPTH_BUFFER_BIT); // on clear le buffer de bouleur, et de profondeur

  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("view");
  gl4duLoadIdentityf(); 
  gl4duLookAtf(x, y, z, 0, 0, 0, 0, 1.0f, 0); 

  scene();
  // permet les mouvement de caméra
  x += 0.3f * dt;
  y += 0.3f * dt;
  z += 0.5f * dt;
  glViewport(0, 0, _wW, _wH);
}

void saturne_sortie(void) {
  glDeleteTextures(1, _texId_saturne);
}

#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <stdio.h>
#include <unistd.h>

void systeme_solaire_init(void);
void systeme_solaire_draw(void);
void systeme_solaire_sortie(void);

static GLuint _wW , _wH;
static GLuint _sphereId = 0;
static GLuint _anneauId = 0;
static GLuint _pId[6] = {0};

static GLuint _texId_systeme_solaire[6] = {0};





void systeme_solaire_init(void) {


  glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // couleur d'effacement

  // création des objets
  _sphereId = gl4dgGenSpheref(90, 90);
  _anneauId = gl4dgGenTorusf(60, 2, 0.1f);
  // pid shader
  _pId[0] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/etoiles.fs", NULL);
  _pId[1] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/lune.fs", NULL);
  _pId[2] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/terre.fs", NULL);
  _pId[3] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/saturne.fs", NULL);
  _pId[4] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/anneau.fs", NULL);
  _pId[5] = gl4duCreateProgram("<vs>shaders/terre.vs", "<fs>shaders/soleil.fs", NULL);

  // activation du test de profondeur
  glEnable(GL_DEPTH_TEST);

  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");

  //resize(_wW, _wH);
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

static void draw_with_perlin_noise(GLuint object_id, float object_scale, float distance) {

  gl4duTranslatef(0, 0, -distance);

  gl4duScalef(object_scale, object_scale,
              object_scale);

  gl4duSendMatrices();
  useNoiseTextures(_pId[object_id], 0);
  gl4dgDraw(object_id);
  unuseNoiseTextures(0);

  
  float ancienne_taille = 1/object_scale;
  gl4duScalef(ancienne_taille, ancienne_taille,
              ancienne_taille); 
}
static void scene(GLfloat a) {
  GLfloat  blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
  gl4duBindMatrix("mod"); // model, car on veut agir sur l'objet
  gl4duLoadIdentityf();   // chargement de matrice identité dans la matrice en
                          // cours càd model
  // ciel
  glUseProgram(_pId[0]); 
  draw_with_perlin_noise(_sphereId, 50.0f, 0);

  float ecart_deux_astre = 0.2f;
  float diviseur = 50.0f; // nécessaire, car je peux pas faire un ciel trop grand, donc je rapetisse les éléments

  // lune, 1 737,4 km de rayon équatorial
  glUseProgram(_pId[1]); 
  draw_with_perlin_noise(_sphereId, 1.7374f/diviseur, 0);
  float distance = 1.7374f/diviseur + ecart_deux_astre;

  // terre, 6378 km : rayon équatorial
  distance += 6.378f / diviseur + ecart_deux_astre;
  glUseProgram(_pId[2]);
  draw_with_perlin_noise(_sphereId, (6.378f / diviseur), distance);

  // saturne, rayon équatorial : 60268f km
  glUseProgram(_pId[3]);
  distance += 2 * (60.268f / diviseur) + ecart_deux_astre;
  draw_with_perlin_noise(_sphereId, 60.268f / diviseur, distance);
  // anneau
  glUseProgram(_pId[4]);
  draw_with_perlin_noise(_anneauId, 2*60.268f/diviseur, 0);
  distance += 696.342f / diviseur + ecart_deux_astre;

  // dessin soleil,  696342 de rayon équatorial
  glUseProgram(_pId[5]);
  draw_with_perlin_noise(_sphereId, 696.342f / diviseur, distance);

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void systeme_solaire_draw(void) {

  // permet d'obtenir le décalage de temps depuis la dernière frame
  static double t0;
  static int t0_init = 0;
  if (!(t0_init)){
    t0_init = 1;
    t0 = gl4dGetElapsedTime() / 1000.0;
  }
  static double total_time = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  total_time += t0;
  static GLfloat a = 0.0f, x = 1.01f, y = 1.0f, z = +0.5;
  glEnable(GL_DEPTH_TEST);
  resize();

  glClear(
      GL_COLOR_BUFFER_BIT |
      GL_DEPTH_BUFFER_BIT); // on clear le buffer de bouleur, et de profondeur

  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("view");
  gl4duLoadIdentityf(); 
  gl4duLookAtf(-x, y, z, 0, 0, z, 0, 1.0f, 0); 

  scene(a);
    z -= 0.5f*
         dt; // on garde en champs de vision tout les astres grâce à ce recul
    x += 1.0 * dt;
  glViewport(0, 0, _wW, _wH);
}

void systeme_solaire_sortie(void) {
  // libération mémoire
  glDeleteTextures(6, _texId_systeme_solaire);
}

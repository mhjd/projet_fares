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
static GLuint _pId = 0;
static GLuint _texId_saturne[4] = {0};

void charge_texture() {

  // ma texture :
  // mes fichiers images de texture
  static char *images[] = {"img/2k_saturn_ring_alpha_transparence.png",
                           "img/lune.jpg", "img/8k_stars_milky_way.jpg",
                           "img/8k_saturn.jpg"};

  SDL_Surface *t;
  glGenTextures(4, _texId_saturne);
  for (int i = 0; i < 4; i++) {
    glBindTexture(GL_TEXTURE_2D, _texId_saturne[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if ((t = IMG_Load(images[i])) != NULL) {
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode,
                   GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file  : %s\n", SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, NULL);
    }
  }
}
void saturne_init(void) {




  glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // couleur d'effacement

  // création des objets
  _sphereId = gl4dgGenSpheref(20, 20);
  _anneauId = gl4dgGenTorusf(60, 2, 0.1f);
  // pid shader
  _pId = gl4duCreateProgram("<vs>shaders/saturne.vs", "<fs>shaders/saturne.fs", NULL);

  // activation du test de profondeur
  glEnable(GL_DEPTH_TEST);

  gl4duGenMatrix(GL_FLOAT, "proj_ninja3");
  gl4duGenMatrix(GL_FLOAT, "mod_ninja3");
  gl4duGenMatrix(GL_FLOAT, "view_ninja3");
  charge_texture();
  //resize(_wW, _wH);
}

static void resize(void) {
  GLfloat ratio;
  GLint data[4];
  glGetIntegerv(GL_VIEWPORT, data);
  _wW = data[2];
  _wH = data[3];
  ratio = _wW / ((GLfloat)_wH); // ratio d'écran
  gl4duBindMatrix("proj_ninja3"); // active (met en current) la matrice liée au nom
  gl4duLoadIdentityf();
  // adaptation de l'écran au ratio
  gl4duFrustumf(-1 * ratio, 1 * ratio, -1, 1, 2, 100);
}

void draw_object(int object_tex_id, int sky, GLuint object_id,
                 float object_scale) {

  gl4duScalef( object_scale,  object_scale,
               object_scale); 

  gl4duSendMatrices(); // envoie les matrices de translation et rotation
  glBindTexture(GL_TEXTURE_2D, _texId_saturne[object_tex_id]);
  glUniform1i(glGetUniformLocation(_pId, "sky"), sky);
  gl4dgDraw(object_id);
  gl4duScalef(1 / object_scale, 1 / object_scale, 1 / object_scale);
}

void scene() {
  GLfloat blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
  gl4duBindMatrix("mod_ninja3"); // model, car on veut agir sur l\'objet
  gl4duLoadIdentityf();   // chargement de matrice identité dans la matrice en
                          // cours càd model
  glUseProgram(_pId); // quel programme va être utilisé ? pId -> .vs fs

  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1,
               blanc);
  // création du ciel étoilé
  draw_object(2, 1, _sphereId, 20.0f);

  // création de saturne
  draw_object(3, 0, _sphereId, 1.0f);

  // création de l'anneeau de saturne
  draw_object(0, 0, _anneauId, 2.5f);


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
  gl4duBindMatrix("view_ninja3");
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

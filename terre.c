#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <stdio.h>

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

/*int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Système solaire", GL4DW_POS_CENTERED,
GL4DW_POS_CENTERED, _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init(); // initialisation
  atexit(sortie); // libération de la mémoire
  gl4duwResizeFunc(resize); // affecte la fonction appelée lors du resize.
  gl4duwDisplayFunc(draw);  // affecte la fonction appelée lors de l'affichage.
  gl4duwMainLoop(); // main loop
  return 0;
}*/

static void charge_texture(void){
  // partie texture :
  // mes fichiers images de texture
  static char *images[] = {"img/terre.jpg", "img/lune.jpg",
                           "img/8k_stars_milky_way.jpg", "img/soleil.jpg"};

  // ce qui suit est inspirer d'un fichier sample que j'avais trouvé, vu que je savais pas comment faire pour coller une texture à un objet
  // je précise également que je me suis beaucoup aider des vidéos moodle donc il y a sûrement des parties ressemblante
  SDL_Surface *t;
  // on va généré 4 textures
  glGenTextures(4, _texId_terre);
  for (int i = 0; i < 4; i++){

    // on bind la texture actuelle
    glBindTexture(GL_TEXTURE_2D, _texId_terre[i]);
    // paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // on essaye de load l'image
    if ((t = IMG_Load(images[i])) != NULL) {
      // si c'est en rgb ou rgba
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

      /* // on relie l'image au texid */
      /* if (i == 0){ */
      /*   GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };   */
      /*   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, mode, */
      /*                GL_UNSIGNED_BYTE, color); */
        
      /* } else { */
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode,
                  GL_UNSIGNED_BYTE, t->pixels);
      /* } */
      // on libère la mémoire de t
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "on ne peut pas ouvrir le fichier : %s\n",  SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
}
void terre_init(void) {

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
  charge_texture();
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


static void draw_object(int object_tex_id, int sky, int sun, float object_scale ) {

  gl4duScalef(
      object_scale, object_scale,
      object_scale);   // le diamètre de la lune est d'environ 1/3 de celui
  gl4duSendMatrices(); // envoie les matrices de translation et rotation
  glBindTexture(GL_TEXTURE_2D, _texId_terre[object_tex_id]);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1i(glGetUniformLocation(_pId, "sky"), sky);
  glUniform1i(glGetUniformLocation(_pId, "sun"), sun);
  gl4dgDraw(_sphereId); // dessine le tore

  float ancienne_taille = 1 / object_scale;
  gl4duScalef(ancienne_taille, ancienne_taille,
              ancienne_taille);
}

static void scene(GLfloat a) {
  GLfloat blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
  gl4duBindMatrix("mod"); // model, car on veut agir sur l\'objet
  gl4duLoadIdentityf();   // chargement de matrice identité dans la matrice en cours càd model
  glUseProgram(_pId[0]);     // quel programme va être utilisé ? pId -> .vs .fs
  /* glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, */
  /*              blanc); // envoie couleur blanche en lumière aux shaders */
  //création du ciel étoilé
  draw_object(2, 1, 0, 20.0f);

  glUseProgram(_pId[1]);
  /* glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, */
  /*              blanc); // envoie couleur blanche en lumière aux shaders */
  // création du soleil
  draw_object(3, 0, 1, 1.0f);

  glUseProgram(_pId[2]);
  /* glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, */
  /*              blanc); // envoie couleur blanche en lumière aux shaders */
  // création de la terre
  gl4duRotatef(a, 0, 1, 0);
  gl4duScalef(0.3f, 0.3f, 0.3f);
  gl4duTranslatef(10.0f, 0, 0);
  draw_object(0, 0, 0, 1);

  glUseProgram(_pId[3]);
  /* glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, */
  /*              blanc); // envoie couleur blanche en lumière aux shaders */
  // création de la lune
  gl4duScalef(0.3f, 0.3f, 0.3f);
  gl4duRotatef(a * 10, 0, 1, 0);
  gl4duRotatef(a, 1, 0, 0);
  gl4duTranslatef(5.0f, 0, 0);
  draw_object(1, 0, 0, 1);

  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
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

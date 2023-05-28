/*!\file animations.h
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 05, 2014
 */
#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void transition_fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void animation_flash(int state);
  extern void animation_vide(int state);
  extern void animation_damier(int state);
  extern void animation_terre(int state);
  extern void animation_saturne(int state);
  extern void animation_systeme_solaire(int state);
  extern void animation_credits(int state);
  extern void animationsInit(void);
    /* Dans base.c */
    extern void base_init(void);
    extern void base_draw(void);
    /* Dans bases3D.c */
    extern void bases3D_init(void);
    extern void bases3D_draw(void);
    extern void bases3D_sortie(void);
    /* Dans terre.c*/

    extern void terre_init(void);
    extern void terre_draw(void);
    extern void terre_sortie(void);

  /*Dans saturne.c*/
  extern void saturne_init(void);
  extern void saturne_draw(void);
  extern void saturne_sortie(void);
  /*Dans systeme_solaire.c */
  extern void systeme_solaire_init(void);
  extern void systeme_solaire_draw(void);
  extern void systeme_solaire_sortie(void);

  /* Dans credits.c*/

  extern void credits_init(void);
  extern void credits_draw(void);
  extern void credits_quit(void);

#ifdef __cplusplus
}
#endif

#endif

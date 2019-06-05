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

  extern void assimpInit(const char * filename);
  extern void assimpDrawScene(void);
  extern void assimpQuit(void);

  extern void assimpInit1(const char * filename);
  extern void assimpDrawScene1(void);
  extern void assimpQuit1(void);

  extern void initNoiseTextures(void);
  extern void useNoiseTextures(GLuint pid, int shift);
  extern void unuseNoiseTextures(int shift);
  extern void freeNoiseTextures(void);



  extern void transition_fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void animation_flash(int state);
  extern void animation_vide(int state);
  extern void animation_damier(int state);
  extern void animation_base(int state);
  extern void animation_earth(int state);
  extern void animationsInit(void);
  extern void animation_laby(int state);
  extern void animation_Arrte(int state);
  extern void animation_Newcity(int state);
  /* Dans base.c */
  extern void initDataL2(void);
  extern void drawLaby2(void);
  extern void idle2(void);
  /* Dans earth.c */
  extern void idleNatu(void);
  extern void initNatu(void);
  extern void drawNatu(void);
  // extern void earth_init(void);
  // extern void earth_draw(void);


  extern void initDataL(void);
  extern void drawLaby(void);
  extern void idle(void);




  extern void text_animation(int state);
#ifdef __cplusplus
}
#endif

#endif

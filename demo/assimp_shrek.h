/*!\file assimp.h
 *
 * \brief fonctionalités pour utilisation de lib Assimp sous GL4Dummies.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 14, 2017
 */

#ifndef _ASSIMP_H

#define _ASSIMP_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void assimpInit_Shrek(const char * filename);
  extern void assimpDrawScene_Shrek(void);
  extern void assimpQuit_Shrek(void);
  
#ifdef __cplusplus
}
#endif

#endif

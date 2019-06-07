#include <assert.h>
#include <SDL_ttf.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dh.h>
#include "animations.h"

static void drawmefirstcall(void);
static void draw_text(void);
static void (*_drawmefunc)(void) = drawmefirstcall;

static GLuint _textTexId = 0, _pId = 0, _quad = 0;
static Uint32 _t0 = 0;

static void init_text(void) {

  SDL_Color c = {255, 0, 0, 0};
  SDL_Surface * d, * s;
  TTF_Font * font = NULL;
  _pId = gl4duCreateProgram("<vs>shaders/text.vs", "<fs>shaders/text.fs", NULL);
  _quad = gl4dgGenQuadf();
  glGenTextures(1, &_textTexId);
  glBindTexture(GL_TEXTURE_2D, _textTexId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  if( !(font = TTF_OpenFont("public/police/DejaVuSans-Bold.ttf", 256)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  d = TTF_RenderUTF8_Blended_Wrapped(font,  "Shrek "
     "est un livre écrit et illustré par William Steig. \n\n"
     "L'ouvrage paraît en 1990 et raconte l'histoire d'un jeune ogre "
     "qui a trouvé l'ogresse de ses rêves, après avoir "
     "quitté sa maison pour découvrir le monde . ", c, 2048);
  if(d == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors du TTF_RenderText\n");
    return;
  }
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  if(s) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else exit(0);
  TTF_CloseFont(font);

  glBindTexture(GL_TEXTURE_2D, 0);

  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
}


static void drawmefirstcall(void) {
  _t0 = SDL_GetTicks();
  _drawmefunc = draw_text;
  draw_text();
}

static void draw_text(void) {

  int vp[4];
  GLuint t = SDL_GetTicks();
  const GLfloat inclinaison = -40.0;
  GLfloat dt = (t - _t0) / 1000.0, d = -1.4 + dt / 7.0;
  glGetIntegerv(GL_VIEWPORT, vp);
  //w = vp[2] - vp[0];
  //h = vp[3] - vp[1];
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 0);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textTexId);
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  gl4duBindMatrix("projectionMatrix");
  gl4duPushMatrix(); {
    gl4duLoadIdentityf();
    gl4duFrustumf(-1, 1, -1, 1, 1, 100);
    gl4duBindMatrix("modelViewMatrix");
    gl4duPushMatrix(); {
      gl4duLoadIdentityf();
      gl4duScalef(3, 3, 2);
      gl4duTranslatef(0.1, d * cos(inclinaison * M_PI / 140.0), -2 + d * sin(inclinaison * M_PI / 140.0));
      gl4duRotatef(inclinaison, 1, 0, 0);
      gl4duSendMatrices();
    } gl4duPopMatrix();
    gl4duBindMatrix("projectionMatrix");
  } gl4duPopMatrix();
  gl4duBindMatrix("modelViewMatrix");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl4dgDraw(_quad);
  glDisable(GL_BLEND);
  glUseProgram(0);
}

static void free_text(void) {
  if(_textTexId)
    glDeleteTextures(1, &_textTexId);
}

void animation_text(int state) {
  switch(state) {
    case GL4DH_INIT:
      init_text();
      return;
    case GL4DH_FREE:
      free_text();
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      return;
    default: /* S_DRAW */
      _drawmefunc();
      return;
  }
}


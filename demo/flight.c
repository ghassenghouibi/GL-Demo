#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

extern void assimpInit_flight(const char * filename);
extern void assimpDrawScene_flight(void);
extern void assimpQuit_flight(void);


 void resize2(int w, int h);


/* from makeLabyrinth.c */
extern unsigned int * labyrinth(int w, int h);

/*!\brief opened window width and height */
static int _wW = 1250, _wH = 750;
/*!\brief mouse position (modified by pmotion function) */
// static int _xm = 600, _ym = 300;
/*!\brief labyrinth to generate */
static GLuint * _labyrinth = NULL;
/*!\brief labyrinth side */
static GLuint _lab_side = 15;
/*!\brief Quad geometry Id  */
static GLuint _plane = 0;
/*!\brief Cube geometry Id  */
static GLuint _cube = 0;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
static GLuint _pId2 = 0;
//static GLuint _pId3 = 0;
/*!\brief plane texture Id */
static GLuint _planeTexId = 0;
static GLuint Tex = 0;
static GLuint Tex1 = 0;
static GLuint TexS2 = 0;
//static GLuint TexS3 = 0;
//static GLuint _sphere = 0;//bounding sphere
static GLuint _sphere1 = 0;

/*!\brief compass texture Id */
static GLuint _compassTexId = 0;

/*!\brief plane scale factor */
static GLfloat _planeScale = 100.0f;

 void initGL2(void) {
  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  _pId  = gl4duCreateProgram("<vs>shaders/laby.vs", "<fs>shaders/laby.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);

  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  
}

/*
*!\brief initializes data :
* creates 3D objects (plane and sphere) and 2D textures.
*/

 void initDataL2(void) {
  initGL2();
  /* a red-white texture used to draw a compass */
  GLuint northsouth[] = {(255 << 24) + 255, -1};
  /* generates a quad using GL4Dummies */
  _plane = gl4dgGenQuadf();
  /* generates a cube using GL4Dummies */
  _cube = gl4dgGenCubef();

  _sphere1 = gl4dgGenSpheref(20,10);



  assimpInit_flight("public/model/NCC-1701/NCC-1701.obj");

  /* creation and parametrization of the plane texture */
  glGenTextures(1, &_planeTexId);
  glBindTexture(GL_TEXTURE_2D, _planeTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  _labyrinth = labyrinth(_lab_side, _lab_side);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _lab_side, _lab_side, 0, GL_RGBA, GL_UNSIGNED_BYTE, _labyrinth);

  /* creation and parametrization of the compass texture */
  glGenTextures(1, &_compassTexId);
  glBindTexture(GL_TEXTURE_2D, _compassTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, northsouth);



    SDL_Surface * t;
    glGenTextures(1, &Tex);

    glBindTexture(GL_TEXTURE_2D, Tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load("public/images/rock.png")) != NULL ) {
#ifdef __APPLE__
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_RGB;
#else
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't %s\n", SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }


    SDL_Surface * t1;
    glGenTextures(1, &Tex1);
    glBindTexture(GL_TEXTURE_2D, Tex1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t1 = IMG_Load("public/images/rock.jpg")) != NULL ) {
#ifdef __APPLE__
      int mode = t1->format->BytesPerPixel == 4 ? GL_BGRA : GL_RGB;
#else
      int mode = t1->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t1->w, t1->h, 0, mode, GL_UNSIGNED_BYTE, t1->pixels);
      SDL_FreeSurface(t1);
    } else {
      fprintf(stderr, "can't %s\n", SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    SDL_Surface * t2;
    glGenTextures(1, &TexS2);
    glBindTexture(GL_TEXTURE_2D, TexS2);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t2 = IMG_Load("public/images/land_ocean_ice_2048.png")) != NULL ) {
#ifdef __APPLE__
      int mode = t2->format->BytesPerPixel == 4 ? GL_BGRA : GL_RGB;
#else
      int mode = t2->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t2->w, t2->h, 0, mode, GL_UNSIGNED_BYTE, t2->pixels);
      SDL_FreeSurface(t2);
    } else {
      fprintf(stderr, "can't %s\n", SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }





  glBindTexture(GL_TEXTURE_2D, 0);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
 void resize2(int w, int h) {
  _wW = w; 
  _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.05, 0.05, -0.05 * _wH / _wW, 0.05 * _wH / _wW, 0.05, _planeScale + 1.0);
  gl4duBindMatrix("modelViewMatrix");
}


static GLfloat _radius = 80, _x0 = 20, _z0 = -20, _y0 = 2.5;
/*!\brief paramètres de l'avion */
static GLfloat _x = 0, _y = 0, _z = 0, _alpha = 0;
void idle2(void) {
  double dt;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  _alpha -= 0.1f * dt;
  _x = _x0 + _radius * cos(_alpha);
  _z = _z0 - _radius * sin(_alpha);
  _y = _y0;
}


static GLfloat TmpSA = 400.0, TmpSAZ = 400.0;
static GLfloat yt = 80.0; 
/*!\brief function called by GL4Dummies' loop at draw.*/
 void drawLaby2(void) {
  // GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};
  /* clears the OpenGL color buffer and depth buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);



  /* sets the current program shader to _pId */
  glUseProgram(_pId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, TexS2);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);



  gl4duBindMatrix("viewMatrix");



  /* loads the identity matrix in the current GL4Dummies matrix ("viewMatrix") */
  gl4duLoadIdentityf();

  gl4duLookAtf(-0 , yt, 2,     -10, -yt, -_z,           -15.0, 1.0, -3.0);
      gl4duBindMatrix("modelMatrix");
      gl4duLoadIdentityf();
      gl4duPushMatrix(); {
        gl4duTranslatef(1, 16, 1);
        gl4duScalef(180, 180, 180);
        gl4duRotatef(140, 1, 1, 0);
        gl4duRotatef(-360*_alpha, 0,1, 0);
        gl4duSendMatrices();
      } gl4duPopMatrix();
  glDisable(GL_CULL_FACE);



  gl4dgDraw(_sphere1);


  
  if(yt > 800.0){
    yt = 1000.0;
    _radius += 1;
    TmpSA -= 0.5; 
    TmpSAZ -= 0.4;
  }
  else
    yt += 0.2; 


    // printf("%f\n", TmpSA);
  GLint i, j;
  /* pushs (saves) the current matrix (modelMatrix), scales, rotates,
   * sends matrices to pId and then pops (restore) the matrix */
  for (i = 0; i < _lab_side ; i++){
    for (j = 0; j < _lab_side; j++){
      gl4duPushMatrix(); {
        gl4duTranslatef(_planeScale - (_planeScale / _lab_side) - (_planeScale / _lab_side * 2 ) * i,0, _planeScale - (_planeScale / _lab_side) - (_planeScale / _lab_side * 2) * j);
        gl4duRotatef(-90, 1, 0, 0);
        gl4duScalef((_planeScale / _lab_side), 7.0,(_planeScale / _lab_side));
        gl4duSendMatrices();
      } gl4duPopMatrix();
      /* culls the back faces */
      // glCullFace(GL_BACK);
      /* uses the checkboard texture */
      glBindTexture(GL_TEXTURE_2D, Tex1);
      /* sets in pId the uniform variable texRepeat to the plane scale */
      glUniform1f(glGetUniformLocation(_pId, "texRepeat"), 1.0);
      /* draws the plane */
      gl4dgDraw(_plane);

    }
  }

  

  
  for (i = 8; i < _lab_side ; i++){
    for (j = 11; j < _lab_side; j++){
      gl4duPushMatrix(); {
        if(_labyrinth[j * _lab_side + (_lab_side-1-i)] == -1){
          gl4duTranslatef(_planeScale - (_planeScale / _lab_side) - (_planeScale / _lab_side * 2 ) * i,5.0, _planeScale - (_planeScale / _lab_side) - (_planeScale / _lab_side * 2) * j);
          gl4duScalef((_planeScale / _lab_side), 20.0,(_planeScale / _lab_side));
          gl4duSendMatrices();
        }
      } 
      gl4duPopMatrix();
      glBindTexture(GL_TEXTURE_2D, Tex);
      /* sets in pId the uniform variable texRepeat to the plane scale */
      glUniform1f(glGetUniformLocation(_pId, "texRepeat"), 1.0);
      /* draws the plane */
      gl4dgDraw(_cube);
   }
  }

  gl4duTranslatef(20, 200, 20);

  gl4duScalef(100,100,100);

  glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
  assimpDrawScene_flight();
  glUniform1i(glGetUniformLocation(_pId, "inv"), 0);

  /* enables cull facing and depth testing */
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}


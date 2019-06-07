/*!\file window.c
 *
 * \brief G�n�ration de paysage fractal � l'aide de l'algorithme de
 * d�placement des milieux. Un algorithme de midpoint displacement est
 * propos� dans GL4Dummies : le Triangle-Edge.
 *
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 07 2017
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL2/SDL_image.h>




extern void assimpInit(const char * filename);
extern void assimpDrawScene(void);
extern void assimpQuit(void);

extern void assimpInit(const char * filename);
extern void assimpDrawScene(void);
extern void assimpQuit(void);

extern void assimpInit(const char * filename);
extern void assimpDrawScene(void);
extern void assimpQuit(void);

/* fonctions externes dans noise.c */
extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);

// static void quit(void);
void initNatu(void);
static void resize(int w, int h);
void idleNatu(void);
// static void keydown(int keycode);
// static void keyup(int keycode);
void drawNatu(void);
static GLfloat heightMapAltitude(GLfloat x, GLfloat z);
void unObjet();

/*!\brief largeur de la fen�tre */
static int _windowWidth = 1024;
/*!\brief haiteur de la fen�tre */
static int _windowHeight =800;
/*!\brief largeur de la heightMap g�n�r�e */
static int _landscape_w = 1000;
/*!\brief heuteur de la heightMap g�n�r�e */
static int _landscape_h = 1000;
/*!\brief scale en x et z du mod�le de terrrain */
static GLfloat _landscape_scale_xz =250.0f;
/*!\brief scale en y du mod�le de terrain */
static GLfloat _landscape_scale_y = 10.0f;
/*!\brief heightMap du terrain g�n�r� */
static GLfloat * _heightMap = NULL;
/*!\brief identifiant d'une sph�re (soleil) */
static GLuint _sphere = 0;
/*!\brief identifiant du terrain g�n�r� */
static GLuint _landscape = 0;
static GLuint _landscape2 = 0;
//static GLuint _landscape3 = 0;
/*!\brief identifiant GLSL program du terrain */
static GLuint _landscape_pId  = 0;
static GLuint _landscape_pId2 = 0;
//static GLuint _landscape_pId3  = 0;
/*!\brief identifiant GLSL program du soleil */
static GLuint _sun_pId = 0;
/*!\brief identifiant de la texture de d�grad� de couleurs du terrain */
static GLuint _terrain_tId = 0;
//static GLuint _terrain_tId2 = 0;
//static GLuint _terrain_tId3 = 0;
/*!\brief identifiant de la texture de d�grad� de couleurs du soleil */
static GLuint _yellowred_tId = 0;
/*!\brief identifiant de la texture de plasma */
static GLuint _plasma_tId = 0;

static GLuint _sky_tId = 0;
static GLuint _pId2 = 0;


/* variable de cycle pour le plasma */
static GLfloat _cycle = 0.0;

static GLboolean _fog = GL_FALSE;

static GLuint _plan = 0;

static GLuint _pId = 0;
//static GLuint _bird_pId = 0;

GLfloat landscape_nuit_y;


static GLfloat  _alpha = 0;
//_x0 = 1, _z0 = 1, _y0 = 10, _radius = 30;



static char modelstree[]  ="public/model/tree/Tree.obj";
static char modelshrek[]  ="public/model/shrek/Shrek.obj";
static char modelfiona[]  ="public/model/PrincessFiona/fiona.obj";


/*!\brief indices des touches de clavier */
enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN
};

/*!\brief clavier virtuel */
// static GLuint _keys[] = {0, 0, 0, 0};

typedef struct cam_t cam_t;
/*!\brief structure de donn�es pour la cam�ra */
struct cam_t {
  GLfloat x, z;
  GLfloat theta;
};


static cam_t _cam = {0, 0, 0};

void initNatu(void) {
  SDL_Surface * t1;
  //SDL_Surface * t2;
  //SDL_Surface * t3;
  SDL_Surface * t4;
  SDL_Surface * tSky;

  /* pour g�n�rer une chaine al�atoire diff�rente par ex�cution */
  /* param�tres GL */
  glClearColor(0.0f, 0.4f, 0.5f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  /* chargement et compilation des shaders */
  _pId = gl4duCreateProgram("<vs>shaders/basic23.vs", "<fs>shaders/basic23.fs", NULL);
  _landscape_pId  = gl4duCreateProgram("<vs>shaders/basic3.vs", "<fs>shaders/basic3.fs", NULL);
  _landscape_pId2  = gl4duCreateProgram("<vs>shaders/basic3.vs", "<fs>shaders/basic3.fs", NULL);
  //_landscape_pId3  = gl4duCreateProgram("<vs>shaders/basic3.vs", "<fs>shaders/basic3.fs", NULL);
  _sun_pId        = gl4duCreateProgram("<vs>shaders/basic3.vs", "<fs>shaders/sun3.fs", NULL);
    _pId2 = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/clouds.fs", NULL);

  
  /* cr�ation des matrices de model-view et projection */
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  /* appel forc� � resize pour initialiser le viewport et les matrices */
  resize(_windowWidth, _windowHeight);
  /* cr�ation de la g�om�trie de la sph�re */
  _plan = gl4dgGenQuadf();
  _sphere = gl4dgGenSpheref(40, 40);

  /* g�n�ration de la heightMap */
  _heightMap = gl4dmTriangleEdge(_landscape_w, _landscape_h, 0.5);
  /* cr�ation de la g�om�trie du terrain en fonction de la heightMap */
  _landscape = gl4dgGenGrid2dFromHeightMapf(_landscape_w, _landscape_h, _heightMap);
  _landscape2 =gl4dgGenGrid2dFromHeightMapf(400, 400, gl4dmTriangleEdge(400, 400, 0.5));
  //_landscape3 =gl4dgGenGrid2dFromHeightMapf(1000, 1000, gl4dmTriangleEdge(1000, 1000, 0.5));

  /* cr�ation, param�trage, chargement et transfert de la texture
     contenant le d�grad� de couleurs selon l'altitude (texture 1D) */
  glGenTextures(1, &_terrain_tId);
  glBindTexture(GL_TEXTURE_1D, _terrain_tId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  t1 = IMG_Load("public/images/alt.png");
  assert(t1);
#ifdef __APPLE__
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, t1->w, 0, t1->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, t1->pixels);
#else
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, t1->w, 0, t1->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, t1->pixels);
#endif
  SDL_FreeSurface(t1);


    glGenTextures(1, &_sky_tId);
  glBindTexture(GL_TEXTURE_1D, _sky_tId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  tSky = IMG_Load("public/images/nuages.jpg");
  assert(tSky);
#ifdef __APPLE__
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, tSky->w, 0, tSky->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, tSky->pixels);
#else
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, tSky->w, 0, tSky->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, tSky->pixels);
#endif
  SDL_FreeSurface(tSky);

  glGenTextures(1, &_yellowred_tId);
  glBindTexture(GL_TEXTURE_1D, _yellowred_tId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  t4 = IMG_Load("public/images/lave.png");
  assert(t4); 

#ifdef __APPLE__
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, t4->w, 0, t4->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, t4->pixels);
#else
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, t4->w, 0, t4->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, t4->pixels);
#endif
  SDL_FreeSurface(t4);
  /* cr�ation, param�trage et transfert de la texture contenant la
     heightMap comme texture */
  glGenTextures(1, &_plasma_tId);
  glBindTexture(GL_TEXTURE_2D, _plasma_tId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _landscape_w, _landscape_h, 0, GL_RED, GL_FLOAT, _heightMap);
  glBindTexture(GL_TEXTURE_2D, 0);

  assimpInit(modelstree);
  assimpInit_Shrek(modelshrek);
  assimpInit_Fiona(modelfiona);

  unObjet();

   
  
}

typedef struct {
  GLfloat x,z;
}Objet;

float myRand() {
  return rand() / (RAND_MAX + 1.0);
}


Objet Objet1[18];
void unObjet(){
  for(int i=0;i<18;i++){
     Objet1[i].x=100.0 * (2.0 * myRand() - 1.0*myRand());
     Objet1[i].z=(100.0 * (2.0 * myRand() - 1.0*myRand()));
  }
}

/*!\brief param�trage du viewport OpenGL et de la matrice de
 * projection en fonction de la nouvelle largeur et heuteur de la
 * fen�tre */
static void resize(int w, int h) {
  glViewport(0, 0, _windowWidth = w, _windowHeight = h);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
}

/*!\brief r�cup�ration du delta temps entre deux appels */
static double get_dt(void) {
  static double t0 = 0, t, dt;
  t = gl4dGetElapsedTime();
  dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}



static GLfloat TmpSA = 1.0;
void idleNatu(void) {
  double dt, dtheta = M_PI;
  dt = get_dt();

  if(TmpSA < 415){
    _cam.z -= dt + 0.15;
    TmpSA++;
  }

  if(TmpSA == 415.0){
    _cam.z += dt + 0.15;
  }

    _cam.theta += dt * dtheta / 8;
 
}

static GLfloat pose[] = {-10, -50,
                          0, -90, 
                          5, 40,
                          15, -45,  
                          9, 28, 
                          -100, 45, 
                          50, 40,
                          100, -110, 
                          -50, 90,

                          15, 45,
                          -90, 50, 
                          -12, 40,
                          15, 70,  
                          9, -73, 
                          30, 45, 
                          30, -200,
                          -50, 50, 
                          40, 170,};


static GLfloat pos_characters[] = {
15, 45,
-90, 50};

                   
/*!\brief dessin de la frame */
void drawNatu(void) {


  int ym = 355;
  GLfloat temp[4] = {(10 - 1) * cos(_cycle), (10 - 1) * cos(_cycle), -900, 1.0}, lumpos[4], landscape_y, *mat;
  landscape_y = heightMapAltitude(_cam.x, _cam.z);
  landscape_nuit_y = landscape_y;
  temp[0]=800;
  temp[1] = (1000 - 1) * cos(_cycle);
  temp[2]= (1000 - 1) * cos(_cycle);
  glClearColor(0.0f, 0.7f, 0.9f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId2);


  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(_cam.x, landscape_y + 20.0, _cam.z, 
         _cam.x - sin(_cam.theta), landscape_y + 20.0 - (ym - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
         0.0, 1.0,0.0);
  /* utilisation du shader soleil */
  glUseProgram(_sun_pId);
  gl4duPushMatrix(); {
    gl4duTranslatef(temp[0], temp[1], temp[2]);
    gl4duScalef(15, 15, 15);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1f(glGetUniformLocation(_sun_pId, "cycle"), _cycle);
  glUniform1i(glGetUniformLocation(_sun_pId, "plasma"), 0);
  glUniform1i(glGetUniformLocation(_sun_pId, "lave"), 1);
    glUniform1i(glGetUniformLocation(_landscape_pId, "eau"), 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _plasma_tId);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, _yellowred_tId);
  gl4dgDraw(_sphere);
  glBindTexture(GL_TEXTURE_1D, 0);
  glActiveTexture(GL_TEXTURE0);
  // utilisation du shader de terrain
  glUseProgram(_landscape_pId);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mat, temp);
  gl4duScalef(_landscape_scale_xz, _landscape_scale_y, _landscape_scale_xz);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_landscape_pId, "lumpos"), 1, lumpos);
  glUniform1i(glGetUniformLocation(_landscape_pId, "degrade"), 0);
  glUniform1i(glGetUniformLocation(_landscape_pId, "fog"), _fog);
  glBindTexture(GL_TEXTURE_1D, _terrain_tId);
  gl4dgDraw(_landscape);


   for (int i=0;i<7;i++){

        glUseProgram(_pId);
        glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 0, lumpos);
        glUniform1i(glGetUniformLocation(_pId, "fog"), _fog);
        gl4duBindMatrix("modelViewMatrix");
        gl4duLoadIdentityf();
        gl4duLookAtf(_cam.x, landscape_y + 5.0, _cam.z, 
            _cam.x - sin(_cam.theta), landscape_y + 5.0 - (ym - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
            0.0, 1.0,0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _pId);
        gl4duTranslatef(pose[i], 0, pose[i+1]);
        gl4duRotatef(270,0,1,0);
        gl4duScalef(15, 30, 25);
        assimpDrawScene();

   }




        glUseProgram(_pId);
        glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 0, lumpos);
        glUniform1i(glGetUniformLocation(_pId, "fog"), _fog);
        gl4duBindMatrix("modelViewMatrix");
        gl4duLoadIdentityf();
        gl4duLookAtf(_cam.x, landscape_y + 5.0, _cam.z, 
            _cam.x - sin(_cam.theta), landscape_y + 5.0 - (ym - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
            0.0, 1.0,0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _pId);
        gl4duTranslatef(pos_characters[0], 0, pos_characters[1]);
        gl4duRotatef(270,0,1,0);
        gl4duScalef(15, 40, 50);
        assimpDrawScene_Shrek();

        glUseProgram(_pId);
        glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 0, lumpos);
        glUniform1i(glGetUniformLocation(_pId, "fog"), _fog);
        gl4duBindMatrix("modelViewMatrix");
        gl4duLoadIdentityf();
        gl4duLookAtf(_cam.x, landscape_y + 5.0, _cam.z, 
            _cam.x - sin(_cam.theta), landscape_y + 5.0 - (ym - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 
            0.0, 1.0,0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _pId);
        gl4duTranslatef(pos_characters[2], 0, pos_characters[3]);
        gl4duRotatef(270,0,1,0);
        gl4duScalef(15, 40, 50);
        assimpDrawScene_Fiona();

   




 
  int h=0;
  gl4duSendMatrices();


}


static GLfloat heightMapAltitude(GLfloat x, GLfloat z) {
  x = (_landscape_w >> 1) + (x / _landscape_scale_xz) * (_landscape_w / 2.0); 
  z = (_landscape_h >> 1) - (z / _landscape_scale_xz) * (_landscape_h / 2.0);
  if(x >= 0.0 && x < _landscape_w && z >= 0.0 && z < _landscape_h)
    return (2.0 * _heightMap[((int)x) + ((int)z) * _landscape_w] - 1) * _landscape_scale_y;
  return 0;
}

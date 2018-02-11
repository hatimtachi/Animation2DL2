#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"
#include <assert.h>
#include <stdlib.h>
#include <GL4D/gl4dg.h>
#include <SDL2/SDL_image.h>


static GLuint _quadId = 0;
static int ok=0,ctp=0,cttp=0,switche=0;
int rand_a_b(int a, int b) {
  return rand()%(b-a) +a;
}
void exemple_de_transition_01(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  int vp[4], i;
  GLint tId;
  static GLuint tex[2], pId;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    glGetIntegerv(GL_VIEWPORT, vp);
    glGenTextures(2, tex);
    for(i = 0; i < 2; i++) {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mix.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tex[0]) {
      glDeleteTextures(2, tex);
      tex[0] = tex[1] = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
    /* JOUER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[0],  0);
    if(a0) a0(state);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[1],  0);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tId,  0);
    glDisable(GL_DEPTH);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    if(et / (GLfloat)t > 1) {
      fprintf(stderr, "%d-%d -- %f\n", et, t, et / (GLfloat)t);
      exit(0);
    }
    glUniform1f(glGetUniformLocation(pId, "dt"), et / (GLfloat)t);
    glUniform1i(glGetUniformLocation(pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(pId, "tex1"), 1);
    gl4dgDraw(_quadId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
}

void exemple_d_animation_00(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Uint16 * s;
  static GLfloat c[4] = {0, 0, 0, 0};
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Uint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    if(l >= 8)
      for(i = 0; i < 4; i++)
	c[i] = s[i] / ((1 << 16) - 1.0);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glClearColor(c[0], c[1], c[2], c[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}
void exemple_d_animation_03(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Sint16 * s;
  GLint rect[4], tr, mr,rect2[4]={5,250,8,0};
  static int r = 0, or = 0;
  static GLuint screen_id;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    screen_id = gl4dpInitScreen();
    gl4dpUpdateScreen(NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 6);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    mr = MAX(200, 100) + 80;
    gl4dpSetScreen(screen_id);
    rect[0] =0 ;
    rect[1] = 0;
    rect[2] =1024;
    rect[3] =768;
    gl4dpSetColor(RGB(0, 0, 0));
    int i;
    if(or>=225){
     for(i=0;i<=80;i++){   
	gl4dpSetColor(RGB(rand(),rand(),rand()));
      }
    }
    gl4dpRect(rect);
    gl4dpSetColor(RGB(0,0,0));
    gl4dpFilledCircle(gl4dpGetWidth() >> 1, gl4dpGetHeight() >> 1,tr-25 );
    gl4dpSetColor(RGB(255,255,255));
    gl4dpFilledCircle(gl4dpGetWidth() >> 1, gl4dpGetHeight() >> 1,tr-50 );
    gl4dpSetColor(RGB(0, 0,0));
    gl4dpFilledCircle(gl4dpGetWidth() >> 1, gl4dpGetHeight() >> 1,tr-75 );
    gl4dpSetColor(RGB(255, 255,255));
    gl4dpFilledCircle(gl4dpGetWidth() >> 1, gl4dpGetHeight() >> 1,tr-100 );
    gl4dpSetColor(RGB(rand(), rand(),rand()));
    gl4dpFilledCircle(gl4dpGetWidth() >> 1, gl4dpGetHeight() >> 1,tr-140 );
    if(or>=225){
      gl4dpSetColor(RGB(0,0,0));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-25 );
      gl4dpSetColor(RGB(rand(), rand(),rand()));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-50);
      gl4dpSetColor(RGB(0,0,0));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-75 );
      gl4dpSetColor(RGB(rand(), rand(),rand()));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-100);
      gl4dpSetColor(RGB(0,0,0));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-125 );
      gl4dpSetColor(RGB(rand(), rand(),rand()));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-150);
      gl4dpSetColor(RGB(0,0,0));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-175 );
      gl4dpSetColor(RGB(rand(), rand(),rand()));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-200);
      gl4dpSetColor(RGB(0,0,0));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-225 );
      gl4dpSetColor(RGB(rand(), rand(),rand()));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-250);
      gl4dpSetColor(RGB(0,0,0));
      gl4dpFilledCircle((gl4dpGetWidth() >> 1), (gl4dpGetHeight() >> 1),tr-275 );
    }
    gl4dpUpdateScreen(rect);
    or = tr;
    return;
  }
}
void exemple_d_animation_04(int state) {//a garder
  /* INITIALISEZ VOS VARIABLES */
  int l, i,la=0;
  Sint16 * s;
  GLint tr;
  static GLint r = 0;
  static GLuint s1, s2;
  static GLfloat r1[] = {0.0, 0.0, 1.0, 1.0}, r2[] = {0, 0, 1, 1}, rf = 0.0;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    s1 = gl4dpInitScreen();
    s2 = gl4dpInitScreen();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(s1);
    gl4dpDeleteScreen();
    gl4dpSetScreen(s2);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 5);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    gl4dpSetScreen(s2);
    gl4dpClearScreen();
    if(rf<=6.){
      gl4dpSetColor(RGB(rand(), rand(), rand()));
      gl4dpCircle(gl4dpGetWidth() * 2 / 4 , gl4dpGetHeight() >> 2, tr);
      gl4dpSetColor(RGB(rand(), rand(), rand()));
      gl4dpCircle(gl4dpGetWidth() * 1 / 4, gl4dpGetHeight() >> 1, tr);
      gl4dpUpdateScreen(NULL);
      gl4dpCircle(gl4dpGetWidth() * 3 / 4, gl4dpGetHeight() >> 1, tr);
      gl4dpUpdateScreen(NULL);
      rf +=0.01;
    }
    else if(rf>=6.){
      gl4dpSetColor(RGB(rand(), rand(), rand()));
      gl4dpCircle(gl4dpGetWidth() * 2 / 4 , gl4dpGetHeight() >> 1, tr);
      rf+=2.;
    }
    
    int j=0;
    printf("rf;%.25lf\n",rf);
    for(i=0;i<=8;i++){
      j++;
      if(rf>=2500.){
	gl4dpSetColor(RGB(rand(), rand(), rand()));
	gl4dpFilledCircle(gl4dpGetWidth() * 1 / 4, gl4dpGetHeight() >> 1, tr);
	gl4dpUpdateScreen(NULL);
	gl4dpSetColor(RGB(rand(), rand(), rand()));
	gl4dpFilledCircle(gl4dpGetWidth() * 3 / 4 , gl4dpGetHeight() >> 1, tr);
	gl4dpUpdateScreen(NULL);
      }
      else if((rf<=2500)&&(la==0)){
	if(rf>=448.){
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 +5, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	}
	if(rf>=900.){
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 +10, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	}
	if(rf>=1500.){
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 , gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 +20, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 -15, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 -10, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 -5, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	  gl4dpSetColor(RGB(rand(), rand(), rand()));
	  gl4dpCircle(gl4dpGetWidth() * j / 4 +15, gl4dpGetHeight() >> 1, tr);
	  gl4dpUpdateScreen(NULL);
	}
	la=1;
      }
    }
    gl4dpSetScreen(s1);
    gl4dpClearScreen();
    gl4dpMap(s1, s2, r1, r2, rf);
    gl4dpUpdateScreen(NULL);
    return;
  }
}
/* exemple simpliste de recopie depuis une surface sdl vers le screen en cours */
static void copyFromSurface(SDL_Surface * s, int x0, int y0) {
  Uint32 * p, coul, ocoul;
  Uint8 rmix, gmix, bmix;
  double f1, f2;
  int x, y, maxx, maxy;
  assert(s->format->BytesPerPixel == 4); /* pour simplifier, on ne gère que le format RGBA */
  p = s->pixels;
  maxx = MIN(x0 + s->w, gl4dpGetWidth());
  maxy = MIN(y0 + s->h, gl4dpGetHeight());
  for(y = y0; y < maxy; y++) {
    for(x = x0; x < maxx; x++) {
      ocoul = gl4dpGetPixel(x, y);
      coul = p[(s->h - 1 - (y - y0)) * s->w + x - x0]; /* axe y à l'envers */
      f1 = ALPHA(coul) / 255.0; f2 = 1.0 - f1;
      rmix = f1 *   RED(coul) + f2 *   RED(ocoul);
      gmix = f1 * GREEN(coul) + f2 * GREEN(ocoul);
      bmix = f1 *  BLUE(coul) + f2 *  BLUE(ocoul);
      gl4dpSetColor(RGB(rmix, gmix, bmix));
      gl4dpPutPixel(x, y);
    }
  }
}
void exemple_d_animation_07(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Sint16 * s;
  GLint rect[4], tr, mr;
  static int r = 0, or = 0;
  static GLuint screen_id;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    screen_id = gl4dpInitScreen();
    gl4dpUpdateScreen(NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 6);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    printf("or-->%d\ntr-->%d",or,tr);
    mr = MAX(200, 100) + 80;
    gl4dpSetScreen(screen_id);
    rect[0] =0 ;
    rect[1] = 0;
    rect[2] =1024;
    rect[3] =768;
    gl4dpSetColor(RGB(0, 0, 0));
    int i;
    gl4dpRect(rect);
    for(i=0;i<=4000;i++){   
      gl4dpSetColor(RGB(rand(),rand(),rand()));
      gl4dpFilledCircle(rand_a_b(0,1204),rand_a_b(0,1024) ,tr-100 );
    }
    gl4dpUpdateScreen(rect);
    or = tr;
    return;
  }
}
void animationsInit(void) {
  if(!_quadId)
    _quadId = gl4dgGenQuadf();
}
void exemple_d_animation_08(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Sint16 * s;
  GLint rect[4], tr, mr;
  static int r = 0, or = 0;
  static GLuint screen_id;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    screen_id = gl4dpInitScreen();
    gl4dpUpdateScreen(NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 6);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    printf("or-->%d\ntr-->%d\n",or,tr);
    mr = MAX(200, 100) + 80;
    gl4dpSetScreen(screen_id);
    rect[0] =0 ;
    rect[1] = 0;
    rect[2] =1024;
    rect[3] =768;
    gl4dpSetColor(RGB(0, 0, 0));
    int i,se;
    gl4dpRect(rect);
    gl4dpSetColor(RGB(rand(),rand(),rand()));
    if(tr>=220)ok=1;
    if((tr>=100)&&(ok==0)){
      gl4dpFilledCircle(200,200 ,tr-130 );
      gl4dpFilledCircle(400,200 ,tr-130 );
      gl4dpFilledCircle(600,200 ,tr-130 );
      gl4dpFilledCircle(800,200 ,tr-130 );
      gl4dpFilledCircle(100,400 ,tr-130 );
      gl4dpFilledCircle(200,600 ,tr-130 );
      gl4dpFilledCircle(400,600 ,tr-130 );
      gl4dpFilledCircle(600,600 ,tr-130 );
      gl4dpFilledCircle(800,600 ,tr-130 );
      gl4dpFilledCircle(900,400 ,tr-130 );
    }
    if((tr>=150)&&(ok==1)){
      if((ctp==0)||(ctp==1)||(ctp==2)||(ctp==3)||(ctp==4)||(ctp==5)
	 ||(ctp==6)||(ctp==7)||(ctp==8)||(ctp==9)||(ctp==10)||(ctp==11))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

       gl4dpFilledCircle(800,200 ,tr-100 );

      if((ctp==12)||(ctp==13)||(ctp==14)||(ctp==15)||(ctp==16)||(ctp==17)
	 ||(ctp==18)||(ctp==19)||(ctp==20)||(ctp==21)||(ctp==22)||(ctp==23))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(600,200 ,tr-100 );

      if((ctp==24)||(ctp==25)||(ctp==26)||(ctp==27)||(ctp==28)||(ctp==29)
	 ||(ctp==30)||(ctp==31)||(ctp==32)||(ctp==33)||(ctp==34)||(ctp==35))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(400,200 ,tr-100 );

      if((ctp==36)||(ctp==37)||(ctp==38)||(ctp==39)||(ctp==40)||(ctp==41)
	 ||(ctp==42)||(ctp==43)||(ctp==44)||(ctp==45)||(ctp==46)||(ctp==47))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(200,200 ,tr-100 );

      if((ctp==48)||(ctp==49)||(ctp==50)||(ctp==51)||(ctp==52)||(ctp==53)
	 ||(ctp==54)||(ctp==55)||(ctp==56)||(ctp==57)||(ctp==58)||(ctp==59))
	gl4dpSetColor(RGB(80,0,0));
      else     
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(100,400 ,tr-100 );

      if((ctp==60)||(ctp==61)||(ctp==62)||(ctp==63)||(ctp==64)||(ctp==65)
	 ||(ctp==66)||(ctp==67)||(ctp==68)||(ctp==69)||(ctp==70)||(ctp==71))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(200,600 ,tr-100 );

     if((ctp==72)||(ctp==73)||(ctp==74)||(ctp==75)||(ctp==76)||(ctp==77)
	 ||(ctp==78)||(ctp==79)||(ctp==80)||(ctp==81)||(ctp==82)||(ctp==83))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(400,600 ,tr-100 );

      if((ctp==84)||(ctp==85)||(ctp==86)||(ctp==87)||(ctp==88)||(ctp==89)
	 ||(ctp==90)||(ctp==91)||(ctp==92)||(ctp==93)||(ctp==94)||(ctp==95))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(600,600 ,tr-100 );

     if((ctp==96)||(ctp==97)||(ctp==98)||(ctp==99)||(ctp==100)||(ctp==101)
	 ||(ctp==102)||(ctp==103)||(ctp==104)||(ctp==105)||(ctp==106)||(ctp==107))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(800,600 ,tr-100 );

     if((ctp==108)||(ctp==109)||(ctp==110)||(ctp==111)||(ctp==112)||(ctp==113)
	 ||(ctp==114)||(ctp==115)||(ctp==116)||(ctp==117)||(ctp==118)||(ctp==119))
	gl4dpSetColor(RGB(80,0,0));
      else
	gl4dpSetColor(RGB(255,255,255));

      gl4dpFilledCircle(900,400 ,tr-100 );

      ctp++;
      if(ctp==120)
	ctp=0;
      printf("ctp::%d\n",ctp);
    }
    gl4dpSetColor(RGB(rand(),rand(),rand()));
    if(tr<=100){
    gl4dpSetColor(RGB(0,0,0));
    }
    gl4dpFilledCircle(400,400 ,tr-40 );
    gl4dpFilledCircle(500,400 ,tr-40 );
    gl4dpFilledCircle(600,400 ,tr-40 );
    for(i=-200;i<=1024;i++){
      se=i;
      gl4dpSetColor(RGB(0,0, 0));
      gl4dpHLine(i, se+=30,rand_a_b(0,764));
    }
    gl4dpUpdateScreen(rect);
    or = tr;
    return;
  }
}

void exemple_d_animation_09(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Sint16 * s;
  GLint rect[4], tr,xr, mr,rect2[4]={5,250,8,0};
  static int r = 0, or = 0;
  static GLuint screen_id;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    screen_id = gl4dpInitScreen();
    gl4dpUpdateScreen(NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 6);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    printf("tr-->%d\n",tr);
    mr = MAX(200, 100) + 80;
    gl4dpSetScreen(screen_id);
    rect[0] =0 ;
    rect[1] = 0;
    rect[2] =1024;
    rect[3] =768;
    gl4dpSetColor(RGB(0, 0, 0));
    int i,j;
    gl4dpSetColor(RGB(0,0,0));
    gl4dpRect(rect);
    for(j=0;j<200;j++){
      gl4dpSetColor(RGB(rand(),rand(),rand()));      
      gl4dpCircle(rand()%gl4dpGetWidth(),rand()% gl4dpGetHeight(), j-100);
    }
    for(j=0;j<200;j++){
      gl4dpSetColor(RGB(255,255,255));      
	gl4dpCircle(rand()%gl4dpGetWidth(),rand()% gl4dpGetHeight(), tr-150);
    }
    gl4dpUpdateScreen(rect);
    or = tr;
    return;
  }
}

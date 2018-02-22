/*
Kummer Surfaces of a family of hyperelliptic curves

COMPILE gcc hyper-kummer.c -lGL -lglut -lGLU -lm
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>


#define DELTA_C 0.02

#define DELTA 0.025
#define xL -6
#define xR  6

#define KSCALE 0.20

#define CSCALEX 0.15
#define CSCALEY 0.015
#define CSCALEZ 1
#define CSCALE 1

#define TRANSKX 1
#define TRANSKY -0.5


#define TRANSCX 4
#define TRANSCY 0


double rot = 0;

typedef struct kpoint_s
{
  double A, B, K;
} kpoint;

typedef struct point_s
{
  double x, y;
} point;

typedef struct hcurve_s
{
  double a0, a1, a2, a3, a4;
  point *pts;
  int size;
} hcurve;


int Tval = 0;


hcurve
init_curve (double a0, double a1, double a2, double a3, double a4)
{
  static hcurve C;
  memset (&C, 0, sizeof (struct hcurve_s));
  C.pts = calloc (2 * (abs (xL) + abs (xR)) / DELTA, sizeof (struct point_s));
  C.a0 = a0;
  C.a1 = a1;
  C.a2 = a2;
  C.a3 = a3;
  C.a4 = a4;
  return C;
}

double
eval_fhcurve (double x, hcurve C)
{
  return x * x * x * x * x + C.a4 * x * x * x * x + C.a3 * x * x * x +
    C.a2 * x * x + C.a1 * x + C.a0;
}

void
calc_curve (hcurve * C)
{
  double x, y, f = 0;
  point T;
  int i = 0;
  for (x = xL; x <= xR; x += DELTA)
    {
      f = eval_fhcurve (x, *C);
      if (f >= 0)
	{
	  y = sqrt (f);
	  C->pts[i].x = x;
	  C->pts[i].y = y;
	  C->pts[i + 1].x = x;
	  C->pts[i + 1].y = -y;
	  C->size += 2;
	  i += 2;
	  glBegin (GL_POINTS);
	  glColor3f (1, 0, 0);
	  glVertex3f (CSCALE * (CSCALEX * x - TRANSCX),
		      CSCALE * (CSCALEY * y - TRANSCY), CSCALEZ * 0);
	  glVertex3f (CSCALE * (CSCALEX * x - TRANSCX),
		      CSCALE * (-CSCALEY * y - TRANSCY), CSCALEZ * 0);
	  glColor3f (1, 1, 1);
	  glEnd ();
	}

      //     printf("[%f] : %f, (%f,%f,%f,%f,%f)\n",x,f,C->a4,C->a3,C->a2,C->a1,C->a0);
    }
  return;
}


kpoint
kum_from_div (point P, point Q, hcurve C)
{
  kpoint S;
  S.A = P.x + Q.x;
  S.B = P.x * Q.x;
  S.K =
    (2 * C.a0 - C.a1 * S.A + 2 * C.a2 * S.B - C.a3 * S.A * S.B +
     2 * C.a4 * S.B * S.B - S.A * S.B * S.B - 2 * P.y * Q.y) / (S.A * S.A -
								4 * S.B);
  return S;
}

kpoint
kum_from_div2 (point P, point Q, hcurve C)
{
  double f1, f2;
  kpoint S;
  S.A = 0;
  S.B = 0;
  S.K = 0;
  f1 = eval_fhcurve (P.x, C);
  f2 = eval_fhcurve (Q.x, C);
  if ((f1 * f2) < 0)
    return S;
  S.A = -(P.x + Q.x);
  S.B = P.x * Q.x;
  S.K = sqrt (f1 * f2);
  return S;

}


void
gen_kummer (hcurve C)
{
  kpoint P;
  int i, j;
  point *hp = C.pts;
  glBegin (GL_POINTS);
  for (i = 0; i < 2 * C.size; i++)
    for (j = 0; j <= i; j++)

      if ((hp[i].x - hp[j].x) != 0)
	{
	  P = kum_from_div (hp[i], hp[j], C);
	  if ((P.A == 0) || (P.B == 0) || (P.K == 0))
	    continue;
	  else
	    {
	      glVertex3f (KSCALE * P.A - TRANSKX, KSCALE * P.B - TRANSKY,
			  KSCALE * P.K);
	      glVertex3f (KSCALE * P.A - TRANSKX, KSCALE * P.B - TRANSKY,
			  -KSCALE * P.K);
	    }
	}
  glEnd ();
  glutSwapBuffers ();
  if(Tval == 0) {
  glClear (GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT |
   GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
}


void
display (void)
{
  hcurve C;
  double t, s;

  glLoadIdentity ();
  glTranslatef (1.5f, 0.0f, -7.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
	   GL_STENCIL_BUFFER_BIT);


  if (Tval > 0)
    {
      C = init_curve (0.0, 5.0 * Tval, 0.0, -6.0, 0.0);
      calc_curve ((hcurve *) & C);
      glRotatef(rot,1.0,1.0,1.0);
      gen_kummer (C);
      glutSwapBuffers ();
    }


  else
    for (t = -5; t <= 5; t += DELTA_C)
      {
	s = t*t ;
	C = init_curve (0.0, 5.0 * t, 0.0, -3.0 * s, 0.0);
	
	calc_curve ((hcurve *) & C);
	gen_kummer (C);
      }
	rot += 0.25;
}

void
reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (50, (GLfloat) w / (GLfloat) h, 10.0, 2.0);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (0, 5, 5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int
main (int argc, char **argv)
{

  if (argc > 1)
    Tval = strtod (argv[1], NULL);

  srand (getpid () + time (NULL));
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH |
		       GLUT_MULTISAMPLE);
  glutInitWindowSize (1024, 768);
  glutInitWindowPosition (10, 10);
  glutCreateWindow ("test");
  glClearColor (0, 0, 0, 0);
  glShadeModel (GL_FLAT);
  glMatrixMode (GL_PROJECTION);
  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutIdleFunc (display);
  glutMainLoop ();
  return 0;
}

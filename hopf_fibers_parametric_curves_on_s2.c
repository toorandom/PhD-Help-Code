/*
 * Draws Hopf fibers of a family of parametric curves (lines) on the sphere, (for every point in each curve in the family you get a circle)
 * this is the hypersphere S3 in the space CxR which will be an stereographic projection from the space of real dimension 4
 * complex dimension 2.
 * Every point belongs to a circle in the space, and every circle does not touch the other (fiber)
 * toorandom@gmail.com
 * Beck
 *
 * I compile like this:
 *  /usr/bin/gcc hopf_fibers_parametric_curves_on_s2.c  -I /opt/X11/include/ -framework OpenGL -framework GLUT
 *
 *  Needs, libm, libglut, libGL, libGLU, of course it can be compiled in linux.
 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#define DELTA 0.01


#include <GL/glut.h>
#include <math.h>
float           rot = 0.0f;

double d = -5.0;
typedef struct point_R2_s
{
  double X, Y;
} point_R2;

typedef struct point_S2_s
{
  double x, y, z;
} point_S2;



#define DELTA_FIBRA 0.01
#define DELTA_CURVE 0.1
#define DELTA_PARAM 0.01
void
Fibra_Hopf (point_S2 p)
{
  double a, b, c, x, y, z, w, phi, theta, alpha, beta, r, k,s, x1,x2,x3,x4;
  a = p.x;
  b = p.y;
  c = p.z;
  s=3;
  alpha = sqrt ((1 + c) / 2);
  beta = sqrt ((1 - c) / 2);
  for (phi = 0; phi <= 2 * M_PI; phi += DELTA_FIBRA)
    {
      theta = atan (-a / b) - phi;
      w = alpha * cos (theta);
      r = acos (w) / M_PI;
      k = r / sqrt (1 - (w * w));
      x1 = w;
      x2 = k * alpha * sin (theta);
      x3 = k * beta * cos (phi);
      x4 = k * beta * sin (phi);
      glBegin(GL_POINTS);
     glVertex3f (s*x2, s*x3, s*x4);
     glVertex3f (-1*s*x2, -1*s*x3, -1*s*x4);
      glEnd();
    }
  return;
}



point_S2
SP_R2toS2 (point_R2 P)
{
  point_S2 S;
  S.x = 2.0 * P.X / (1.0 + (P.X * P.X) + (P.Y * P.Y));
  S.y = 2.0 * P.Y / (1.0 + (P.X * P.X) + (P.Y * P.Y));
  S.z =
    (-1.0 + (P.X * P.X) + (P.Y * P.Y)) / (1.0 + (P.X * P.X) + (P.Y * P.Y));
  return S;
}

point_R2
Rotate(point_R2 P,  double phi) {
point_R2 N;
N.X = P.X*cos(phi)-P.Y*sin(phi);
N.Y = P.X*sin(phi)+P.Y*cos(phi);
return N;
}

void
parametric_circles_from_curve_S2 (double t)
{
  double x, y,s;
  point_S2 S;
  point_R2 P,Q;

  //  for (P.X = 0.0; P.X < 2 * M_PI; P.X += DELTA)

    for(P.X=-2*M_PI; P.X < 2*M_PI;P.X+=DELTA_CURVE) 
      {
 	P.Y = t*P.X+(t-cos(P.X*P.X));
 	
	Q = P;
 //       for(s = 0 ; s< 2*M_PI;s+=DELTA_CURVE); {
	//P.X = s;
	//P.Y = t*s;
	S = SP_R2toS2 (Q);

	Fibra_Hopf(S);
/*
	printf ("%f %f %f\n", S.x, S.y, S.z);
	printf ("%f %f %f\n", P.X*t,P.Y,0.0);
*/
      }
  return;
}

void
display(void)
{
        float           origin, side;
        float t;
        struct timespec ts;
        ts.tv_sec=0;
        ts.tv_nsec = 50000000;
       // nanosleep(&ts,NULL);
        origin = 0;
        side = 2;
        glLoadIdentity();
        glTranslatef(1.5f, 0.0f, -7.0f);
        glRotatef(rot, 1.0, 1.0, 1.0);
       // glClear(GL_COLOR_BUFFER_BIT);
	glClear (GL_COLOR_BUFFER_BIT|GL_ACCUM_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glBegin(GL_POINTS);
        parametric_circles_from_curve_S2(d);
	glEnd();
        d+=DELTA_PARAM;
        //rot += 0.5; // velocidad
        glutSwapBuffers();
}
void
reshape(int w, int h)
{
        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(50, (GLfloat) w / (GLfloat) h, 10.0, 2.0);
        //GLKMatrix4MakePerspective(50, (GLfloat) w / (GLfloat) h, 10.0, 2.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,5,5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        //GLKMatrix4MakeLookAt(0,5,5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


int
main(int argc, char **argv)
{
        srand(getpid()+time(NULL));
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
        glutInitWindowSize(1024, 768);
        glutInitWindowPosition(10, 10);
        glutCreateWindow("test");
        glClearColor(0, 0, 0, 0);
        glShadeModel(GL_FLAT);
        glMatrixMode(GL_PROJECTION);
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);
        glutIdleFunc(display);
        glutMainLoop();
        return 0;
}


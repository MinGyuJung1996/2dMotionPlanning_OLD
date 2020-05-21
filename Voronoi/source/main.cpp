#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;



/* set global variables */
int mButton = -1;
int mOldY, mOldX;

/* vectors that makes the rotation and translation of the cube */
float eye[3] = {0.0f, 0.0f, 10.0f};
float rot[3] = {0.0f, 0.0f, 0.0f};

void clamp(float *v)
{
    int i;

    for (i = 0; i < 3; i ++)
        if (v[i] > 360 || v[i] < -360)
            v[i] = 0;
}
bool mat=true;
double xmin=-3,xmax=3,ymin=-3,ymax=3;
//double xmin=-.5,xmax=1.2,ymin=-.5,ymax=1.2;
int width=1000,height=1000;
void pixel2pos(int i,int j,double& x,double& y){
    x=xmin+i*1./width*(xmax-xmin);
    y=ymax+j*1./height*(ymin-ymax);
}

int clicked=0;
double old_x,old_y;

//------------------------------------------------------------------------
// Moves the screen based on mouse pressed button
//------------------------------------------------------------------------
void glutMotion(int i, int j)
{
    if(clicked){
        double x,y;
        pixel2pos(i,j,x,y);
        x-=old_x;
        y-=old_y;
        xmin-=x;
        xmax-=x;
        ymin-=y;
        ymax-=y;
        glutPostRedisplay();
    }
/*
    if (mButton == BUTTON_LEFT)
    {
        rot[0] -= (mOldY - y);
        rot[1] -= (mOldX - x);
        clamp (rot);
    }
    else if (mButton == BUTTON_RIGHT)
    {
        eye[2] -= (mOldY - y) * 0.05f; // here I multiply by a 0.2 factor to
                                      // slow down the zoom
        clamp (rot);
    }
    else if (mButton == BUTTON_LEFT_TRANSLATE)
    {
        eye[0] += (mOldX - x) * 0.01f;
        eye[1] -= (mOldY - y) * 0.01f;
        clamp (rot);
    }

    mOldX = x;
    mOldY = y;
    */
}

//------------------------------------------------------------------------
// Function that handles mouse input
//------------------------------------------------------------------------
void glutMouse(int button, int state, int i, int j)
{
    double x,y;
    pixel2pos(i,j,x,y);
    if(button==GLUT_LEFT_BUTTON){
        if(state==GLUT_DOWN){
            clicked=1;
            old_x=x;
            old_y=y;
        }
        if(state==GLUT_UP){
            clicked=0;
        }
    }
    if(button==3){
        xmin=x+(xmin-x)*.9;
        xmax=x+(xmax-x)*.9;
        ymin=y+(ymin-y)*.9;
        ymax=y+(ymax-y)*.9;
    }
    if(button==4){
        xmin=x+(xmin-x)/.9;
        xmax=x+(xmax-x)/.9;
        ymin=y+(ymin-y)/.9;
        ymax=y+(ymax-y)/.9;

    }


    /*
    if(state == GLUT_DOWN)
    {
        mOldX = x;
        mOldY = y;
        switch(button)
        {
            case GLUT_LEFT_BUTTON:
                if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
                {
                   mButton = BUTTON_LEFT_TRANSLATE;
                   break;
                } else
                {
                   mButton = BUTTON_LEFT;
                   break;
                }
            case GLUT_RIGHT_BUTTON:
                mButton = BUTTON_RIGHT;
                break;
        }
    } else if (state == GLUT_UP)
      mButton = -1;
      */
}

void drawBox() {
    glColor3f(1.f, 0.f, 0.f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(-1.f, -1.f, -1.f);
        glVertex3f(1.f, -1.f, -1.f);
        glVertex3f(1.f, 1.f, -1.f);
        glVertex3f(-1.f, 1.f, -1.f);
    glEnd();
    glColor3f(0.f, 1.f, 0.f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(-1.f, -1.f, 1.f);
        glVertex3f(1.f, -1.f, 1.f);
        glVertex3f(1.f, 1.f, 1.f);
        glVertex3f(-1.f, 1.f, 1.f);
    glEnd();
    glColor3f(0.f, 0.f, 1.f);
    glBegin(GL_LINES);
        glVertex3f(-1.f, -1.f, -1.f);
        glVertex3f(-1.f, -1.f, 1.f);
        glVertex3f(1.f, -1.f, -1.f);
        glVertex3f(1.f, -1.f, 1.f);
        glVertex3f(1.f, 1.f, -1.f);
        glVertex3f(1.f, 1.f, 1.f);
        glVertex3f(-1.f, 1.f, -1.f);
        glVertex3f(-1.f, 1.f, 1.f);
    glEnd();
}

void draw_mat();

void set_frame(double xmin,double xmax,double ymin,double ymax){

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xmin,xmax,ymin,ymax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    set_frame(xmin,xmax,ymin,ymax);

    glClearColor(1.f, 1.f, 1.f, 1.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    draw_mat();

    glutSwapBuffers();
}

void resize(int w, int h) {
    width=height=min(w,h);
    glViewport(0, 0, width, height);
}

bool dynamic=true;

int frame=0,frame2=100,frame3=200,frame4=300;
int index=0;
int index2=100;
int index3=200;
int index4=300;
extern vector<int> simple;

void keyboard(unsigned char key, int, int) {
    switch (tolower(key)) {
    case 27:
        exit(0);
        break;
    case ' ':
        dynamic=!dynamic;
        break;
    case 'q':
        index--;
        if(index<0)
            index+=simple.size();
        frame=simple[index];
        glutPostRedisplay();
        break;
    case 'w':
        index++;
        if(index>=simple.size())
            index-=simple.size();
        frame=simple[index];
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

void idle(){
}

void timer(int n){
    if(dynamic){
        index++;
        if(index>=simple.size())
            index-=simple.size();
        frame=simple[index];

        index2++;
        if(index2>=simple.size())
            index2-=simple.size();
        frame2=simple[index2];

        index3++;
        if(index3>=simple.size())
            index3-=simple.size();
        frame3=simple[index3];

        index4++;
        if(index4>=simple.size())
            index4-=simple.size();
        frame4=simple[index4];
    }
    glutPostRedisplay();
    glutTimerFunc(1000./30,timer,0);
}

void init_mat();

int main(int argc, char **argv) {
    init_mat();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0,0);

    glutCreateWindow("Medial Axis Transform");


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);


    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    //glutIdleFunc(idle);
    glutTimerFunc(1000./30,timer,0);
    glutMainLoop();

    return 0;
}

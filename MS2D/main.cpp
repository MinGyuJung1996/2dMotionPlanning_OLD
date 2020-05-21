#include "MS2D.h"
#include <iostream>

/* OpenGL Parameters */
GLsizei wd = 900, ht = 600;
double zoom = 1.2;
double tx, ty;
using namespace std;

/* Comments - MS2D.h 참고 */
int ModelInfo_CurrentFrame = 0;
pair<int, int> ModelInfo_CurrentModel;

int numoftest = 0;

FILE *f;

bool curves = false;
vector<BezierCrv> Models_Imported[8];
vector<Circle> InteriorDisks_Imported[8];

vector<Circle> InteriorDisks_Rotated[numofframe];
vector<Circle> InteriorDisks_Convolution;

vector<BezierCrv> Models_Rotated[numofframe];
vector<BezierCrv> Models[8];
vector<ArcSpline> Models_Rotated_Approx[numofframe];
vector<ArcSpline> Models_Approx[8];
Grid Grid_Trimming;

bool ModelInfo_Identical;

vector<BezierCrv> Crvr;

vector<deque<ArcSpline>> Model_Result;
vector<bool> ModelInfo_Boundary;
CacheCircles Cache_Trimming;
clock_t now, last;

void animate_func()
{
	now = clock();
	clock_t seconds = now - last;
	if (ModelInfo_CurrentFrame == 1) {
		while (clock() - last < 1000);
	}
	if ((ModelInfo_CurrentFrame == 0) && (ModelInfo_Identical))
		minkowskisum_id(ModelInfo_CurrentFrame, ModelInfo_CurrentModel.second);
	else
		minkowskisum(ModelInfo_CurrentFrame, ModelInfo_CurrentModel.second);

	ModelInfo_CurrentFrame++;
	last = now;


	glutPostRedisplay();
}


void reshape_callback(GLint w, GLint h)
{
	if (w * 2 < h * 3) {
		wd = w;
		ht = w * 2 / 3;
		glViewport(wd * 1 / 3, 0, wd * 2 / 3, ht);
	}
	else {
		wd = h * 3 / 2;
		ht = h;
		glViewport(wd * 1 / 3, 0, wd * 2 / 3, ht);
	}
}


void setup_viewvolume()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-zoom+ tx, zoom+ tx, -zoom+ ty, zoom+ ty);
}
void setup_transform()
{
}

void display_callback(void)
{
	if (ModelInfo_CurrentFrame == numofframe) {
		ModelInfo_CurrentFrame = 0;
		ModelInfo_CurrentModel.second++;

		if (ModelInfo_CurrentModel.second == 8) {
			ModelInfo_CurrentModel.second = 0;
			ModelInfo_CurrentModel.first++;
		}

		if (ModelInfo_CurrentModel.first == 8) {
			numoftest++;
			if (numoftest == 1) {
				save();
				system("pause");
				exit(0);
			}
			ModelInfo_CurrentModel.first = ModelInfo_CurrentModel.second = 0;
		}
		postProcess(ModelInfo_CurrentModel.first, ModelInfo_CurrentModel.second);
	}

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	setup_viewvolume();
	setup_transform();


	// Rendering code....
	glLineWidth(4.0f);
	glPointSize(2.8f);
	glColor3f(0.0f, 0.0f, 0.0f);


	glViewport(wd * 1 / 3, 0, wd * 2 / 3, ht);

	/* Boundary 위에 Model을 올려놓은 그림을 그리는 코드 */
	//if (curves) {
	//	for (int u = 0; u < (int)Crv[ModelInfo_CurrentModel.second].size(); u++)
	//		Crv[ModelInfo_CurrentModel.second][u].draw();
	//}

	//if (curves) {
	//	glColor3f(1.0f, 0.0f, 0.0f);
	//	for (int i = 0; i < (int)Model_Result.size(); i++)
	//		for (int j = 0; j < (int)Model_Result[i].size(); j++)
	//			for (int u = 0; u < (int)Models_Rotated[ModelInfo_CurrentFrame].size(); u++)
	//				Models_Rotated[ModelInfo_CurrentFrame][u].draw(Model_Result[i][j].Models_Rotated_Approx.front().x[0]);

	//}

	glColor3f(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < (int)Model_Result.size(); i++) {
		if (ModelInfo_Boundary[i])
			glColor3d(0.0, 0.0, 0.0);
		else
			glColor3d(0.0, 0.0, 0.0);
		for (int j = 0; j < (int)Model_Result[i].size(); j++)
			for (int k = 0; k < (int)Model_Result[i][j].Arcs.size(); k++)
				Model_Result[i][j].Arcs[k].draw();
	}


	
	glColor3f(0.0f, 0.0f, 0.0f);

	glViewport(0, 0, wd * 1 / 3, ht / 2);
	for (int i = 0; i < (int)Models_Rotated[ModelInfo_CurrentFrame].size(); i++)
		Models_Rotated[ModelInfo_CurrentFrame][i].draw();

	glViewport(0, ht / 2, wd * 1 / 3, ht / 2);


	for (int i = 0; i < (int)Models_Imported[ModelInfo_CurrentModel.second].size(); i++) {
		Models_Imported[ModelInfo_CurrentModel.second][i].draw();
	}

	glutSwapBuffers();
}

void hit_index(int x, int y)
{
}

void mouse_callback(int button, int action, int x, int y)
{

	if (button == 3)
		zoom *= 1.05;

	if (button == 4)
		zoom *= 0.95;

	glutPostRedisplay();
}

void keyboard_callback(unsigned char a, int b, int c) {
	if (a == 'a')
		tx += 0.05 * zoom;
	if (a == 'd')
		tx -= 0.05 * zoom;
	if (a == 's')
		ty += 0.05 * zoom;
	if (a == 'w')
		ty -= 0.05 * zoom;
	if (a == 'm')
		curves ^= true;
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(wd, ht);
	glutCreateWindow("Minkowski Sum");
	initialize();
	glutReshapeFunc(reshape_callback);
	glutDisplayFunc(display_callback);
	glutMouseFunc(mouse_callback);
	glutKeyboardFunc(keyboard_callback);
	glutIdleFunc(animate_func);
	glutMainLoop();
	return 0;
}
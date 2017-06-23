#include <fstream>
#include <regex>
#include <iostream>
#include <vector>
#include <windows.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include "glut.h"
#include "Xline.hpp"
#include <numeric>

using namespace std;
//global variables
int dataPoints;
vector< vector<float> > vec;
vector<Xline> Xlines;
void Display();
void Init(void);

char* filename = "Machine.txt";

void countData(char * filename)
{
	std::fstream inStream;
	inStream.open(filename, ios::in); //openfile
	if (inStream.fail())
		return;

	std::string line;
	getline(inStream, line);

	regex rgx("\\s+");
	sregex_token_iterator iter(line.begin(), line.end(), rgx, -1);
	sregex_token_iterator end;
	for (; iter != end; ++iter)
		dataPoints++;
}//end countData


void getPointsFromFile(char * filename)
{

	countData(filename);

	//add points to matrix
	fstream inStream;

	inStream.open(filename, ios::in); //openfile
	if (inStream.fail())
		return;

	string line;
	float count = 0;

	while (getline(inStream, line)) {

		vec.push_back(vector<float>());

		regex rgx("\\s+");
		sregex_token_iterator iter(line.begin(), line.end(), rgx, -1);
		sregex_token_iterator end;
		for (; iter != end; ++iter) {
			vec.at(count).push_back(stof(*iter));
		}
		count++;
	}//end while

}//end getPointsFromFile

float normalize(float rawNum, float minAllowed, float maxAllowed, float min, float max) {
	return (maxAllowed - minAllowed) * (rawNum - min) / (max - min) + minAllowed;
}//end normalize

 /*Return max value*/
float maxVal(vector<vector<float>> vec, int col) {
	float max = vec.at(0).at(col);
	for (int i = 0; i < vec.size(); i++) {
		if (vec.at(i).at(col) > max)
			max = vec.at(i).at(col);
	}
	return max;
}//end maxVal

 /*Return max value*/
float minVal(vector<vector<float>> vec, int col) {
	float min = vec.at(0).at(col);
	for (int i = 0; i < vec.size(); i++) {
		if (vec.at(i).at(col) < min)
			min = vec.at(i).at(col);
	}
	return min;
}//end maxVal

 /*scalePoints of the vector*/
void scaleVector(vector<vector<float>> *vec) {
	for (int c = 0; c < vec->at(0).size() - 1; c++) {
		float min = minVal(*vec, c);
		float max = maxVal(*vec, c);
		for (int r = 0; r < vec->size(); r++) {
			vec->at(r).at(c) = normalize(vec->at(r).at(c), 0, 1, min, max);
		}
	}
}//end scaleVector

 /*Find the class in the data*/
int getClass(vector<vector <float> > *vec, int row) {
	int _class = vec->at(row).back();
	if (_class == 1) {
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	else if (_class == 2) {
		glColor3f(0.0f, 1.0f, 0.0f);
	}
	else if (_class == 3) {
	glColor3f(0.0f, 0.0f, 1.0f);
	}
	else {
	glColor3f(0.0f, 1.0f, 1.0f);
	}
	return _class;
}

//add draw verticle anchors
void addXlinesVert() {
	float stepX = 800 / dataPoints;
	float x = 0;
	for (int i = 0; i < dataPoints - 1; i++) {
		Vector2f tail(x, 1);
		Vector2f head(x, 0);
		Xline line(head, tail);
		Xlines.push_back(line);
		x += stepX;
	}
}//end addXlinesVert

//draw horizontal anchors
void addXlinesHor() {
	int stepX = 800 / dataPoints;
	int x = 0;
	for (int i = 0; i < dataPoints - 1; i++) {
		Vector2f head(x, .5);
		x += stepX;
		Vector2f tail(x, .5);
		Xline line(head, tail);
		Xlines.push_back(line);
	}
}//end addXlinesHor

//draw xlines
void drawXline(Vector2f head, Vector2f tail) {
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(tail.x, tail.y);
	glVertex2f(head.x, head.y);
	glEnd();
	glFlush();
}

/*bezier curves*/
vector<Vector2f> CalcCubicBezier(
	Vector2f start,
	Vector2f end,
	Vector2f startControl,
	Vector2f endControl,
	size_t numSegments)
{
	vector<Vector2f> ret;
	if (!numSegments)
		return ret;

	ret.push_back(start); // First point is fixed
	float p = 1.f / numSegments;
	float q = p;
	for (size_t i = 1; i < numSegments; i++, p += q) // Generate all between
		ret.push_back(p * p * p * (end + 3.f * (startControl - endControl) - start) +
			3.f * p * p * (start - 2.f * startControl + endControl) +
			3.f * p * (startControl - start) + start);
	ret.push_back(end); // Last point is fixed
	return ret;
}

/*keyboard function*/
void keyBoard(unsigned char key, int x, int y) {
	if (key == '1') {	//CPC with no color

		//set up for program
		vec.clear();
		dataPoints = 0;
		getPointsFromFile(filename);
		scaleVector(&vec);
		Xlines.clear();

		//set up window to draw
		glClear(GL_COLOR_BUFFER_BIT);
		int stepX = 800 / dataPoints;

		addXlinesVert();
		//draw Xlines
		for (int i = 0; i < Xlines.size(); i++) {
			drawXline(Xlines.at(i).head, Xlines.at(i).tail);
		}//end for
		glLineWidth(1);
		int currentX = 0;
		for (int i = 0; i < vec.size(); i++) {
			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < vec[i].size() - 1; j++) {
				glVertex2f(currentX, vec[i][j]);
				currentX += stepX;
			}
			currentX = 0;
			glEnd();
			glFlush();
		}

	}//end if
	else if (key == '2') {	//cpc with color
		//set up for program
		vec.clear();
		dataPoints = 0;
		getPointsFromFile(filename);
		scaleVector(&vec);
		Xlines.clear();

		//set up window to draw
		glClear(GL_COLOR_BUFFER_BIT);
		int stepX = 800 / dataPoints;

		addXlinesVert();
		//draw Xlines
		for (int i = 0; i < Xlines.size(); i++) {
			drawXline(Xlines.at(i).head, Xlines.at(i).tail);
		}//end for
		glLineWidth(1);
		int currentX = 0;
		for (int i = 0; i < vec.size(); i++) {
			getClass(&vec, i);
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < vec[i].size() - 1; j++) {
				glVertex2f(currentX, vec[i][j]);
				currentX += stepX;
			}
			currentX = 0;
			glEnd();
			glFlush();
		}
	}
	else if (key == '3') { //points only with color
		//set up for program
		vec.clear();
		dataPoints = 0;
		getPointsFromFile(filename);
		scaleVector(&vec);
		Xlines.clear();

		//set up window to draw
		glClear(GL_COLOR_BUFFER_BIT);
		int stepX = 800 / dataPoints;
		int currentX = 0;
		for (int i = 0; i < vec.size(); i++) {
			getClass(&vec, i);
			glBegin(GL_POINTS);
			for (int j = 0; j < vec[i].size() - 1; j++) {
				glVertex2f(currentX, vec[i][j]);
				currentX += stepX;
			}
			currentX = 0;
			glEnd();
			glFlush();
		}

	}
	else if (key == '4') {	//bezier curves with color
		//set up for program
		vec.clear();
		dataPoints = 0;
		getPointsFromFile(filename);
		scaleVector(&vec);
		Xlines.clear();

		glClear(GL_COLOR_BUFFER_BIT);
		int stepX = 800 / dataPoints;

		addXlinesHor();
		//draw Xlines
		for (int i = 0; i < Xlines.size(); i++) {
			drawXline(Xlines.at(i).head, Xlines.at(i).tail);
		}//end for
		glLineWidth(1);
		int currentX = 0;
		//draw each line for file
		for (int i = 0; i < vec.size(); i++) {
			getClass(&vec, i);	//get class and assign color
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < vec[i].size() - 1; j++) {
				if (j < Xlines.size() - 1) {
					Vector2f v = Xlines.at(j).findGlobalPoint(vec[i][j]); //get point and find position on xline
					glVertex2f(v.x, v.y);	//draw point
					Vector2f v2 = Xlines.at(j + 1).findGlobalPoint(vec[i][j + 1]); //get point and find position on xline
					Vector2f difference = v2 - v;
					Vector2f mid(Xlines.at(j).tail);
					if (getClass(&vec, i) == 1) {
						mid.y = 1.0 - difference.x / 800;
					}
					else if (getClass(&vec, i) == 2) {
						mid.y = difference.x / 800;
					}

					vector<Vector2f> points = CalcCubicBezier(v, v2, mid, mid, 20);

					for (auto i : points)
						glVertex2f(i.x, i.y);

					currentX += stepX;
				}
				else {
					Vector2f v = Xlines.at(j).findGlobalPoint(vec[i][j]); //get point and find position on xline
					glVertex2f(v.x, v.y);	//draw point
					currentX += stepX;
				}
			}//end for
			currentX = 0;
			glEnd();
			glFlush();
		}//end for
	}

}//end keyboard

void Display()
{

}//end display


void Init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	//gluOrtho2D(-100.0, 800.0, 0.0, 1.0);
	gluOrtho2D(-250.0, 800.0, -1.0, 1.5);
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glLoadIdentity();
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Final Project");
	glutKeyboardFunc(keyBoard);
	glutDisplayFunc(Display);
	Init();
	glutMainLoop();

}
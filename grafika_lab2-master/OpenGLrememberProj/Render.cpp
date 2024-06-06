#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include <list>
#define PI 3.14159265

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("fish.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}



std::list<std::pair<double, double>> find_anchors(double v1[3], double v2[3], int steps = 5, double mid[3] = nullptr)
{
	double m[3];
	if (mid == nullptr)
	{
		for (int i = 0; i < 3; i++) {
			m[i] = (v1[i] + v2[i]) / 2;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++) {
			m[i] = mid[i];
		}
	}

	std::list<std::pair<double, double>> xyl;
	double vect[3], rad, a1, A1, sa;

	for (int i = 0; i < 3; i++) {
		vect[i] = v2[i] - v1[i];
	}

	rad = sqrt(pow(vect[0], 2) + pow(vect[1], 2) + pow(vect[2], 2)) / 2;

	a1 = acos((v1[0] - m[0]) / rad);

	sa = PI / steps;

	A1 = asin((v1[1] - m[1]) / rad);

	for (int i = 0; i <= steps; i++) {
		//x=a+r*cos(t)
		//y=b+r*sin(t)
		std::pair<double, double> pos;
		pos.first = m[0] + rad * cos(a1 + sa * i);
		pos.second = m[1] + rad * sin(A1 + sa * i);
		xyl.push_back(pos);
	}

	return xyl;
}

void cells()
{
	int x = -100;
	while (x <= 100) {
		glColor3d(0.7, 0.7, 0.7);
		glBegin(GL_LINES);
		glVertex3d(-100, x, 0);
		glVertex3d(100, x, 0);

		glVertex3d(x, -100, 0);
		glVertex3d(x, 100, 0);
		glEnd();
		x += 1;
	}
}

double* normal_vect(double p1[3], double pm[3], double p2[3]) {
	double vect1[3], vect2[3], n_vect[3];

	for (int i = 0; i < 3; i++) {
		vect1[i] = p1[i] - pm[i];
		vect2[i] = p2[i] - pm[i];
	}

	n_vect[0] = 1 * (vect1[1] * vect2[2] - vect1[2] * vect2[1]);
	n_vect[1] = 1 * (- vect1[0] * vect2[2] + vect1[2] * vect2[0]);
	n_vect[2] = 1 * (vect1[0] * vect2[1] - vect1[1] * vect2[0]);

	return n_vect;
}

void dr_trg(double x[3], double y[3], double z[3], double color[3]) {
	

	glBegin(GL_TRIANGLES);
	//glColor3dv(color);
	//glNormal3dv(normal_vect(x, y, z));
	glTexCoord2d((10 + x[1]) * 0.025, (10 - x[0]) * 0.025);
	glVertex3dv(x);
	glTexCoord2d((10 + y[1]) * 0.025, (10 - y[0]) * 0.025);
	glVertex3dv(y);
	glTexCoord2d((10 + z[1]) * 0.025, (10 - z[0]) * 0.025);
	glVertex3dv(z);
	glEnd();

	glColor3d(0, 0, 0);
}

void dr_horiz_quad(double x1[3], double y1[3], double y2[3], double x2[3])
{
	glBegin(GL_QUADS);
	glNormal3dv(normal_vect(x1, y1, x2));
	glTexCoord2d((10 + x1[1]) * 0.025, (10 - x1[0]) * 0.025);
	glVertex3dv(x1);
	glTexCoord2d((10 + y1[1]) * 0.025, (10 - y1[0]) * 0.025);
	glVertex3dv(y1);
	glTexCoord2d((10 + y2[1]) * 0.025, (10 - y2[0]) * 0.025);
	glVertex3dv(y2);
	glTexCoord2d((10 + x2[1]) * 0.025, (10 - x2[0]) * 0.025);
	glVertex3dv(x2);
	glEnd();

	glColor3d(0, 0, 0);
}

void dr_vert_quad(double x1[3], double y1[3], double y2[3], double x2[3], double zero_x, double zero_y, double end_x, double end_y)
{
	glBegin(GL_QUADS);
	glNormal3dv(normal_vect(x1, y1, x2));
	glTexCoord2d(end_x, zero_y);
	glVertex3dv(x1);
	glTexCoord2d(zero_x, zero_y);
	glVertex3dv(y1);
	glTexCoord2d(zero_x, end_y);
	glVertex3dv(y2);
	glTexCoord2d(end_x, end_y);
	glVertex3dv(x2);
	glEnd();

	glColor3d(0, 0, 0);
}

void prism_func(double height)
{
	glBindTexture(GL_TEXTURE_2D, texId);

	//bottom//

	glNormal3d(0, 0, -1);

	double color[] = { 0.3, 0, 0 };

	//double m[] = { -9,-3,0 };

	double v1[] = { -6, -6, 0 };
	double v2[] = { -3, -1, 0 };
	double v3[] = { -7, 3, 0 };
	double v4[] = { -2, 6, 0 };
	double v5[] = { 0, 1, 0 };
	double v6[] = { 7, 0, 0 };
	double v7[] = { 0, -1, 0 };
	double v8[] = { -2, -6, 0 };

	int steps = 5;
	std::list<std::pair<double, double>> h_cyl = find_anchors(v3, v4, steps);
	std::list<std::pair<double, double>>::iterator it = h_cyl.begin();
	std::pair<double, double> temp = *it;
	double mid[3];
	for (int i = 0; i < 3; i++) {
		mid[i] = (v3[i] + v4[i]) / 2;
	}
	for (int i = 0; i < steps; i++) {
		it++;
		glBegin(GL_TRIANGLES);
		//glColor3dv(color);
		glTexCoord2d((10 + it->second) * 0.025, (10 - it->first) * 0.025);
		glVertex3d(it->first, it->second, 0);
		glTexCoord2d((10 + temp.second) * 0.025, (10 - temp.first) * 0.025);
		glVertex3d(temp.first, temp.second, 0);
		glTexCoord2d((10 + mid[1]) * 0.025, (10 - mid[0]) * 0.025);
		glVertex3dv(mid);
		glEnd();
		temp = *it;
	}
	it = h_cyl.begin();
	temp = *it;

	dr_horiz_quad(v8, v7, v2, v1);
	dr_horiz_quad(v5, v4, v3, v2);
	dr_trg(v5, v2, v7, color);
	dr_trg(v7, v5, v6, color);

	//up//
	
	glNormal3d(0, 0, 1);

	//color[0] = 0; color[1] = 0.3; color[2] = 0;

	double Uv1[] = { -6, -6, height };
	double Uv2[] = { -3, -1, height };
	double Uv3[] = { -7, 3, height };
	double Uv4[] = { -2, 6, height };
	double Uv5[] = { 0, 1, height };
	double Uv6[] = { 7, 0, height };
	double Uv7[] = { 0, -1, height };
	double Uv8[] = { -2, -6, height };

	mid[2] = height;
	for (int i = 0; i < steps; i++) {
		it++;
		glBegin(GL_TRIANGLES);
		//glColor3dv(color);
		glTexCoord2d((10 + it->second) * 0.025, (10 - it->first) * 0.025);
		glVertex3d(it->first, it->second, height);
		glTexCoord2d((10 + temp.second) * 0.025, (10 - temp.first) * 0.025);
		glVertex3d(temp.first, temp.second, height);
		glTexCoord2d((10 + mid[1]) * 0.025, (10 - mid[0]) * 0.025);
		glVertex3dv(mid);
		glEnd();
		temp = *it;
	}
	it = h_cyl.begin();
	temp = *it;

	dr_horiz_quad(Uv1, Uv2, Uv7, Uv8);
	dr_horiz_quad(Uv2, Uv3, Uv4, Uv5);
	dr_trg(Uv2, Uv5, Uv7, color);
	dr_trg(Uv5, Uv7, Uv6, color);

	//sides//

	//color[0] = 0; color[1] = 0; color[2] = 0.3;

	for (int i = 0; i < steps; i++) {
		it++;
		glBegin(GL_QUADS);
		//glColor3dv(color);
		double x1[3] = {temp.first, temp.second, 0}, y1[3] = { it->first, it->second, 0 }, x2[3] = {it->first, it->second, height};
		glNormal3dv(normal_vect(x1, y1, x2));
		glTexCoord2d(0.5 + 0.057 * (i), 0);
		glVertex3d(it->first, it->second, 0);
		glTexCoord2d(0.557 + 0.057 * (i), 0);
		glVertex3d(temp.first, temp.second, 0);
		glTexCoord2d(0.557 + 0.057 * (i), 0.1);
		glVertex3d(temp.first, temp.second, height);
		glTexCoord2d(0.5 + 0.057 * (i), 0.1);
		glVertex3d(it->first, it->second, height);
		glEnd();
		temp = *it;
	}
	it = h_cyl.begin();
	temp = *it;

	dr_vert_quad(v1, v2, Uv2, Uv1, 0.500, 0.200, 0.646, 0.300);
	dr_vert_quad(v2, v3, Uv3, Uv2, 0.648, 0.200, 0.787, 0.300);
	dr_vert_quad(v4, v5, Uv5, Uv4, 0.789, 0.200, 0.924, 0.300);

	dr_vert_quad(v5, v6, Uv6, Uv5, 0.677, 0.400, 0.500, 0.500);
	dr_vert_quad(v6, v7, Uv7, Uv6, 0.500, 0.400, 0.677, 0.500);
	dr_vert_quad(v7, v8, Uv8, Uv7, 0.679, 0.400, 0.839, 0.500);
	dr_vert_quad(v8, v1, Uv1, Uv8, 0.841, 0.400, 0.942, 0.500);
}


void Render(OpenGL *ogl)
{

	

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	cells();

	prism_func(4);

	//��������������� ���������
	glBegin(GL_QUADS);
	glColor4d(0.6, 0, 0.4, 0.3);
	glVertex3d(-10, -10, 6);
	glVertex3d(-10, 10, 6);
	glVertex3d(10, 10, 6);
	glVertex3d(10, -10, 6);
	glEnd();

	//������ ��������� ���������� ��������
	/*double A[3] = { -4, -4, 0 };
	double B[3] = { 4, -4, 0 };
	double C[3] = { 4, 4, 5 };
	double D[3] = { -4, 4, 5 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 1);
	glVertex3dv(C);
	glTexCoord2d(0, 1);
	glVertex3dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


	//��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
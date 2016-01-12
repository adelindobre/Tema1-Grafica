
//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//librarie matematica 
#include "..\\dependente\glew\glew.h"
#include "..\\dependente\glm\glm.hpp"
#include "..\\dependente\glm\gtc\type_ptr.hpp"
#include "..\\dependente\glm\gtc\matrix_transform.hpp"

//neimportant pentru acest laborator, va fi explicat in viitor
#include "..\\lab_blackbox.hpp"
//interfata cu glut, ne ofera fereastra, input, context opengl
#include "..\\lab_glut.hpp"

//time
# define M_PI  3.14159265358979323846
#include <ctime>
#include <iostream>
#include <math.h>

//clasa lab, mosteneste din WindowListener
class Lab : public lab::glut::WindowListener{

//variabile
private:
	//un obiect ce ascunde functii pe care vom ajunge sa le scriem de mana.
	lab::BlackBox BLACKBOX;

	//un obiect ce va fi desenat (nu ne intereseaza propietatile sale momentan)
	lab::Polyline *cadru;
	lab::Mesh	***piesa;
	lab::Mesh  *sursa, *camera;
	lab::Mesh **raza;

	float length = 5, size[10];
	float x1 = 70, y1 = 145, x3 = 70, y3 = 150;
	float xc = x3, yc = y3;
	float unghi = 0;
	int first_press = 0, cont = 0, surface, flag = 0;

	glm::mat3 transformation[20];
	unsigned int width, height;
	
	//parametrii pentru prima transformare fereastra poarta - desenare in viewport 1
	float ferx1 = 0, fery1 = 0, widthfer1 = 300, heightfer1 = 200; //coltul stanga jos al ferestrei (coordonatele logice) si dimensiunile ferestrei
	float viewx1, viewy1, widthview1, heightview1; //coltul stanga jos al portii (coordonatele ferestrei de afisare) si dimensiunile portii


//metode ale clasei lab
public:
	
	//constructor .. e apelat cand e instantiata clasa
	Lab(){

		width = lab::glut::getInitialWindowInformation().width;
		height = lab::glut::getInitialWindowInformation().height;
		piesa = new lab::Mesh**[3];

		cadru = myLoadSquare_as_Polyline(-10, 5, 360, 230);  //coordonatele sunt date relativ la fereastra aplicatie
		
		size[0] = 3;
		piesa[0] = new lab::Mesh*[3];
		piesa[0][0] = myLoadSquare_as_Mesh(100, 160, 35, 35);
		piesa[0][1] = myLoadSquare_as_Mesh(115, 155, 35, 5);
		piesa[0][2] = myLoadSquare_as_Mesh(125, 120, 5, 35);

		size[1] = 1;
		piesa[1] = new lab::Mesh*[1];
		piesa[1][0] = myLoadSquare_as_Mesh(10, 160, 35, 35);


		size[2] = 3;
		piesa[2] = new lab::Mesh*[3];
		piesa[2][0] = myLoadSquare_as_Mesh(-10, 100, 20, 20);
		piesa[2][1] = myLoadSquare_as_Mesh(-5, 90, 50, 10);
		piesa[2][2] = myLoadSquare_as_Mesh(-5, 60, 10, 30);

		size[3] = 1;
		piesa[3] = new lab::Mesh*[1];
		piesa[3][0] = myLoadSquare_as_Mesh(100, 70, 15, 10);

		size[4] = 1;
		piesa[4] = new lab::Mesh*[1];
		piesa[4][0] = myLoadSquare_as_Mesh(60, 5, 40, 40);


		size[5] = 1;
		piesa[5] = new lab::Mesh*[1];
		piesa[5][0] = myLoadSquare_as_Mesh(140, 5, 50, 40);


		size[6] = 1;
		piesa[6] = new lab::Mesh*[1];
		piesa[6][0] = myLoadSquare_as_Mesh(210, 5, 55, 40);

		size[7] = 1;
		piesa[7] = new lab::Mesh*[1];
		piesa[7][0] = myLoadSquare_as_Mesh(210, 150, 20, 30);

		size[8] = 1;
		piesa[8] = new lab::Mesh*[1];
		piesa[8][0] = myLoadSquare_as_Mesh(210, 100, 20, 10);
		

		size[9] = 1;
		piesa[9] = new lab::Mesh*[1];
		piesa[9][0] = myLoadCircle_as_Mesh(153, 80, 20);
		

		sursa = myLoadCircle_as_Mesh(70, 150, 10);
		camera = myLoadTriangle_as_Mesh(280, 195, 310, 195, 310, 165);
	
		raza = new lab::Mesh*[20];
		raza[cont] = myLoadSquare_as_Mesh(70, 145, 1, 5);

		//transformari pentru fiecare obiect
		for (int i = 0; i < 20; i++)
			transformation[i] = glm::mat3(1);

	}

	//destructor .. e apelat cand e distrusa clasa
	~Lab(){
		delete cadru;
	}

	//format vertex de mana
	struct MyVertexFormat{
		glm::vec2 pozitie;
		MyVertexFormat(){
			pozitie = glm::vec2(0, 0);
		}
		MyVertexFormat(float px, float py){
			pozitie = glm::vec2(px, py);
		}
		MyVertexFormat operator=(const MyVertexFormat &rhs){
			pozitie = rhs.pozitie;
			return (*this);
		}
	};


	//gives ownership
	lab::Mesh* myLoadSquare_as_Mesh(float x, float y, float w, float h){
		//definim containere pentru date
		std::vector<MyVertexFormat> vertecsi;
		std::vector<glm::uvec3> indecsi;

		//4 vertecsi (doar pozitii fara normale fara texcoords)
		vertecsi.push_back(MyVertexFormat(x, y));
		vertecsi.push_back(MyVertexFormat(x + w, y));
		vertecsi.push_back(MyVertexFormat(x + w, y + h));
		vertecsi.push_back(MyVertexFormat(x , y + h));
		
		//2 triunghiuri pentru 1 fata
		indecsi.push_back(glm::uvec3(0, 1, 2));	indecsi.push_back(glm::uvec3(2, 3, 0));

		//creaza vao
		unsigned int mesh_vao;
		glGenVertexArrays(1, &mesh_vao);
		glBindVertexArray(mesh_vao);

		//creeaza vbo
		unsigned int mesh_vbo;
		glGenBuffers(1, &mesh_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertexFormat)*vertecsi.size(), &vertecsi[0], GL_STATIC_DRAW);
		
		//creeaza ibo
		unsigned int mesh_ibo;
		glGenBuffers(1, &mesh_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indecsi.size() * 3, &indecsi[0], GL_STATIC_DRAW);

		//creez obiect de tip mesh
		lab::Mesh* mesh = new lab::Mesh(mesh_vbo, mesh_ibo, mesh_vao, indecsi.size() * 3);
		mesh->setCoordinates(x, y, x + w, y + h);

		// traducem la OpenGL cum sa foloseasca datele noastre
		BLACKBOX.bindMesh(mesh, sizeof(MyVertexFormat));

		//return
		return mesh;
	}

	lab::Mesh* myLoadTriangle_as_Mesh(float x1, float y1, float x2, float y2, float x3, float y3){
		//definim containere pentru date
		std::vector<MyVertexFormat> vertecsi;
		std::vector<glm::uvec3> indecsi;

		//4 vertecsi (doar pozitii fara normale fara texcoords)
		vertecsi.push_back(MyVertexFormat(x1, y1));
		vertecsi.push_back(MyVertexFormat(x2, y2));
		vertecsi.push_back(MyVertexFormat(x3, y3));
	
		//2 triunghiuri pentru 1 fata
		indecsi.push_back(glm::uvec3(0, 1, 2));

		//creaza vao
		unsigned int mesh_vao;
		glGenVertexArrays(1, &mesh_vao);
		glBindVertexArray(mesh_vao);

		//creeaza vbo
		unsigned int mesh_vbo;
		glGenBuffers(1, &mesh_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertexFormat)*vertecsi.size(), &vertecsi[0], GL_STATIC_DRAW);

		//creeaza ibo
		unsigned int mesh_ibo;
		glGenBuffers(1, &mesh_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indecsi.size() * 3, &indecsi[0], GL_STATIC_DRAW);

		//creez obiect de tip mesh
		lab::Mesh* mesh = new lab::Mesh(mesh_vbo, mesh_ibo, mesh_vao, indecsi.size() * 3);

		// traducem la OpenGL cum sa foloseasca datele noastre
		BLACKBOX.bindMesh(mesh, sizeof(MyVertexFormat));

		//return
		return mesh;
	}
	lab::Mesh* myLoadCircle_as_Mesh(float x, float y, float raza)
	{
		std::vector<MyVertexFormat> vertecsi;
		std::vector<glm::uvec3> indecsi;

		for (int i = 0; i < 1000; i++)
		{
			float theta = 2.0f * 3.14159 * float(i) / 100.0f;

			float x1 = raza * cosf(theta);
			float y1 = raza * sinf(theta);

			vertecsi.push_back(MyVertexFormat(x + x1, y + y1));

		}
		for (int i = 0; i < 999; i++)
		{
			indecsi.push_back(glm::uvec3(0, i, i+1));
		}

		unsigned int mesh_vao;
		glGenVertexArrays(1, &mesh_vao);
		glBindVertexArray(mesh_vao);

		unsigned int mesh_vbo;
		glGenBuffers(1, &mesh_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertexFormat)* vertecsi.size(), &vertecsi[0], GL_STATIC_DRAW);

		unsigned int mesh_ibo;
		glGenBuffers(1, &mesh_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* indecsi.size() * 3, &indecsi[0], GL_STATIC_DRAW);

		//creez obiect de tip mesh
		lab::Mesh* mesh = new lab::Mesh(mesh_vbo, mesh_ibo, mesh_vao, indecsi.size() * 3);
		mesh->setCircleCoordinates(x, y, raza);

		// traducem la OpenGL cum sa foloseasca datele noastre
		BLACKBOX.bindMesh(mesh, sizeof(MyVertexFormat));

		//return
		return mesh;
	}

	lab::Polyline* myLoadSquare_as_Polyline(float x, float y, float width, float height){
		
		//definim containere pentru date
		std::vector<MyVertexFormat> vertecsi;
		std::vector<unsigned int> indecsi; //nu mai dau cate trei indici, pentru ca nu mai desenez triunghiuri, ci LINE_STRIP, adica segmente conectate

		//4 vertecsi (doar pozitii fara normale fara texcoords)
		vertecsi.push_back(MyVertexFormat(x, y));
		vertecsi.push_back(MyVertexFormat(x + width - 40 , y));
		vertecsi.push_back(MyVertexFormat(x + width - 40 , y + height - 40));
		vertecsi.push_back(MyVertexFormat(x, y + height - 40));

		//insiruirea punctelor (daca dorim sa obtinem un patrat, primul punct se pune si la sfarsit, ca sa se inchida polilinia)
		indecsi.push_back(0);	
		indecsi.push_back(1);
		indecsi.push_back(2);
		indecsi.push_back(3);
		indecsi.push_back(0);

		//creaza vao
		unsigned int polyline_vao;
		glGenVertexArrays(1, &polyline_vao);
		glBindVertexArray(polyline_vao);

		//creeaza vbo
		unsigned int polyline_vbo;
		glGenBuffers(1, &polyline_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, polyline_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertexFormat)*vertecsi.size(), &vertecsi[0], GL_STATIC_DRAW);

		//creeaza ibo
		unsigned int polyline_ibo;
		glGenBuffers(1, &polyline_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polyline_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indecsi.size(), &indecsi[0], GL_STATIC_DRAW);

		//creez obiect de tip polilinie
		lab::Polyline* polyline = new lab::Polyline(polyline_vbo, polyline_ibo, polyline_vao, indecsi.size() * 3);
		polyline->setCoordinates(x, y, x + width - 40, y + width - 40);
	
		// traducem la OpenGL cum sa foloseasca datele noastre
		BLACKBOX.bindPolyline(polyline, sizeof(MyVertexFormat));

		//return
		return polyline;
	}

	//--------------------------------------------------------------------------------------------
	//functii de transformare --------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	glm::mat3 myIdentity(){
		return glm::transpose(glm::mat3(	//de ce? pt ca in memorie se scriu coloane dar in IDE le vezi ca randuri
			1, 0, 0,						//asa le vezi ca randuri si le scrii ca randuri!
			0, 1, 0,
			0, 0, 1
			));
	}
	glm::mat3 myTranslate(float tx, float ty){
		return glm::transpose(glm::mat3(
			1,0,tx,
			0,1,ty,
			0,0,1
			));
	}
	glm::mat3 myRotate(float u){
		return glm::transpose(glm::mat3(
			glm::cos(u), -glm::sin(u), 0,
			glm::sin(u), glm::cos(u), 0,
			0, 0, 1
			));
	}
	glm::mat3 myScale(float sx, float sy){
		return glm::transpose(glm::mat3(
			sx,0,0,
			0,sy,0,
			0,0,1
			));
		}
	// Intoarce matricea transformarii de vizualizare
	glm::mat3 myTransform_viz_2D(float ferx, float fery, float widthfer, float heightfer,
		float viewx, float viewy, float widthview, float heightview)
	{
		float sx, sy, tx, ty;
		sx = widthview / widthfer; sy = heightview / heightfer;
		tx = -sx*ferx; ty = -sy*fery;
		return glm::mat3(sx, 0, 0, 0, sy, 0, tx, ty, 1);
	}
	// Intoarce matricea transformarii de vizualizare cu scalare uniforma
	glm::mat3 myTransform_viz_2D_unif(float ferx, float fery, float widthfer, float heightfer,
		float viewx, float viewy, float widthview, float heightview)
	{
		float sx, sy, tx, ty, smin;
		sx = widthview / widthfer; sy = heightview / heightfer;
		if (sx < sy) smin = sx; else smin = sy;
		tx = -smin*ferx + (widthview - smin*widthfer) / 2; ty = -smin*fery + (heightview - smin*heightfer) / 2;

		return glm::mat3(smin, 0, 0, 0, smin, 0, tx, ty, 1);
	}
	//calculeaza directia suprafetei pe care se proiecteaza raza(pentru cadru)
	void calculateLineEcuation(int i, int j){
		float a, b, c;
		float d1, d2, d3, d4, min;

		a = piesa[i][j]->y3 - piesa[i][j]->y1;
		b = 0;
		c = piesa[i][j]->x1 * (piesa[i][j]->y1 - piesa[i][j]->y3);
		d1 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		min = d1;

		a = 0;
		b = piesa[i][j]->x1 - piesa[i][j]->x3;
		c = piesa[i][j]->y3 * (piesa[i][j]->x3 - piesa[i][j]->x1);
		d2 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		if (d2 < min)
			min = d2;

		a = piesa[i][j]->y3 - piesa[i][j]->y1;
		b = 0;
		c = piesa[i][j]->x3 * (piesa[i][j]->y1 - piesa[i][j]->y3);
		d3 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		if (d3 < min)
			min = d3;

		a = 0;
		b = piesa[i][j]->x1 - piesa[i][j]->x3;
		c = piesa[i][j]->y1 * (piesa[i][j]->x3 - piesa[i][j]->x1);
		d4 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		if (d4 < min)
			min = d4;

		if (min == d1 || min == d3)
			surface = 1;
		else
			surface = 0;
	}
	//calculeaza directia suprafetei pe care se proiecteaza raza(pentru obiecte)
	void calculateLineEcuationFrame(){
		float a, b, c;
		float d1, d2, d3, d4, min;

		a = cadru->y3 - cadru->y1;
		b = 0;
		c = cadru->x1 * (cadru->y1 - cadru->y3);
		d1 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		min = d1;

		a = 0;
		b = cadru->x1 - cadru->x3;
		c = cadru->y3 * (cadru->x3 - cadru->x1);
		d2 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		if (d2 < min)
			min = d2;

		a = cadru->y3 - cadru->y1;
		b = 0;
		c = cadru->x3 * (cadru->y1 - cadru->y3);
		d3 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		if (d3 < min)
			min = d3;

		a = 0;
		b = cadru->x1 - cadru->x3;
		c = cadru->y1 * (cadru->x3 - cadru->x1);
		d4 = abs(a * xc + b * yc + c) / (sqrt(a*a + b*b));
		if (d4 < min)
			min = d4;

		if (min == d1 || min == d3)
			surface = 1;
		else
			surface = 0;
	}
	//functie pentru detectare coliziune cu incadratorul
	int frameColision(){
		if (xc <= cadru->x1 || xc >= cadru->x3 || yc <= cadru->y1 || yc >= cadru->y3)
		{
			calculateLineEcuationFrame();
			printf("%d\n", surface);
			return 1;
		}
		else
			return 0;
	}
	
	//functie pentru detectare coliziune cu obiecte square
	int objectColision(){
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < size[i]; j++)
				if (xc >= piesa[i][j]->x1 && xc <= piesa[i][j]->x3 &&
					yc >= piesa[i][j]->y1 && yc <= piesa[i][j]->y3)
					{
						calculateLineEcuation( i, j);
					//	printf("%d\n", surface);
						return 1;
					}	
		}
		return 0;
	}
	//functie care determina coliziunea cu obiecte circle
	int circleColision(){
		for (int i = 9; i < 10; i++)
		{
			if (sqrt((xc - piesa[i][0]->x1) * (xc - piesa[i][0]->x1) +
				(yc - piesa[i][0]->y1) * (yc - piesa[i][0]->y1)) <= piesa[i][0]->raza)
				return 1;
		}
		return 0;
	}
	int computeNormRadius(){

		if (x3 == xc && y3 > yc && surface == 0)
		{
			x1 = xc + 1; y1 = yc + 5;
			x3 = xc + 1; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc + 1, yc, 1, 5);
			unghi = 0;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 9;
			printf("flag %d\n", flag);
			return 1;
		}
	
		if (x3 < xc && y3 > yc && surface == 0)
		{
			x1 = xc; y1 = yc + 5; 
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, 1, 5);
			transformation[cont] *= myTranslate(xc, yc) * myRotate(-unghi) * myTranslate(-xc, -yc);
			unghi = - unghi;
		    length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 1;
			printf("flag %d\n", flag);
			return 1;
		}
		if (x3 > xc && y3 > yc && surface == 0)
		{
			x1 = xc ; y1 = yc + 5;
			x3 = xc ; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, -1, 5);
			transformation[cont] *= myTranslate(xc, yc) * myRotate( 6.28 - unghi) * myTranslate(-xc, -yc);
			unghi = 6.28 - unghi;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 2;
			printf("flag %d\n", flag);
			return 1;
		}
		if (x3 < xc && y3 > yc && surface == 1)
		{
			x1 = xc - 5; y1 = yc;
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, -5, -1);
			transformation[cont] *= myTranslate(xc, yc) * myRotate(1.57 - unghi) * myTranslate(-xc, -yc);
			unghi = 1.57 - unghi;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 3;
			printf("flag %d\n", flag);
			return 1;
		}
		if (x3 < xc && y3 < yc && surface == 1)
		{
			x1 = xc - 5; y1 = yc;
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, -5, 1);
			transformation[cont] *= myTranslate(xc, yc) * myRotate(1.57 - unghi) * myTranslate(-xc, -yc);
			unghi =  1.57 - unghi;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 4;
			printf("flag %d x3 %f  y3 %f\n", flag, x3, y3);
			return 1;
		}
		if (x3 < xc && y3 < yc && surface == 0)
		{
			x1 = xc; y1 = yc - 5;
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, 1, -5);
			if (flag == 0)
			{
				transformation[cont] *= myTranslate(xc, yc) * myRotate(3.14 - unghi) * myTranslate(-xc, -yc);
				unghi = 3.14 - unghi;
			}
			if (flag == 1)
			{
				transformation[cont] *= myTranslate(xc, yc) * myRotate(- unghi) * myTranslate(-xc, -yc);
				unghi = - unghi;
			}
			if (flag == 8)
			{
				transformation[cont] *= myTranslate(xc, yc) * myRotate(1.57 - unghi) * myTranslate(-xc, -yc);
				unghi =  1.57 - unghi ;
			}

			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 5;
			printf("flag %d\n", flag);
			printf("unghi %f\n", unghi);
			return 1;
		}
		if (x3 > xc && y3 < yc && surface == 0)
		{
			x1 = xc; y1 = yc - 5;
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, -1, -5);
			transformation[cont] *= myTranslate(xc, yc) * myRotate(unghi - 1.57) * myTranslate(-xc, -yc);
			unghi =  unghi - 1.57;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 6;
			printf("flag %d\n", flag);
			return 1;
		}
		if (x3 > xc && y3 > yc && surface == 1)
		{
			x1 = xc + 5;  y1 = yc;
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, 5, -1);
			transformation[cont] *= myTranslate(xc, yc) * myRotate( - unghi + 4.71) * myTranslate(-xc, -yc);
			unghi = -unghi + 4.71;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 7;
			printf("flag %d\n", flag);
			return 1;
		}
		if (x3 > xc && y3 < yc && surface == 1)
		{
			x1 = xc + 5;  y1 = yc;
			x3 = xc; y3 = yc;
			raza[cont] = myLoadSquare_as_Mesh(xc, yc, 5, 1);
			transformation[cont] *= myTranslate(xc, yc) * myRotate( - 1.57 - unghi) * myTranslate(-xc, -yc);
			unghi =  - 1.57 - unghi;
			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
			xc = x1; yc = y1;
			flag = 8;
			printf("flag %d\n", flag);
			return 1;
		}
	}
	//--------------------------------------------------------------------------------------------
	//functii de cadru, apelate per FIECARE cadru ------------------------------------------------
	//--------------------------------------------------------------------------------------------
	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){}
	//functia de afisare
	void notifyDisplayFrame(){
		//per ECRAN
		//bufferele din framebuffer sunt aduse la valorile initiale (setate de clear color)
		//adica se sterge ecranul si se pune culoarea initiala (si alte propietati)
		glClearColor(0.9, 0.9, 0.9, 0.9);			//la ce culoare sterg
		glClear(GL_COLOR_BUFFER_BIT);		//comanda de stergere
		BLACKBOX.notifyDisplay();

		//setez spatiul de desenare relativ la spatiul ecranului
		width = lab::glut::getInitialWindowInformation().width;
		height = lab::glut::getInitialWindowInformation().height;

		//viewport 1
		glViewport(0, 0, width, height);

		viewx1 = 0;
		viewy1 = 0;
		widthview1 = width;
		heightview1 = height;
		

		cadru->setColor(0, 0, 0);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawPolyline(cadru); //polilinie
		
		/*
		camera->setColor(0, 0, 0);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(camera, 1); //mesh umplut
		
		for (int i = 0; i < 3; i++)
		{
			piesa[0][i]->setColor(0, 0.1, 1);
			BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
			BLACKBOX.drawMesh(piesa[0][i], 1); //mesh umplut	
		}
		
		piesa[1][0]->setColor(0.5, 0.5, 0.5);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[1][0], 1); //mesh umplut
		
		for (int i = 0; i < 3; i++)
		{
			piesa[2][i]->setColor(0.8, 0.3, 0);
			BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
			BLACKBOX.drawMesh(piesa[2][i], 1); //mesh umplut	
		}

		piesa[3][0]->setColor(1, 0.2, 0);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[3][0], 1); //mesh umplut
		
		piesa[4][0]->setColor(0.9, 0.5, 0.3);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[4][0], 1); //mesh umplut

		piesa[5][0]->setColor(0.5, 1, 0.6);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[5][0], 1); //mesh umplut

		piesa[6][0]->setColor(1, 0.2, 0.8);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[6][0], 1); //mesh umplut

		piesa[7][0]->setColor(0.3, 0.2, 1);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[7][0], 1); //mesh umplut

		piesa[8][0]->setColor(0.9, 0.2, 1);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[8][0], 1); //mesh umplut

		piesa[9][0]->setColor(1, 0, 0);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(piesa[9][0], 1); //mesh umplut
		*/

		sursa->setColor(1, 0.9, 0);
		BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*myIdentity());
		BLACKBOX.drawMesh(sursa, 1); //mesh umplut

		for (int i = 0; i < cont + 1; i++)
		{
			raza[i]->setColor(1, 0.9, 0);
			BLACKBOX.setModelMatrix(myTransform_viz_2D_unif(ferx1, fery1, widthfer1, heightfer1, viewx1, viewy1, widthview1, heightview1)*transformation[i]);
			BLACKBOX.drawMesh(raza[i], 1); //mesh umplut
		}
		if (frameColision() == 0)//  &&  objectColision() == 0  && circleColision() == 0)
		{
			if (flag == 0 || flag == 5)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale(1, (length + 0.1) / length) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 + (glm::sin(unghi) * length);
				yc = y3 - (glm::cos(unghi) * length);
			}
			if (flag == 1 || flag == 2 || flag == 9)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale(1, (length + 0.1) / length) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 - (glm::sin(unghi) * length);
				yc = y3 + (glm::cos(unghi) * length);
			
			}
			if (flag == 3)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale((length + 0.1) / length, 1) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 - (glm::cos(unghi) * length);
				yc = y3 - (glm::sin(unghi) * length);
			}
			if (flag == 4)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale((length + 0.1) / length, 1) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 - (glm::cos(unghi) * length);
				yc = y3 - (glm::sin(unghi) * length);
				printf("xc %f yc %f\n", xc, yc);
			}
			if (flag == 6)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale(1, (length + 0.1) / length) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 + (glm::sin(unghi) * length);
				yc = y3 - (glm::cos(unghi) * length);
			}
			if (flag == 7)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale((length + 0.1) / length, 1) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 + (glm::cos(unghi) * length);
				yc = y3 - (glm::sin(unghi) * length);
			}
			if (flag == 8)
			{
				transformation[cont] *= myTranslate(x3, y3) * myScale((length + 0.1) / length, 1) * myTranslate(-x3, -y3);
				length += 0.1;
				xc = x3 + (glm::cos(unghi) * length);
				yc = y3 + (glm::sin(unghi) * length);
			}
		}
		else
		{
			printf("xc %f yc %f\n", xc, yc);
			cont++;
			computeNormRadius();
		}	 
	}

	//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functie care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//blackbox needs to know
		BLACKBOX.notifyReshape(width,height);
	}
	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if (key_pressed == '1')
		{
			x1 = 70;
			y1 = 145;
			x3 = 70;
			y3 = 150;
			xc = x1;
			yc = y1;
			unghi = 0;
			cont = 0; flag = 0;

			transformation[0] = glm::mat3(1);
			transformation[1] = glm::mat3(1);

			first_press++;
			for (int i = 0; i < first_press; i++)
			{
				transformation[0] *= myTranslate(70, 150) * myRotate(0.1f) * myTranslate(-70, -150);
				unghi += 0.1f;
			}

			length = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3));
		}
	}
	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
		
	}
	//tasta speciala ridicata
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){}
	//drag cu mouse-ul
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	//am miscat mouseul (fara sa apas vreun buton)
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	//am apasat pe un boton
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
	//scroll cu mouse-ul
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ std::cout<<"Mouse scroll"<<std::endl;}

};

int main(){
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("Lab EGC 3 - transformari"),1000,600,300,50,true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,false,false);
	lab::glut::init(window, context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//_DUPA_ GLEW ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Lab mylab;
	lab::glut::setListener(&mylab);

	//run
	lab::glut::run();
	return 0;
}

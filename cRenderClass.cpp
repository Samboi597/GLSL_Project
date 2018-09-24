// cRenderClass.cpp
// 
//////////////////////////////////////////////////////////////////////////////////////////
// includes 
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Declarations 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass graphics( SCREEN_WIDTH, SCREEN_HEIGHT );

unsigned int	m_tex[256];
float			angle = 0.0f, fromX, fromY;
bool			pointSelected = false;
int				selectedI, selectedJ;

//////////////////////////////////////////////////////////////////////////////////////////
// cRenderClass() - constructor 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass::cRenderClass(const int w, const int h)
{
	m_sw = w;
	m_sh = h;

	int total_count = w*h*4;

	m_buffer = new float[total_count];

	for( int i=0; i<total_count; i++ )
	{
		m_buffer[i] = 1;
	}

	m_point_size = 1;

	m_OPENGL_2_0_SUPPORTED = true;

	// generate a number of textures...
	glGenTextures(1, m_tex);

	for (int i = 0; i < 4; i++) //s
	{
		for (int j = 0; j < 4; j++) //t
		{
			points[i][j].x = i * 100;
			points[i][j].y = j * 100;
			if (j == 1 || j == 2)
			{
				points[i][j].z = 200;
			}
			else
			{
				points[i][j].z = 0;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// cRenderClass() - destructor 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass::~cRenderClass()
{
	delete[] m_buffer;
}

//////////////////////////////////////////////////////////////////////////////////////////
// loop() - enters game loop
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::loop()
{
	glutMainLoop();
}

extern cShaderInfo	ShaderInfo;

//////////////////////////////////////////////////////////////////////////////////////////
// initShaders() - init GLSL shaders
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::initShaders()
{
	GLenum err = glewInit();

	if( GLEW_OK != err )
	{
		printf("Error: Glew is not supported\n\n");
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		printf("OpenGL 2.0 is supported\n\n");
	}
	else
	{
		printf("Error: OpenGL 2.0 is not supported\n\n");
		m_OPENGL_2_0_SUPPORTED = false;
	}

	ShaderInfo.create();
}

bool cRenderClass::testIntersection(ControlPoint point, float ox, float oy, float oz, float dx, float dy, float dz)
{
	ControlPoint min, max;

	//10.0f is based upon the size of the point in the scene
	min.x = point.x - 10.0f;
	min.y = point.y - 10.0f;
	min.z = point.z - 10.0f;

	max.x = point.x + 10.0f;
	max.y = point.y + 10.0f;
	max.z = point.z + 10.0f;

	float tXmin, tXmax;
	if (dx >= 0.0f)
	{
		tXmin = (min.x - ox) / dx;
		tXmax = (max.x - ox) / dx;
	}
	else
	{
		tXmin = (max.x - ox) / dx;
		tXmax = (min.x - ox) / dx;
	}

	float tYmin, tYmax;
	if (dy >= 0.0f)
	{
		tYmin = (min.y - oy) / dy;
		tYmax = (max.y - oy) / dy;
	}
	else
	{
		tYmin = (max.y - oy) / dy;
		tYmax = (min.y - oy) / dy;
	}

	if (tXmin > tYmax || tYmin > tXmax) return false;

	float tmin;
	if (tXmin > tYmin)
		tmin = tXmin;
	else
		tmin = tYmin;

	float tmax;
	if (tXmax < tYmax)
		tmax = tXmax;
	else
		tmax = tYmax;

	float tZmin, tZmax;
	if (dz >= 0.0f)
	{
		tZmin = (min.z - oz) / dz;
		tZmax = (max.z - oz) / dz;
	}
	else
	{
		tZmin = (max.z - oz) / dz;
		tZmax = (min.z - oz) / dz;
	}

	if (tmin > tZmax || tZmin > tmax) return false;

	if (tZmin > tmin) tmin = tZmin;
	if (tZmax < tmax) tmax = tZmax;
	return true;
}

void cRenderClass::onMouseClick(int button, int state, int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	if (state == GLUT_UP)
	{
		pointSelected = false;
		return;
	}

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;

	gluUnProject(winX, winY, 0, modelview, projection, viewport, &posX, &posY, &posZ);
	ControlPoint start;
	start.x = posX; start.y = posY; start.z = posZ;

	float rayOX = start.x, rayOY = start.y, rayOZ = start.z;

	gluUnProject(winX, winY, 1, modelview, projection, viewport, &posX, &posY, &posZ);
	ControlPoint end;
	end.x = posX; end.y = posY; end.z = posZ;

	//normalise vector direction
	float rayDX = end.x - start.x, rayDY = end.y - start.y, rayDZ = end.z - start.z;
	float length = sqrt(rayDX * rayDX + rayDY * rayDY + rayDZ * rayDZ);
	rayDX /= length; rayDY /= length; rayDZ /= length;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (testIntersection(points[i][j], rayOX, rayOY, rayOZ, rayDX, rayDY, rayDZ))
				{
					pointSelected = true;
					selectedI = i;	selectedJ = j;
					fromX = winX;	fromY = winY;
				}
				//if there is a positive collision between any of them
				//set a boolean value for selection to true
				//find the coordinate with the smallest t-value
				//record the location of the selected value
			}
		}
		break;
	}
}

void cRenderClass::onMouseDrag(int x, int y)
{
	GLfloat winX, winY;
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble posX, posY, posZ;
	GLdouble stepxX, stepxY, stepxZ, stepyX, stepyY, stepyZ;

	if (pointSelected)
	{
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;

		gluUnProject(winX, winY, 1, modelview, projection, viewport, &posX, &posY, &posZ);
		stepxX = posX;	stepxY = posY;	stepxZ = posZ;
		stepyX = posX;	stepyY = posY;	stepyZ = posZ;

		//how much do the unprojected x, y and z coordinates change
		//after the x- and y-components of the screen coordinates have been incremented by one?
		gluUnProject(winX + 1, winY, 1, modelview, projection, viewport, &posX, &posY, &posZ); //positive x-step
		stepxX = posX - stepxX;	stepxY = posY - stepxY;	stepxZ = posZ - stepxZ;

		gluUnProject(winX, winY + 1, 1, modelview, projection, viewport, &posX, &posY, &posZ); //positive y-step
		stepyX = posX - stepyX;	stepyY = posY - stepyY;	stepyZ = posZ - stepyZ;

		float xDiff = winX - fromX, yDiff = winY - fromY;

		points[selectedI][selectedJ].x += ((stepxX * xDiff) + (stepyX * yDiff));
		points[selectedI][selectedJ].y += ((stepxY * xDiff) + (stepyY * yDiff));
		points[selectedI][selectedJ].z += ((stepxZ * xDiff) + (stepyZ * yDiff));

		fromX = winX;	fromY = winY;
	}
}

void motion(int x, int y)
{
	graphics.onMouseDrag(x, y);
}

void mouse(int button, int state, int Mx, int My)
{
	graphics.onMouseClick(button, state, Mx, My);
}

//////////////////////////////////////////////////////////////////////////////////////////
// initialize glut stuff
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::create(int argc, _TCHAR* argv[])
{
	// initialise the glut library
	glutInit(&argc, argv);

	// set up the initial display mode
	// need both double buffering and z-buffering
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	
	// set the initial window position
	glutInitWindowPosition(100, 100);

	// set the initial window size
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// create and name the window
	glutCreateWindow("Using GLSL Shaders for Post Processing (Multi-Pass)!");
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	//mouse functions
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// reshape callback for current window
	glutReshapeFunc(winReshapeFunc);

	// set display callback for current window
	glutDisplayFunc(renderScene);	

	// set up the global idle callback
	glutIdleFunc(update);

	initShaders();	
}

//////////////////////////////////////////////////////////////////////////////////////////
// drawPixel() - draw a pixel into an off-screen buffer
//				 if m_point_size>1 then draw a block of pixels
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::drawPixel(int x, int y)
{
	int start, end;

	start = -(m_point_size/2);
	end	= (int)((m_point_size/2.0f) + 0.5);

	for( int i=start; i<end; i++ )
	{
		for( int j=start; j<end; j++ )
		{
			// reject values outside m_buffer range
			if( (x+j < 0) || (x+j >= m_sw) )
				continue;

			if( (y+i < 0) || (y+i >= m_sh) )
				continue;

			m_buffer[(((y+i)*m_sw+(x+j))*4) + 0] = m_colour.r;
			m_buffer[(((y+i)*m_sw+(x+j))*4) + 1] = m_colour.g;
			m_buffer[(((y+i)*m_sw+(x+j))*4) + 2] = m_colour.b;
		}
	}
}

unsigned char* cRenderClass::loadImage(const char * filepath, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE* file;
	BITMAPFILEHEADER bitmapFileHeader;
	unsigned char* result;

	file = fopen(filepath, "rb");
	if (file == NULL)
	{
		printf("Cannot open file\n");
		return NULL;
	}

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, file);

	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != 0x4D42)
	{
		printf("Not a bmp file\n");
		fclose(file);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	// move file pointer to beginning of bitmap data
	fseek(file, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	result = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// verify memory allocation
	if (!result)
	{
		printf("Insufficient memory allocation\n");
		free(result);
		fclose(file);
		return NULL;
	}

	// read in the bitmap image data
	fread(result, 1, bitmapInfoHeader->biSizeImage, file);

	// make sure bitmap image data was read
	if (result == NULL)
	{
		printf("Data read unsuccessful\n");
		fclose(file);
		return NULL;
	}

	// close the file and return the bitmap image data
	fclose(file);
	return result;
}

ControlPoint cRenderClass::findVertex(float _s, float _t)
{
	float matA[4];
	matA[0] = (pow((1 - _t), 3.0f)) / 6.0f;
	matA[1] = ((3 * (_t * _t * _t)) - (6 * (_t * _t)) + 4) / 6.0f;
	matA[2] = ((-3 * (_t * _t * _t)) + (3 * (_t * _t)) + (3 * _t) + 1) / 6.0f;
	matA[3] = (_t * _t * _t) / 6.0f;

	float matC[4];
	matC[0] = (pow((1 - _s), 3.0f)) / 6.0f;
	matC[1] = ((3 * (_s * _s * _s)) - (6 * (_s * _s)) + 4) / 6.0f;
	matC[2] = ((-3 * (_s * _s * _s)) + (3 * (_s * _s)) + (3 * _s) + 1) / 6.0f;
	matC[3] = (_s * _s * _s) / 6.0f;

	float matBx[4], matBy[4], matBz[4];
	for (int i = 0; i < 4; i++)
	{
		matBx[i] = matA[0] * points[0][i].x + matA[1] * points[1][i].x + matA[2] * points[2][i].x + matA[3] * points[3][i].x;
		matBy[i] = matA[0] * points[0][i].y + matA[1] * points[1][i].y + matA[2] * points[2][i].y + matA[3] * points[3][i].y;
		matBz[i] = matA[0] * points[0][i].z + matA[1] * points[1][i].z + matA[2] * points[2][i].z + matA[3] * points[3][i].z;
	}

	ControlPoint returnVert;
	returnVert.x = matBx[0] * matC[0] + matBx[1] * matC[1] + matBx[2] * matC[2] + matBx[3] * matC[3];
	returnVert.y = matBy[0] * matC[0] + matBy[1] * matC[1] + matBy[2] * matC[2] + matBy[3] * matC[3];
	returnVert.z = matBz[0] * matC[0] + matBz[1] * matC[1] + matBz[2] * matC[2] + matBz[3] * matC[3];

	return returnVert;
}

//////////////////////////////////////////////////////////////////////////////////////////
// render() - 
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::render()
{
	float camPosX = 300.0f, camPosY = 300.0f, camPosZ = 300.0f;

	if (angle < 720.0f)
		angle += 1.0f;

	// disable shader program
	glUseProgram(0);

	// clear the back buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camPosX, camPosY, camPosZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	glScalef(1.0f, 1.0f, 1.0f);

	float matAmbient[] = { 0.10f, 0.10f, 0.10f, 1.0f };
	float matDiff[] = { 0.50f, 0.50f, 0.50f, 1.0f };
	float matSpec[] = { 0.40f, 0.40f, 0.40f, 1.0f };

	glEnable(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);

	float ambientLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightPosition[] = { 150.0f, 150.0f, 250.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////

	glEnable(GL_TEXTURE_2D);
	bitmapData = loadImage("unionjack.bmp", &bitmapInfoHeader);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
		bitmapInfoHeader.biHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bitmapData);

	// render selected texture map here..
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//interpolate s and t values based upon number of polygons
	//apply surface equation for x, y and z components

	cShader *pList = ShaderInfo.getList();
	int shdr = 0;

	if (shdr >= ShaderInfo.shaderCount())
	{
		printf("Error: Can't access requested shader\n");
		shdr = 0;
	}

	// enable shader program..
	glUseProgram(pList[shdr].program());

	glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);
	glUniform3f(pList[shdr].cameraPos(), camPosX, camPosY, camPosZ);

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);//set backdrop material properties
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);

	glActiveTexture(GL_TEXTURE0 + m_tex[0]);

	glLineWidth(1);

	/////////////////////////////////////////////////////////////
	// draw to screen using OpenGL standard calls
	/////////////////////////////////////////////////////////////

	//find vertices
	float xMax = -100000.0f, xMin = 100000.0f, yMax = -100000.0f, yMin = 100000.0f, zMax = -100000.0f, zMin = 100000.0f;
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 21; j++)
		{
			float s = i / 20.0f;
			float t = j / 20.0f;

			vertices[i][j] = findVertex(s, t);
			if (vertices[i][j].x > xMax) xMax = vertices[i][j].x;
			if (vertices[i][j].x < xMin) xMin = vertices[i][j].x;

			if (vertices[i][j].y > yMax) yMax = vertices[i][j].y;
			if (vertices[i][j].y < yMin) yMin = vertices[i][j].y;

			if (vertices[i][j].z > zMax) zMax = vertices[i][j].z;
			if (vertices[i][j].z < zMin) zMin = vertices[i][j].z;
		}
	}

	//find central point
	ControlPoint centerPoint;
	centerPoint.x = (xMax + xMin) / 2;
	centerPoint.y = (yMax + yMin) / 2;
	centerPoint.z = (zMax + zMin) / 2;

	glPushMatrix();
	glTranslatef(centerPoint.x, centerPoint.y, centerPoint.z);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-centerPoint.x, -centerPoint.y, -centerPoint.z);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			//draw two polygons for each iteration of j
			//one for (0, 0), (1, 1), (0, 1)
			//another for (0, 0), (1, 0), (1, 1)

			float s = i / 20.0f;
			float t = j / 20.0f;
			
			//draw triangle
			glBegin(GL_TRIANGLES);
			glColor3f(0.0, 1.0, 0.0);
			
			glTexCoord2f(s, t);					glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
			glTexCoord2f(s + 0.05f, t + 0.05f);	glVertex3f(vertices[i + 1][j + 1].x, vertices[i + 1][j + 1].y, vertices[i + 1][j + 1].z);
			glTexCoord2f(s, t + 0.05f);			glVertex3f(vertices[i][j + 1].x, vertices[i][j + 1].y, vertices[i][j + 1].z);

			glEnd();
		
			//draw triangle
			glBegin(GL_TRIANGLES);
			glColor3f(0.0, 1.0, 0.0);

			glTexCoord2f(s, t);					glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
			glTexCoord2f(s + 0.05f, t);			glVertex3f(vertices[i + 1][j].x, vertices[i + 1][j].y, vertices[i + 1][j].z);
			glTexCoord2f(s + 0.05f, t + 0.05f);	glVertex3f(vertices[i + 1][j + 1].x, vertices[i + 1][j + 1].y, vertices[i + 1][j + 1].z);

			glEnd();
		}
	}
	glPopMatrix();
	
	//draw control points
	glPointSize(10.0);
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
		}
	}
	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);

	glutSwapBuffers();

	// clear out the temp buffer		

	clear();
	delete[] bitmapData;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// winReshapeFunc() - gets called initially and whenever the window get resized
// resizing has been locked
//////////////////////////////////////////////////////////////////////////////////////////
void winReshapeFunc(GLint w, GLint h)
{
	// specify current matrix
	glMatrixMode(GL_PROJECTION);

	// load an identity matrix
	glLoadIdentity();	

	// create a projection matrix... i.e. 2D projection onto xy plane
	//glOrtho( -SCREEN_WIDTH, SCREEN_WIDTH, -SCREEN_HEIGHT, SCREEN_HEIGHT, -100, 100);
	gluPerspective(100.0, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);  //redefine projection matrix to match changes in w and h

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set up the viewport
	glViewport
	(
		0,				// lower left x position
		0,				// lower left y position
		(GLsizei) SCREEN_WIDTH,	// viewport width
		(GLsizei) SCREEN_HEIGHT	// viewport height
	);
}

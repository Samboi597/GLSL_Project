#include "stdafx.h"

#define SHADER_COUNT	1

char *vShader[] = {	"shaders\\intensity.vert",	// FLAG_MAPS_INTENSITYS
};

char *fShader[] = {	"shaders\\intensity.frag",	// FLAG_MAPS_INTENSITY
};


extern cRenderClass graphics;


//////////////////////////////////////////////////////////////////////////////////////////
// cShaderInfo() - constructor
//////////////////////////////////////////////////////////////////////////////////////////
cShaderInfo::cShaderInfo()
{
	m_num_shaders = SHADER_COUNT;
	m_pList = NULL;
	m_flags = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// clean() - delete shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::clean(char *pVertexShader_path, char *pFragmentShader_Path)
{
	if( !graphics.OpenGL2_0() )
		return;

	if( m_v )
		glDeleteShader(m_v); 
	if( m_f )
		glDeleteShader(m_f); 
	if( m_program )
		glDeleteProgram(m_program);
}

//////////////////////////////////////////////////////////////////////////////////////////
// create() - creates a new shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::create(char *pVertexShader_path, char *pFragmentShader_Path)
{
	if( !graphics.OpenGL2_0() )
		return;

	char *vs = NULL, *fs = NULL;

	m_v  = glCreateShader(GL_VERTEX_SHADER);
	m_f  = glCreateShader(GL_FRAGMENT_SHADER);

	_getcwd(m_cwd, _MAX_PATH);
	
	// store vertex path..
	strcpy( m_vpath, m_cwd );
	strcat( m_vpath, "\\" );
	strcat( m_vpath, pVertexShader_path );

	// store shader path..
	strcpy( m_fpath, m_cwd );
	strcat( m_fpath, "\\" );
	strcat( m_fpath, pFragmentShader_Path );

	vs = textFileRead(m_vpath);
	fs = textFileRead(m_fpath);

	if( !vs )
	{
		printf("\n\nERROR: file: %s not found -  ", m_vpath);
		return;
	}
	if( !fs )
	{
		printf("\n\nERROR: file: %s not found -  ", m_fpath);
		return;
	}


	const char *vv = vs;
	const char *ff = fs;

	glShaderSource(m_v, 1, &vv, NULL );
	glShaderSource(m_f, 1, &ff, NULL );

	free(vs);
	free(fs);

	glCompileShader(m_v);
	glCompileShader(m_f);

	m_program = glCreateProgram();
	glAttachShader(m_program, m_v);
	glAttachShader(m_program, m_f); 

	glLinkProgram(m_program);

	m_grabLoc			= glGetUniformLocation(m_program, "grabTexture");
	m_cameraLocation	= glGetUniformLocation(m_program, "cameraPos");
	
//	m_MVMatrix			= glGetUniformLocation(m_program, "MVMatrix" );
//	m_MVPMatrix			= glGetUniformLocation(m_program, "MVPMatrix" );
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
// create() - allocate block of memory to hold ALL shaders
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::create()
{
	m_num_shaders = SHADER_COUNT;

	m_pList = new cShader[SHADER_COUNT];	

	for( int i=0; i<SHADER_COUNT; i++ )
	{
		list(i)->create( vShader[i], fShader[i] );	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// clean() - delete an existing shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::clean()
{
	m_num_shaders = SHADER_COUNT;

	if( m_pList )
	{
		for( int i=0; i<SHADER_COUNT; i++ )
		{
			list(i)->clean( vShader[i], fShader[i] );	
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// setOGL2support() - sets flag for OpenGL 2.0 support
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::setOGL2support(bool value)
{
	graphics.OpenGL2_0(value);
}

//////////////////////////////////////////////////////////////////////////////////////////
// isOGL2supported() - query OpenGL 2.0 support
//////////////////////////////////////////////////////////////////////////////////////////
bool cShaderInfo::isOGL2supported()
{
	return graphics.OpenGL2_0();
}

//////////////////////////////////////////////////////////////////////////////////////////
// list() - returns requested element from shader list
//////////////////////////////////////////////////////////////////////////////////////////
cShader *cShaderInfo::list(int id)
{
	if( id >= m_num_shaders )
	{
		// if you get here, then you are trying to access a shader element that exceeds the shader array size.
		// try increasing the number_of_shaders created in main() - ShaderInfo.create(number_of_shaders);
		printf("ERROR: ");
		printf("array limits exceeded\n");
	}

	return &m_pList[id];
}


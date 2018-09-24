//////////////////////////////////////////////////////////////////////////////////////////
// cShader.h
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class cShader
{
public:

	cShader(){}
	~cShader(){}

	inline void		cwd(char *path){ strcpy( m_cwd, path );}
	inline char		*cwd(){return m_cwd;}
	
	inline u32		program()		{ return m_program; }
	inline GLint	get_grabLoc()	{ return m_grabLoc; }
	inline GLint	cameraPos()		{ return m_cameraLocation; }
	inline GLint	MVMatrix()		{ return m_MVMatrix; }
	inline GLint	MVPMatrix()		{ return m_MVPMatrix; }

	void clean(char *pVertexShader_path, char *pFragmentShader_Path);
	void create(char *pVertexShader_path, char *pFragmentShader_Path);

private:
	char m_vpath[256];
	char m_fpath[256];
	char m_cwd[256];

	GLuint m_v;
	GLuint m_f;

	u32 m_program;

	GLint	m_grabLoc;
	GLint	m_cameraLocation;
	GLint	m_MVMatrix;
	GLint	m_MVPMatrix;
};


class cShaderInfo
{
public:

	cShaderInfo();
	~cShaderInfo()
	{
	//	delete m_pList;
	}
	void create();//int count);
	void clean();

	inline cShader	*getList(){	return m_pList;}

	cShader *list(int id);

	void setOGL2support(bool value);
	bool isOGL2supported();

	void set_flags(u32 value)
	{
		m_flags = value;
	}

	u32 get_flags()
	{
		return m_flags;
	}

	inline void shaderCount(int count){ m_num_shaders = count;}
	inline int  shaderCount(){return m_num_shaders;}
	

private:
	cShader		*m_pList;

	int			m_num_shaders;
	u32			m_flags;		// used to render individual maps
	
//	bool		m_OpenGL2_supported;
};
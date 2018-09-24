struct ControlPoint
{
	float x, y, z;
};

class cRenderClass
{
public:

	cRenderClass(const int, const int);
	~cRenderClass();

	void create(int argc, _TCHAR* argv[]);
	void initShaders();
	void loop();

	void render();
	void drawPixel(int, int);	
	unsigned char* loadImage(const char* filepath, BITMAPINFOHEADER *bitmapInfoHeader);
	ControlPoint findVertex(float, float);
	bool testIntersection(ControlPoint point, float ox, float oy, float oz, float dx, float dy, float dz);
	void onMouseClick(int button, int state, int x, int y);
	void onMouseDrag(int x, int y);

	inline void clear(){memset( m_buffer, 0, sizeof(float)*m_sw*m_sh*4 );}
	inline void pointSize(int size){m_point_size = size;}
	inline void colour(float r, float g, float b)
	{
		m_colour.r = r;
		m_colour.g = g;
		m_colour.b = b;
	}

	inline void OpenGL2_0(bool value){m_OPENGL_2_0_SUPPORTED = value;}
	inline bool OpenGL2_0(){return m_OPENGL_2_0_SUPPORTED;}
	
	class cShaderInfo	ShaderInfo;

private:

	int					m_sw, m_sh;
	int					m_point_size;
	float				*m_buffer;
	sRGB				m_colour;
	bool				m_OPENGL_2_0_SUPPORTED;
	BITMAPINFOHEADER	bitmapInfoHeader;
	unsigned char*		bitmapData;
	ControlPoint		points[4][4];
	ControlPoint		vertices[21][21];
};
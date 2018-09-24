// colour vertex shader
// Sam Mounter
// Duck Meatballs Productions 2017

uniform vec3	cameraPos;

varying vec2 	texCoord;
varying vec3	N;
varying vec3	V;
varying vec3	L;
varying vec3	C;

void main(void)
{		
	// Transforming The Vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 
    // Passing The Texture Coordinate Of Texture Unit 0 To The Fragment Shader
    texCoord = vec2(gl_MultiTexCoord0);
		
	//find the position of the vertex in the world
	V = vec3(gl_ModelViewMatrix * gl_Vertex); 

	//find the normal relative to the shape
	N = normalize(gl_NormalMatrix * gl_Normal);
	
	//find the light direction
	L = normalize(gl_LightSource[0].position.xyz - V); 
	
	//find the camera direction
	C = normalize(cameraPos - V);
}
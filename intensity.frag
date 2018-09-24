// colour fragment shader
// Sam Mounter
// Duck Meatballs Productions 2017

uniform sampler2D	grabTexture;
uniform vec3		cameraPos;

varying vec2 		texCoord;
varying vec3		N;
varying vec3		V;
varying vec3		L;
varying vec3		C;

void main(void)
{
	//ambient component
	vec4 Iamb = gl_FrontLightProduct[0].ambient;  

	//diffuse component
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * clamp(dot(N, L), 0.0, 1.0);
	
	//specular component
	vec4 Ispec = vec4(0.0, 0.0, 0.0, 1.0);
	
	//a specular reflection will only be recorded if
	//the surface is within range of the light source
	if (dot(N, L) > 0)
	{
		//halfway vector
		vec3 H = normalize(L + C);
		Ispec = gl_FrontLightProduct[0].specular * pow(dot(N, H), gl_FrontMaterial.shininess); 
	}
	
	//colour the pixel
	gl_FragColor = texture2D(grabTexture, texCoord) * (Iamb + Idiff + Ispec);
}
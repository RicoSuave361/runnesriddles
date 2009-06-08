uniform float time;
uniform float r1;

uniform int activeLight[5];
uniform int fogA;
uniform int text;

varying vec3 V;
varying vec3 L[5];
varying vec3 N;


void main()
{
	vec4 vertex = vec4(gl_Vertex);
	float rnd = cos(time/5);
	
	
	vertex.x =   (0.25f*vertex.y * vertex.y * abs(rnd*rnd+vertex.z/5))+vertex.x;  
	
	vec3 vertexPos = vec3(gl_ModelViewMatrix * vertex);
    V = -normalize(vertexPos);
    for(int i=0;i<5;++i){
   		if(activeLight[i]==1) 
    		L[i] = normalize(vec3(gl_LightSource[i].position) - vertexPos);
    }
    N = (gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    gl_FogFragCoord = gl_Position.z;
    gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
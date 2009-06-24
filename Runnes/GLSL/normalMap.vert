uniform int activeLight[5];
uniform int fogA;
uniform int text;


varying vec3 lightDir[3];
varying vec3 halfVector[3];

void main()
{
    vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FogFragCoord = gl_Position.z;
    gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
    vec3 t = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);
    vec3 b = cross(n, t) * gl_MultiTexCoord1.w;
    
    mat3 tbnMatrix = mat3(t.x, b.x, n.x,
                          t.y, b.y, n.y,
                          t.z, b.z, n.z);
                          
    for(int i=0;i<3;++i){
   		if(activeLight[i]==1){
		    lightDir[i] = gl_LightSource[i].position.xyz;
		    lightDir[i] = tbnMatrix * lightDir[i];
		    
		    halfVector[i] = gl_LightSource[i].halfVector.xyz;
		    halfVector[i] = tbnMatrix * halfVector[i];
    	
    	}
    }
}

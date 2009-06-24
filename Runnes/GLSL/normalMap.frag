varying vec3 lightDir[3];
varying vec3 halfVector[3];

uniform int activeLight[5];
uniform int fogA;
uniform int text;
uniform sampler2D texture;
uniform sampler2D normalMap;


void main()
{

    vec3 n = normalize(texture2D(normalMap, gl_TexCoord[0].st).xyz * 2.0 - 1.0);
    vec3 l[5];
    vec3 h[5];
    float nDotL[5];
    float nDotH[5];
    float power[5];
    vec4 amb;
	vec4 diff;
	vec4 spec;
	vec4 sum;
    for(int i=0;i<3;++i){
    	l[i] = normalize(lightDir[i]);
    	h[i] = normalize(halfVector[i]);
        nDotL[i] = max(0.0, dot(n, l[i]));
 	    nDotH[i] = max(0.0, dot(n, h[i]));
 	    power[i] = (nDotL[i] == 0.0) ? 0.0 : pow(nDotH[i], gl_FrontMaterial.shininess);
            if(activeLight[i]==1)
			{
				amb = gl_FrontLightProduct[i].ambient;
		   		diff= gl_FrontLightProduct[i].diffuse * nDotL[i];
		   		spec= gl_FrontLightProduct[i].specular * power[i];
				sum=sum+amb+diff+spec;
            }
    }

    if(text==1) 
		sum=texture2D(texture,gl_TexCoord[0].st)*sum;
 	float fog = 1.0;
    if(fogA==1)
	{
    	fog = ((gl_Fog.end - gl_FogFragCoord)+0.5) * gl_Fog.scale;
        fog = clamp(fog, 0.0, 1.0);
        if (fog >= 0.0)
        	gl_FragColor = mix(gl_Fog.color,sum, fog);
        else
        	gl_FragColor = gl_Fog.color;
    }else{
    	gl_FragColor=sum;
    }

}

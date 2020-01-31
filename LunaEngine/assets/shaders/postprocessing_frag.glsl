#version 420

// Vertex Shader Input
in vec4 fColour;	
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

// Object
uniform vec4 diffuseColour;
uniform vec4 specularColour;
uniform vec4 eyeLocation;

// Used to draw debug (or unlit) objects
uniform bool isUniform;
uniform bool isSkybox;

// Texture
uniform sampler2D textSamp00;
uniform sampler2D textSamp01;
uniform sampler2D textSamp02;
uniform sampler2D textSamp03;
uniform vec4 tex_0_3_ratio;		// x = 0, y = 1, z = 2, w = 3
uniform samplerCube skyBox;


// Globals
in float fiTime;
in float fisWater;
uniform vec2 iResolution;



out vec4 pixelColour;			// RGB A   (0 to 1) 

// Fragment shader
struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

// Lights
const int LIGHT_BUFFER = 24;
uniform int NUMBEROFLIGHTS;
uniform sLight theLights[LIGHT_BUFFER];

const int num_samples = 9;

vec4 Blur()
{

	vec4 rgba = vec4(0.0);
	float offset = 0.01;

	vec3 offsets[num_samples] = {
		vec3(0., 0., 0.5),
		vec3(offset, 0., 0.1),
		vec3(0., offset, 0.1),
		vec3(-offset, 0., 0.1),
		vec3(0., -offset, 0.1),
		vec3(offset * 2.0, 0., 0.05),
		vec3(0., offset * 2.0, 0.05),
		vec3(-offset * 2.0, 0., 0.05),
		vec3(0., -offset * 2.0, 0.05),
	};

	for (int i = 0; i < num_samples; ++i)
	{
		rgba += texture( textSamp00, fUVx2.st + offsets[i].xy ).rgba * offsets[i].z;
	}

	rgba.a = 1.0;

	return rgba;
}

vec4 NoEffect()
{
	vec4 tex = texture( textSamp00, fUVx2.st ).rgba;
	tex.a = 1.0;
	return tex;
}

//vec4 BokehBlur()
//{
//	vec4 colour = vec4(0.0);
//
//	float max_radius = 0.05;
//
//	float weight = 0.;//vec4(0.,0.,0.,0.);  
//    int radius = int(max_radius);  
//    for(int x=radius*-1;x<radius;x++) {  
//        for(int y=radius*-1;y<radius;y++){  
//            vec2 coord = gl_TexCoord[0].xy+vec2(x,y);  
//            if(distance(coord, gl_TexCoord[0].xy) < float(radius)){  
//                vec4 texel = texture2DRect(tex, coord);  
//                float w = length(texel.rgb)+0.1;  
//                weight+=w;  
//                finalColor += texel*w;  
//            }  
//        }  
//    }  
//  
//    colour finalColor/weight; 
//
//	return colour;
//}


void main()  
{

	pixelColour = vec4(0.);
	pixelColour += Blur();

	return;
	
} // end main

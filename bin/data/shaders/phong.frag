#version 150

in vec4 pos;
in vec3 norm;
in float zHeight;

uniform vec4 LightPosition;
out vec4 fragColor;

//-------------------
//Phong stuff

//Material properties
uniform vec3 Kd; //Diffuse Reflectivity
uniform vec3 Ka; //Ambient Reflectivity
uniform vec3 Ks; //Specular Refelctivity
uniform float Shininess;

//Light properties
uniform vec3 La; //Ambient Light Intensity
uniform vec3 Ld; //Diffuse Light Intensity
uniform vec3 Ls; //Specular Light Intensity
//-------------------

// calculate the color
vec3 phongModel(vec4 thePos, vec3 theNorm)
{
    vec3 s = normalize(vec3(LightPosition - thePos));
    vec3 v = normalize(- thePos.xyz);
    vec3 r = reflect(- s, theNorm);
    
    vec3 ambient = La * Ka;
    float sDotN = max(dot(s, theNorm), 0.0);
    vec3 diffuse = Ld * Kd * sDotN;
    
    vec3 spec = vec3(0.0);
    if(sDotN > 0)
    {
        spec = Ls * Ks * pow(max(dot(r, v), 0.0), Shininess);
    }
    
    return (ambient + diffuse + spec) * ((zHeight / 100) * 1); //ADS shading model multiplied with mapped vertex height;
}

void main()
{
    fragColor = vec4(phongModel(pos, norm), 1.0);
}
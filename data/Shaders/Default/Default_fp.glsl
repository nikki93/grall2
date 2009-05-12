//Constants.
const int NUM_LIGHTS = 3;

//Common.
varying vec3 v_N;
varying vec3 eyeCoord;

//Params.
uniform vec3 l_ambient;
uniform float l_distances[3];
uniform vec3 l_attens[3];

//uniform vec3 m_ambient;
//uniform vec3 m_diffuse;
//uniform vec3 m_specular;
//uniform float m_shininess;

void main () 
{
    vec3 N = normalize(v_N);
    vec3 L, H;
    float atten;

    vec3 ambient = gl_FrontMaterial.ambient * l_ambient;
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        L = normalize(gl_LightSource[i].position.xyz);
        H = normalize(gl_LightSource[i].halfVector.xyz);

        float d = length(gl_LightSource[i].position.xyz - eyeCoord);
        //atten = 1.0 / l_attens[i].x + l_attens[i].y * d + l_attens[i].z * d*d;
        atten = 1;

        diffuse += gl_LightSource[i].diffuse * atten * max(dot(L, N), 0.0);
        specular += gl_LightSource[i].specular * atten * vec3(1.0, 1.0, 1.0) * pow(max(dot(H, N), 0.0), 16.0);
    }

    diffuse *= gl_FrontMaterial.diffuse;
    specular *= gl_FrontMaterial.specular;

    gl_FragColor = vec4(l_distances[2]);
}


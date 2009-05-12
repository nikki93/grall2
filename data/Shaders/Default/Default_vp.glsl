//Common.
varying vec3 v_N;
varying vec3 eyeCoord;

void main () 
{
    eyeCoord = vec3(gl_ModelViewMatrix * gl_Vertex);
    gl_Position = ftransform();
    v_N = gl_NormalMatrix * gl_Normal;
}

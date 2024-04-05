out vec4 FragColor;

in vec3 localPos;
  
uniform samplerCube skyboxMap;
  
void main()
{
    vec3 envColor = texture(skyboxMap, localPos).rgb;
    
    FragColor = vec4(envColor, 1.0);
}
out vec4 FragColor;

in vec3 localPos;
  
uniform samplerCube skyboxMap;
  
void main()
{
    vec3 envColor = texture(skyboxMap, localPos).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
  
    FragColor = vec4(envColor, 1.0);
}
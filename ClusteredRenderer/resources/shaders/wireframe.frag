noperspective in vec3 dist;

uniform vec4 u_WireColor;
uniform vec4 u_FillColor;

out vec4 outColor;

void main(void) {
	float d = min(dist[0],min(dist[1],dist[2]));
	float I = exp2(-0.5*d*d);

	//if(I < 0.5f) discard;
	vec4 col = I*u_WireColor + (1.0 - I)*u_FillColor;
	outColor = vec4(col.xyz, u_FillColor.a);
}
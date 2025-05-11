#version 460 core

uniform vec2 Resolution;
uniform vec3 Orientation;
uniform vec3 Cam_pos;
out vec4 FragColor;
float box(vec3 sizes, vec3 pos, vec3 ray) {
    
    vec3 q = abs(pos - ray) - sizes;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    

}


vec3 repeat(vec3 p, vec3 c) {
    return mod(p + 0.5 * c, c) - 0.5 * c;
}

float sphere(float radius, vec3 pos, vec3 ray) {
    return length(pos-ray)-radius;
}
float testSDFComplitation(vec3 ray)
{

    return max(-sphere(150, vec3(0,0,200), ray), box(vec3(100,100,100), vec3(0,0,300),ray));
}
vec3 normal(vec3 point) {
    vec2 e = vec2(.01, 0); // x smol, y none
    float dist = testSDFComplitation(point);
    // Find normal as tangent of distance function
    return normalize(dist - vec3(
        testSDFComplitation(point - e.xyy),
        testSDFComplitation(point - e.yxy),
        testSDFComplitation(point - e.yyx)
    ));
}



void main() {
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, Orientation);
    vec3 localUp = cross(-right, Orientation);
    vec3 focalPoint = vec3(Resolution.x/2, Resolution.y/2, -400.0);
    vec3 ray = gl_FragCoord.xyz - focalPoint;
    
    vec3 raydir = normalize(ray);
    raydir = right * raydir.x + localUp * raydir.y + Orientation * raydir.z;
    ray = Cam_pos;
    float dist = 0.0;
    
    // while(dist >=0.000001) {
    //     dist = sphere(100.0f, vec3(400,400,1000), ray);
    //     ray += raydir * dist;
        
    //     if (dist >= 1000000) {
    //         break;
    //     }
    // }
    for (int i = 0; i< 5; ++i) {
        ray = repeat(ray, vec3(1000.0, 1000.0, 1000.0));
        dist = testSDFComplitation(ray);
        ray += raydir * dist;

        
    }

    //FragColor = vec4(gl_FragCoord.xy/Resolution.xy,1,1);
    FragColor = vec4(normal(ray) / 2.0f + 0.5f, 1.0);
    
}
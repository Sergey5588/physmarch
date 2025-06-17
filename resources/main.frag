#version 460 core
uniform vec2 Resolution;
uniform vec3 Orientation;
uniform vec3 Cam_pos;
uniform int Iterations = 100;
out vec4 FragColor;
float box(vec3 sizes, vec3 pos, vec3 ray) {
    
    vec3 q = abs(pos - ray) - sizes;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    

}

float bulb(vec3 p, float power) {
    //p = p/20;
    vec3 z = p;
    float dr = 1.0;
    float r = 0.0;
    
    for (int i = 0; i < 32; i++) { // Increased iterations
        r = length(z);
        
        if (r > 4.0) // Increased bailout radius
            break;
        
        // Convert to polar coordinates
        float theta = acos(z.z / r);
        float phi = atan(z.y, z.x); // Use 2-argument atan
        float zr = pow(r, power);
        
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        
        // Scale and rotate the point
        theta = theta * power;
        phi = phi * power;
        
        // Convert back to cartesian coordinates
        z = zr * vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        z += p;
    }
    
    return 0.5 * log(r) * r / dr;
}


vec3 repeat(vec3 p, vec3 c) {
    return mod(p + 0.5 * c, c) - 0.5 * c;
}



// correct way to repeat space every s units



float sphere(float radius, vec3 pos, vec3 ray) {
    return length(pos-ray)-radius;
}
float testSDFComplitation(vec3 ray)
{
    
    //return max(-sphere(150, vec3(0,0,200), ray), box(vec3(100,100,100), vec3(0,0,300),ray));
    //return min(sphere(150, vec3(0,200,200), ray), box(vec3(100,100,100), vec3(0,0,300),ray));
    return bulb(ray, 8.0);
    //return sphere(100, vec3(0,200,300), ray);
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
    vec3 right = normalize(cross(up, Orientation));
    vec3 localUp = normalize(cross(-right, Orientation));
    vec3 focalPoint = vec3(Resolution.x/2, Resolution.y/2, -400.0);
    vec3 ray = gl_FragCoord.xyz - focalPoint;
    
    vec3 raydir = normalize(ray);
    raydir = right * raydir.x + localUp * raydir.y + Orientation * raydir.z;
    ray = Cam_pos;
    float dist = 0.0;
    
    for (int i = 0; i< Iterations; ++i) {
        //ray = repeat(ray, vec3(1000.0, 1000.0, 1000.0));
        dist = testSDFComplitation(ray);
        if(dist < 0.001) break;
        ray += raydir * dist;

        
    }

    //FragColor = vec4(gl_FragCoord.xy/Resolution.xy,1,1);
    FragColor = vec4(normal(ray) / 2.0f + 0.5f, 1.0);
    
}
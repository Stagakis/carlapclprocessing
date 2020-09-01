#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float hole_radius;
uniform float hole_depth;
uniform vec3 hole_center;
uniform vec3 cameraPos;

void main()
{
    fragColor = vec3(1.0f);
    vec4 world_pos = model*vec4(aPos, 1.0f);


    if(distance(world_pos.xyz, hole_center) <= hole_radius){
        world_pos.y -= hole_depth - (distance(world_pos.xyz, hole_center)/hole_radius) * hole_depth;

        vec3 camera_to_point_ray = world_pos.xyz - cameraPos;
        float modifier = (- 2.4 - world_pos.y)/camera_to_point_ray.y;
        //world_pos.y + x*camera_to_point.y = -2.4 => x = (-2.4 -wolrd_pos.y)/camera_to_point.y;
        vec3 intersection_point = world_pos.xyz + camera_to_point_ray*modifier;
        if(distance(intersection_point, hole_center) > hole_radius) return;

        fragColor = vec3( distance(world_pos.xyz, hole_center) / (hole_radius), 0.1f,0.1f);
        gl_Position = projection * view * world_pos;

        gl_PointSize = max(5.0f, 5.0f -0.15f*length(gl_Position));
        return;
    }
    gl_Position = projection * view * world_pos;
    gl_PointSize = max(1.0f, 4.0f -0.1f*length(gl_Position));
    //gl_PointSize = 4.0f * min(1, (1/(length(gl_Position)*0.05)));

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}


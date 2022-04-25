
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Shader* shader, unsigned int amount);
    // update all particles
    void Update(float dt, glm::vec3& object, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    // render all particles
    void Draw(glm::mat4 pv, glm::mat4 m);
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader* shader;
    Texture texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, glm::vec3& object, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include <functional>
#include <thread>

#include "Camera.h"
#define LIFE 5.0f
// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity, Scale;
    glm::vec4 Color;
    int Age;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(LIFE), Scale(0.01f), Age(0) { }
    Particle(float life, float scale, int age) : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(life), Scale(scale), Age(age) { }
    
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Shader* shader, unsigned int amount, float life, float scale, int ages);
    ParticleGenerator(Shader* shader, string directory, string name, unsigned int amount, float life, float scale, int ages);
    ParticleGenerator(Shader* shader, string directory, string name, unsigned int amount, float life, float scale, int ages, glm::vec3 color);
    
    // update all particles
    void Update(float dt, glm::vec3 object, glm::vec3 velocity, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    void Update(float dt, glm::vec3& object, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    void Update(float dt, glm::vec3 object, unsigned int newParticles, std::function<glm::vec3()> vel_law, std::function<glm::vec3()> off_law);
    // render all particles
    void Update(float dt, glm::vec3 object, unsigned int newParticles, std::function<glm::vec3()> vel_law, std::function<glm::vec3()> ass_law, std::function<glm::vec3()> off_law);
    // render all particles
    void Draw(glm::mat4 pv);
    void Draw(glm::mat4 pv, Camera& camera);
private:
    // state
    std::vector<Particle> particles;
    std::vector<Texture> texture_stages;
    unsigned int amount;
    int ages;
    float life;
    float scale;
    glm::vec3 color;
    unsigned int lastUsedParticle = 0;
    // render state
    Shader* shader;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    void init(string directory, string name);
    void loadTextureStages(string path, string filename);
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, glm::vec3& object, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    void respawnParticle(Particle& particle, glm::vec3& object, glm::vec3& velocity, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};
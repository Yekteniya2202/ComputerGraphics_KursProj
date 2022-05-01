#include "ParticleSystem.h"
#include "Shader.h"
#include <thread>
#include "stb_image.h"
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

ParticleGenerator::ParticleGenerator(Shader* shader, string directory, string name, unsigned int amount, float life, float scale, int ages)
    : shader(shader), amount(amount), life(life), ages(ages)
{
    this->init(life, scale, directory, name);
}




void ParticleGenerator::Update(float dt, glm::vec3 object, glm::vec3 velocity, unsigned int newParticles, glm::vec3 offset)
{
    // add new particles 
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, velocity, offset);
    }
    // update all particles
    float lifeBorder = life / ages;
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle& p = this->particles[i];
        
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f)
        {	// particle is alive, thus update
            p.Position += p.Velocity * dt;
            p.Color.a = p.Life / life;
            if (p.Life > lifeBorder * p.Age) {
                //p.Age++;
            }
        }
    }
}

// render all particles
void ParticleGenerator::Draw(glm::mat4 pv)
{
    // use additive blending to give it a 'glow' effect
    
    this->shader->use();
    for (auto& particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            //cout << particle.Age << endl;
            glActiveTexture(GL_TEXTURE0 + particle.Age + 1);
            string number = to_string(particle.Age + 1);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, particle.Position);
            model = glm::scale(model, particle.Scale);
            this->shader->setMatrix4F("pv", pv);
            this->shader->setMatrix4F("model", model);
            this->shader->setVec4("color", particle.Color);
            this->shader->setInt("sprite1", particle.Age + 1);

            //cout << (texture_stages[particle.Age].type + number).c_str() << endl;
            glBindTexture(GL_TEXTURE_2D, texture_stages[particle.Age].id);
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

        }
    }
    //glDisable(GL_BLEND);
    // don't forget to reset to default blending mode
    
}

void ParticleGenerator::init(float life, float scale, string directory, string name)
{
    // set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = {
        //position			texture		
    -1.0f,-1.0f,-1.0f,	0.0f, 0.0f,
    -1.0f,-1.0f, 1.0f,	1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,	1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,	0.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,	1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,	0.0f, 1.0f,

    1.0f, 1.0f,-1.0f,	0.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,	1.0f, 0.0f,
    -1.0f, 1.0f,-1.0f,	1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,	0.0f, 1.0f,
    1.0f,-1.0f,-1.0f,	0.0f, 0.0f,
    -1.0f,-1.0f,-1.0f,	1.0f, 0.0f,

    1.0f,-1.0f, 1.0f,	0.0f, 0.0f,
    -1.0f,-1.0f,-1.0f,	1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,	0.0f, 1.0f,
    1.0f,-1.0f, 1.0f,	0.0f, 0.0f,
    -1.0f,-1.0f, 1.0f,	1.0f, 0.0f,
    -1.0f,-1.0f,-1.0f,	1.0f, 1.0f,

    -1.0f, 1.0f, 1.0f,	0.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,	0.0f, 0.0f,
    1.0f,-1.0f, 1.0f,	1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,	1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,	0.0f, 1.0f,
    1.0f,-1.0f, 1.0f,	1.0f, 0.0f,

    1.0f, 1.0f, 1.0f,	0.0f, 1.0f,
    1.0f,-1.0f,-1.0f,	1.0f, 0.0f,
    1.0f, 1.0f,-1.0f,	1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,	1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,	0.0f, 1.0f,
    1.0f,-1.0f, 1.0f,	0.0f, 0.0f,

    1.0f, 1.0f, 1.0f,	1.0f, 0.0f,
    1.0f, 1.0f,-1.0f,	1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,	0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,	1.0f, 0.0f,
    -1.0f, 1.0f,-1.0f,	0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,	0.0f, 0.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    loadTextureStages(directory, name);
    // create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle(life, scale, rand() % ages));
}

void ParticleGenerator::loadTextureStages(string directory, string name)
{
    
    
    int i = 0;
    while (true) {
        Texture texture;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        int width, height, nrComponents;
        string filename = directory + '/' + name + to_string(i) + ".png";
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        i++;
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            texture.id = textureID;
            texture.type = "sprite";
            texture.path = name.c_str();
            texture_stages.push_back(texture);
            stbi_image_free(data);
        }
        else
        {
            return;
        }
    }
}

// stores the index of the last particle used (for quick access to next dead particle)

unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, glm::vec3& object, glm::vec3& velocity, glm::vec3 offset)
{
    float randomX = ((rand() % 10) - 50) / 10.0f;
    float randomY = ((rand() % 10) - 50) / 10.0f;
    float randomZ = ((rand() % 10) - 50) / 10.0f;
    glm::vec3 random(randomX, randomY, randomZ);
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1);
    particle.Life = LIFE;
    particle.Age = 0;
    particle.Velocity = velocity;
}

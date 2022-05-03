#pragma once
#include <glad/glad.h>
class Picker
{
	float coords[2] = {
		0.0f, 0.0f
	};

	unsigned int VAO, VBO;

public:
	
	

	void draw() {
		glDrawArrays(GL_POINTS, 0, 1);
	}
	Picker() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);



		// vertex positions
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
};


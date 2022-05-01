#pragma once
#include <glad/glad.h>
class Picker
{
	float coords[8] = {
		
	};

	unsigned int indices[6] = {
		0, 1, 2,
		1, 2, 3
	};
	unsigned int VAO, VBO, EBO;

public:
	
	
	void setPoints(float x1, float x2, float y1, float y2) {
		coords[0] = x1; coords[1] = y1;
		coords[2] = x2; coords[3] = y1;
		coords[4] = x1; coords[5] = y2;
		coords[6] = x2; coords[7] = y2;
	}

	void draw() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coords), indices, GL_STREAM_DRAW);

		// vertex positions
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	Picker() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STREAM_DRAW);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STREAM_DRAW);

		// vertex positions
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
};


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL2/soil2.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Cylinder.h"
#include "Utils.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 8

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float stair1LocX, stair1LocY, stair1LocZ, stair2LocX, stair2LocY, stair2LocZ, stair3LocX, stair3LocY, stair3LocZ, stair4LocX, stair4LocY, stair4LocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

Cylinder myCylinder1(48, 12.0f);

int numCylinderVertices = myCylinder1.getNumVertices();
int numCylinderIndices = myCylinder1.getNumIndices();

glm::vec3 lightLoc = glm::vec3(50.0f, 20.0f, 20.0f);
float amt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::mat4 m1CylMat, m2CylMat, m3CylMat, m4CylMat, m5CylMat, m6CylMat;
glm::mat4 m1StepMat, m2StepMat, m3StepMat, m4StepMat;
glm::mat4 m1TopMat, m2TopMat, m3TopMat, m4TopMat, m5TopMat, m6TopMat;
glm::mat4 entablatureMat;
glm::mat4 mRoofMat;
glm::vec3 currentLightPos, transformed;
float lightPos[3];

// white light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// bronze material
float* matAmb = Utils::bronzeAmbient();
float* matDif = Utils::bronzeDiffuse();
float* matSpe = Utils::bronzeSpecular();
float matShi = Utils::bronzeShininess();

void installLights(glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}


void setupVertices(void) {

	std::vector<int> ind = myCylinder1.getIndices();
	std::vector<glm::vec3> vert = myCylinder1.getVertices();
	std::vector<glm::vec2> tex = myCylinder1.getTexCoords();
	std::vector<glm::vec3> norm = myCylinder1.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myCylinder1.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}

	float vertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,//5
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,//10
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	float normalPositions[108] =
	{
		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //5
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, //10
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	};
	
	float vertexPyramidPos[72]
	{
		-1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //Front
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, //Right side
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, -1.0f, //RS
		1.0f, 0.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, -1.0f, //Back
		-1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, //Bottom
		-1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, //Bottom
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, //Left Side
		0.0f, 1.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, //LS
	};

	float PyramidNorms[72]
	{
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

	//Cube VBOs
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertexPositions), &vertexPositions[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalPositions), &normalPositions[0], GL_STATIC_DRAW);

	//Prism VBOs

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPyramidPos), &vertexPyramidPos[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidNorms), &PyramidNorms[0], GL_STATIC_DRAW);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 0.0f; cameraY = 4.0f; cameraZ = 35.0f;
	//cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
}

void drawCylinder(glm::mat4 mMat) {
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glDrawElements(GL_TRIANGLES, myCylinder1.getIndices().size(), GL_UNSIGNED_INT, 0);
}

void drawRectPrism(glm::mat4 mMat) {
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawTriPrism(glm::mat4 mMat) {
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
	glDrawArrays(GL_TRIANGLES, 0, 24);
}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	//Collumns
	m1CylMat = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, -2.6f, -3.0f));
	m2CylMat = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, -2.6f, -3.0f));
	m3CylMat = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.6f, -3.0f));
	m4CylMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -2.6f, -3.0f));
	m5CylMat = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, -2.6f, -3.0f));
	m6CylMat = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -2.6f, -3.0f));
	
	//Steps
	m1StepMat = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(15.0f, 0.5f, 5.0f)), glm::vec3(0.0f, -8.0f, -2.0f));
	m2StepMat = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(15.0f, 0.5f, 4.0f)), glm::vec3(0.0f, -10.0f, -1.0f));
	m3StepMat = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(15.0f, 0.5f, 3.0f)), glm::vec3(0.0f, -12.0f, 0.0f));
	m4StepMat = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(15.0f, 0.5f, 3.0f)), glm::vec3(0.0f, -14.0f, 1.0f));

	//Collumn Rectangles
	m1TopMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 1.0f)), glm::vec3(-10.0f, 46.0f, -3.0f));
	m2TopMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 1.0f)), glm::vec3(-6.0f, 46.0f, -3.0f));
	m3TopMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 1.0f)), glm::vec3(-2.0f, 46.0f, -3.0f));
	m4TopMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 1.0f)), glm::vec3(2.0f, 46.0f, -3.0f));
	m5TopMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 1.0f)), glm::vec3(6.0f, 46.0f, -3.0f));
	m6TopMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.2f, 1.0f)), glm::vec3(10.0f, 46.0f, -3.0f));

	//Entablature
	entablatureMat = translate(glm::scale(glm::mat4(1.0f), glm::vec3(12.0f, 0.5f, 5.0f)), glm::vec3(0.0f, 20.0f, -1.0f));

	//Roof
	mRoofMat = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3 (13.0f, 5.0f, 5.0f)), glm::vec3(0.0f, 2.2f, -1.0f));

	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	amt += 0.5f;
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(amt), glm::vec3(0.0f, 0.0f, 1.0f));
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f));

	installLights(vMat);
	
	//Draw Collumns
	drawCylinder(m1CylMat);
	drawCylinder(m2CylMat);
	drawCylinder(m3CylMat);
	drawCylinder(m4CylMat);
	drawCylinder(m5CylMat);
	drawCylinder(m6CylMat);

	//Draw Steps
	drawRectPrism(m1StepMat);
	drawRectPrism(m2StepMat);
	drawRectPrism(m3StepMat);
	drawRectPrism(m4StepMat);

	//Draw collumn tops
	drawRectPrism(m1TopMat);
	drawRectPrism(m2TopMat);
	drawRectPrism(m3TopMat);
	drawRectPrism(m4TopMat);
	drawRectPrism(m5TopMat);
	drawRectPrism(m6TopMat);

	//Draw entablature
	drawRectPrism(entablatureMat);

	//Draw Roof
	drawTriPrism(mRoofMat);
}


void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter7 - program1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

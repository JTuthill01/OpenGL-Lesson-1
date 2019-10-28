#include <iostream>
#include <sstream>
#include "GL/glew.h"
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture2D.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"

const char* APP_TITLE = "Load OBJ files";

int WIDTH = 1024;
int HEIGHT = 800;

bool isCursorGrabbed = false;
bool isOpen = true;
int gIsFlashlightOn = true;

bool initGlew();

void processEvents(sf::Window& window);
void showFPS(sf::Window& window, sf::Clock& clock);
void updatePositions(sf::Window& window, float& deltaTime);
void update(sf::Window& window, float& deltaTime);

float gYaw = 0.F;
float gPitch = 0.F;
float gRadius = 10.F;
float mousePosX = 0.F;
float mousePosY = 0.F;
float deltaTime = 0.F;

FPSCamera fpsCamera(glm::vec3(0.F, 3.F, 15.F));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.F;
const float MOUSE_SENSITIVITY = 0.1F;

int main(int argc, char* argv[])
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;

	sf::Window window;

	window.create(sf::VideoMode(WIDTH, HEIGHT), APP_TITLE, sf::Style::Default, settings);

	window.setActive(true);

	if (!initGlew())
		throw std::runtime_error("Glew failed!!!");

	std::unique_ptr<Shader> lightShader;
	lightShader = std::make_unique<Shader>("Shaders/lighting_spot.vert",
		"Shaders/lighting_spot.frag");

	const int numModels = 7;

	Mesh lightMesh;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	lightMesh.loadOBJ("models/light.obj");

	mesh[0].loadOBJ("models/crate.obj");
	mesh[1].loadOBJ("models/woodcrate.obj");
	mesh[2].loadOBJ("models/robot.obj");
	mesh[3].loadOBJ("models/floor.obj");
	mesh[4].loadOBJ("models/bowling_pin.obj");
	mesh[5].loadOBJ("models/bunny.obj");
	mesh[6].loadOBJ("models/lampPost.obj");

	texture[0].loadTexture("Textures/crate.jpg", true);
	texture[1].loadTexture("Textures/woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("Textures/robot_diffuse.jpg", true);
	texture[3].loadTexture("Textures/tile_floor.jpg", true);
	texture[4].loadTexture("Textures/AMF.tga", true);
	texture[5].loadTexture("Textures/bunny_diffuse.jpg", true);
	texture[6].loadTexture("Textures/lamp_post_diffuse.png", true);

	//Model Positions
	glm::vec3 modelPosition[] =
	{
		glm::vec3(-2.5F, 1.F, 0.F),
		glm::vec3(2.5F, 1.F, 0.F),
		glm::vec3(0.F, 0.F, -2.F),
		glm::vec3(0.F, 0.F, 0.F),
		glm::vec3(0.F, 0.F, 2.F),
		glm::vec3(-9.F, 0.F, -9.F),
		glm::vec3(-5.F, 0.F, 0.F),
	};

	//Model Scale
	glm::vec3 modelScale[] =
	{
		glm::vec3(1.F, 1.F, 1.F),
		glm::vec3(1.F, 1.F, 1.F),
		glm::vec3(1.F, 1.F, 1.F),
		glm::vec3(10.F, 1.F, 10.F), //Floor
		glm::vec3(0.1F, 0.1F, 0.1F),
		glm::vec3(0.9F, 0.9F, 0.9F),
		glm::vec3(1.F, 1.F, 1.F),
	};

	sf::Clock clock;
	sf::Clock deltaTimeClock;

	float angle = 0.F;

	while (isOpen)
	{
		showFPS(window, clock);

		updatePositions(window, deltaTime);

		update(window, deltaTime);

		deltaTime = deltaTimeClock.restart().asSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.F);
		glm::mat4 view(1.F);
		glm::mat4 projection(1.F);

		view = fpsCamera.getViewMatrix();

		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)WIDTH / (float)HEIGHT,
			0.1F, 200.F);

		lightShader->use();

		glm::vec3 lightPos = fpsCamera.getPosition();

		lightPos.y -= 0.5F;

		lightShader->setUniform("view", view);
		lightShader->setUniform("projection", projection);
		lightShader->setUniform("viewPos", fpsCamera.getPosition());

		lightShader->setUniform("spotLight.ambient", glm::vec3(0.2F, 0.2F, 0.2F));
		lightShader->setUniform("spotLight.diffuse", glm::vec3(1.F, 1.F, 1.F));
		lightShader->setUniform("spotLight.specular", glm::vec3(1.F, 1.F, 1.F));
		lightShader->setUniform("spotLight.position", lightPos);
		lightShader->setUniform("spotLight.direction", fpsCamera.getLook());
		lightShader->setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(15.F)));
		lightShader->setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(20.F)));
		lightShader->setUniform("spotLight.constant", 1.F);
		lightShader->setUniform("spotLight.linear", 0.07F);
		lightShader->setUniform("spotLight.exponent", 0.017F);
		lightShader->setUniform("spotLight.on", gIsFlashlightOn);

		for (size_t i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(1.F), modelPosition[i]) *
				glm::scale(glm::mat4(1.F), modelScale[i]);

			lightShader->setUniform("model", model);

			lightShader->setUniform("material.ambient", glm::vec3(0.29F, 0.3F, 0.1F));
			lightShader->setUniformSampler("material.diffuseMap", 0);
			lightShader->setUniform("material.specular", glm::vec3(0.51F, 0.51F, 0.51F));
			lightShader->setFloat("material.shininess", 32.F);

			texture[i].bind(0);
			mesh[i].render();
			texture[i].unbind(0);
		}

		window.display();

		processEvents(window);
	}

	return 0;
}

bool initGlew()
{
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Glew Failed" << '\n';

		return false;
	}

	glClearColor(0.55F, 0.7F, 1.F, 1.F);

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);

	return true;
}

void processEvents(sf::Window& window)
{
	sf::Event e;

	while (window.pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
			isOpen = false;

		if (e.key.code == sf::Keyboard::Escape)
			isOpen = false;

		if (e.mouseButton.button == sf::Mouse::Left)
			isCursorGrabbed = true;

		if (e.mouseButton.button == sf::Mouse::Right)
			isCursorGrabbed = false;
	}
}

void showFPS(sf::Window& window, sf::Clock& clock)
{
	sf::Time time;

	time = clock.getElapsedTime();

	sf::Int32 currentSeconds = time.asMilliseconds();

	static double previousSeconds = 0.F;
	static int frameCount = 0;

	double elapsedSeconds;
	elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;

		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.F / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed << APP_TITLE << "   " << "FPS: " << fps <<
			"   " << "Frame Time: " << msPerFrame << " (ms)";

		window.setTitle(outs.str().c_str());

		frameCount = 0;

		clock.restart();
	}

	frameCount++;
}

void updatePositions(sf::Window& window, float& deltaTime)
{
	mousePosX = static_cast<float>(sf::Mouse::getPosition(window).x);

	mousePosY = static_cast<float>(sf::Mouse::getPosition(window).y);
}

void update(sf::Window& window, float& deltaTime)
{
	if (isCursorGrabbed == true)
	{
		fpsCamera.rotate((float)(WIDTH / 2.F - mousePosX) * MOUSE_SENSITIVITY,
			(float)(HEIGHT / 2.F - mousePosY) * MOUSE_SENSITIVITY);

		sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

		window.setMouseCursorVisible(false);

		//Forward and Backwards
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			fpsCamera.move(MOVE_SPEED * deltaTime * fpsCamera.getLook());

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			fpsCamera.move(MOVE_SPEED * deltaTime * -fpsCamera.getLook());

		//Left and Right
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			fpsCamera.move(MOVE_SPEED * deltaTime * fpsCamera.getRight());

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			fpsCamera.move(MOVE_SPEED * deltaTime * -fpsCamera.getRight());

		//Up and Down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			fpsCamera.move(MOVE_SPEED * deltaTime * fpsCamera.getUp());

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			fpsCamera.move(MOVE_SPEED * deltaTime * -fpsCamera.getUp());

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			gIsFlashlightOn = 1 - gIsFlashlightOn;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		gIsFlashlightOn = 1 - gIsFlashlightOn;

	if (isCursorGrabbed == false)
		window.setMouseCursorVisible(true);
}
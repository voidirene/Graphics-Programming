#include "Game.h"

//unsigned int indices[] = { 0, 1, 2 };

Game::Game() : cubemapTexture(0), skyboxVAO(0), skyboxVBO(0)
{
	gameState = GameState::PLAYING; //set the game state to PLAYING
	gameDisplay = new ScreenDisplay(); //create a new display

	counter = 0;
}

Game::~Game()
{

}

void Game::StartGame()
{
	InitializeSystems();
	GameLoop();
}

void Game::Exit(std::string text)
{
	gameDisplay->~ScreenDisplay();
	std::cout << text << '\n';
	SDL_Quit();
}

void Game::InitializeSystems()
{
	gameDisplay->InitializeDisplay(); //initializes the game display

	mesh1.LoadModel("..\\res\\monkey3.obj"); //loads a mesh from file
	mesh2.LoadModel("..\\res\\teapot.obj");
	mesh3.LoadModel("..\\res\\capsule.obj");

	shader.InitializeShader("..\\res\\shader"); //create a new shader
	fogshader.InitializeShader("..\\res\\FogShader");
	//fogshader.UseShader();
	//LinkFogShaderData(fogshader);
	toonshader.InitializeShader("..\\res\\ToonShader");
	//toonshader.UseShader();
	//LinkToonShaderData(toonshader);
	rimshader.InitializeShader("..\\res\\RimLightingShader");
	//rimshader.UseShader();
	//LinkRimLightingShaderData(rimshader);
	toonrimshader.InitializeShader("..\\res\\ToonRimShader");
	//toonrimshader.UseShader();
	//LinkToonRimShaderData(toonrimshader);
	geoshader.InitializeGeoShader("..\\res\\shaderGeoText");
	//geoshader.UseShader();
	//LinkGeoShader();
	reflectionshader.InitializeShader("..\\res\\shaderReflection");
	reflectionshader.UseShader();
	LinkReflectionShader();

	texture.InitializeTexture("..\\res\\bricks.jpg"); //load a texture
	texture.InitializeTexture("..\\res\\water.jpg");
	texture.InitializeTexture("..\\res\\grass.jpg");

	camera.InitializeCamera(glm::vec3(0, 0, -5), 70.0f, (float) gameDisplay->GetWidth() / gameDisplay->GetHeight(), 0.01f, 1000.0f); //initializes the camera

	audio.AddNewSound("..\\res\\bang.wav");
	audio.AddNewBackgroundMusic("..\\res\\background.wav");

	shaderSkybox.InitializeShader("..\\res\\SkyboxShader");
	InitializeSkybox();
}

void Game::InitializeSkybox()
{
	std::vector<std::string> faces
	{
		"..\\res\\skybox\\right.jpg",
		"..\\res\\skybox\\left.jpg",
		"..\\res\\skybox\\top.jpg",
		"..\\res\\skybox\\bottom.jpg",
		"..\\res\\skybox\\front.jpg",
		"..\\res\\skybox\\back.jpg"
	};

	cubemapTexture = skybox.LoadCubemap(faces); //Load the cubemap using "faces" into cubemapTextures

	float skyboxVertices[] = {
		// positions          
		-6.0f,  6.0f, -6.0f,
		-6.0f, -6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,
		6.0f,  6.0f, -6.0f,
		-6.0f,  6.0f, -6.0f,

		-6.0f, -6.0f,  6.0f,
		-6.0f, -6.0f, -6.0f,
		-6.0f,  6.0f, -6.0f,
		-6.0f,  6.0f, -6.0f,
		-6.0f,  6.0f,  6.0f,
		-6.0f, -6.0f,  6.0f,

		6.0f, -6.0f, -6.0f,
		6.0f, -6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,

		-6.0f, -6.0f,  6.0f,
		-6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f, -6.0f,  6.0f,
		-6.0f, -6.0f,  6.0f,

		-6.0f,  6.0f, -6.0f,
		6.0f,  6.0f, -6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		-6.0f,  6.0f,  6.0f,
		-6.0f,  6.0f, -6.0f,

		-6.0f, -6.0f, -6.0f,
		-6.0f, -6.0f,  6.0f,
		6.0f, -6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,
		-6.0f, -6.0f,  6.0f,
		6.0f, -6.0f,  6.0f
	};

	//use openGL functionality to generate & bind data into buffers
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Game::DisplaySkybox()
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shaderSkybox.UseShader();
	shaderSkybox.setInt("skybox", 0);
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetView())); // remove translation from the view matrix
	shaderSkybox.setMat4("view", view);
	shaderSkybox.setMat4("projection", camera.GetProjection());
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}


void Game::LinkFogShaderData() //TODO: combine these methods?
{
	fogshader.setVec3("fogColor", glm::vec3(0.2, 0.2, 0.2));
	fogshader.setFloat("minDist", -5.0f);
	fogshader.setFloat("maxDist", 5.0f);
}

void Game::LinkToonShaderData()
{
	toonshader.setVec3("lightDir", glm::vec3(1, 5, 1));
}

void Game::LinkRimLightingShaderData()
{
	rimshader.setVec3("lightDir", glm::vec3(0, 0, 3));
	rimshader.setMat4("m", mesh1.getMM());
}

void Game::LinkToonRimShaderData()
{
	toonrimshader.setVec3("lightDir", glm::vec3(0, 0, 3));
	toonrimshader.setMat4("m", mesh1.getMM());
}

void Game::LinkGeoShader()
{
	float randomX = ((float)rand() / (RAND_MAX));
	float randomY = ((float)rand() / (RAND_MAX));
	float randomZ = ((float)rand() / (RAND_MAX));
	geoshader.setFloat("randColourX", randomX);
	geoshader.setFloat("randColourY", randomY);
	geoshader.setFloat("randColourZ", randomZ);
	geoshader.setFloat("time", counter);
}

void Game::LinkReflectionShader()
{
	reflectionshader.setMat4("view", camera.GetView());
	reflectionshader.setMat4("projection", camera.GetProjection());
	reflectionshader.setVec3("cameraPos", camera.GetPosition());
	reflectionshader.setMat4("model", mesh1.getMM()); //TODO: fix & remove some of these
	//reflectionshader.setMat4("model", mesh2.getMM());
	//reflectionshader.setMat4("model", mesh3.getMM());
}

void Game::GameLoop()
{
	while (gameState == GameState::PLAYING)
	{
		audio.PlayBackgroundMusic();
		ProcessUserInputs();
		UpdateDisplay();

		//DetectCollision(mesh1.boundingSphere.GetPosition(), mesh1.boundingSphere.GetRadius(), mesh2.boundingSphere.GetPosition(), mesh2.boundingSphere.GetRadius());
	}

	Exit("Escape key pressed, closing program...");
}

void Game::ProcessUserInputs()
{
	glfwPollEvents(); //poll events

	//for quitting the game
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		gameState = GameState::QUITTING;
		return;
	}

	//for keyboard camera movement
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.MoveCameraVertically(1);
	}
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.MoveCameraVertically(-1);
	}
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.MoveCameraHorizontally(1);
	}
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.MoveCameraHorizontally(-1);
	}
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		camera.ZoomCamera(1);
	}
	if (glfwGetKey(gameDisplay->window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		camera.ZoomCamera(-1);
	}

	//camera mouse input
	camera.MouseControls(gameDisplay);
}

void Game::UpdateDisplay()
{
	gameDisplay->ClearDisplay(0.0f, 0.0f, 0.0f, 1.0f); //clear the display

	//shader.UseShader();
	//fogshader.UseShader();
	//toonshader.UseShader();
	//LinkRimLightingShaderData(rimshader);
	//rimshader.UseShader();
	//LinkToonRimShaderData(toonrimshader);
	//toonrimshader.UseShader();
	//geoshader.UseShader();	
	//LinkGeoShader();
	reflectionshader.UseShader();
	LinkReflectionShader();

	//MESH1
	//shader.UpdateTransform(mesh1.transform, camera);
	//fogshader.UpdateTransform(mesh1.transform, camera);
	//fogshader.setMat4("ModelMatrix", mesh1.transform.GetModel());
	//toonshader.UpdateTransform(mesh1.transform, camera);
	//rimshader.UpdateTransform(mesh1.transform, camera);
	//toonrimshader.UpdateTransform(mesh1.transform, camera);
	//geoshader.UpdateTransform(mesh1.transform, camera);
	reflectionshader.UpdateTransform(mesh1.transform, camera);
	texture.UseTexture(0);
	mesh1.Display(-1.0, 0.0, 0.0, counter, 0.0, 0.0, 1.0, camera);

	//MESH2
	//shader.UpdateTransform(mesh2.transform, camera);
	//fogshader.UpdateTransform(mesh2.transform, camera);
	//fogshader.setMat4("ModelMatrix", mesh2.transform.GetModel());
	//toonshader.UpdateTransform(mesh2.transform, camera);
	//rimshader.UpdateTransform(mesh2.transform, camera);
	//toonrimshader.UpdateTransform(mesh2.transform, camera);
	//geoshader.UpdateTransform(mesh2.transform, camera);
	//reflectionshader.UpdateTransform(mesh2.transform, camera);
	//texture.UseTexture(1);
	//mesh2.Display(0.0, sinf(counter) * 5, 0.0, 0.0, 0.0, 0.0, 0.1, camera);

	//MESH3
	//shader.UpdateTransform(mesh3.transform, camera);
	//fogshader.UpdateTransform(mesh3.transform, camera);
	//fogshader.setMat4("ModelMatrix", mesh3.transform.GetModel());
	//toonshader.UpdateTransform(mesh3.transform, camera);
	//rimshader.UpdateTransform(mesh3.transform, camera);
	//toonrimshader.UpdateTransform(mesh3.transform, camera);
	//geoshader.UpdateTransform(mesh3.transform, camera);
	//reflectionshader.UpdateTransform(mesh3.transform, camera);
	//texture.UseTexture(2);
	//mesh3.Display(3.0, 0.0, sinf(counter) * 15, 0.0, counter, 0.0, 1.0, camera);

	counter += 0.01f;

	DisplaySkybox();

	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();

	gameDisplay->ChangeBuffer(); //swap the buffers
}

bool Game::DetectCollision(glm::vec3 position1, float radius1, glm::vec3 position2, float radius2)
{
	float distance = glm::sqrt((position2.x - position1.x) * (position2.x - position1.x) + (position2.y - position1.y) * (position2.y - position1.y) + (position2.z - position1.z) * (position2.z - position1.z)); //pythagoras

	if (distance < (radius1 + radius2))
	{
		cout << "collision! : " << distance << '\n';
		audio.PlaySound(0); //plays a sound if sound isn't already playing
		return true;
	}
	else
	{
		cout << "NO collision! : " << distance << '\n';
		return false;
	}
}
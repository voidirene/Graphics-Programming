#include "Game.h"

//unsigned int indices[] = { 0, 1, 2 };

Game::Game()
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
	rimshader.UseShader();
	LinkRimLightingShaderData(rimshader);

	texture.InitializeTexture("..\\res\\bricks.jpg"); //load a texture
	texture.InitializeTexture("..\\res\\water.jpg");
	texture.InitializeTexture("..\\res\\grass.jpg");

	camera.InitializeCamera(glm::vec3(0, 0, -5), 70.0f, (float) gameDisplay->GetWidth() / gameDisplay->GetHeight(), 0.01f, 1000.0f); //initializes the camera

	audio.AddNewSound("..\\res\\bang.wav");
	audio.AddNewBackgroundMusic("..\\res\\background.wav");
}

void Game::LinkFogShaderData(Shading fogshader) //TODO: combine these methods?
{
	fogshader.setVec3("fogColor", glm::vec3(0.2, 0.2, 0.2));
	fogshader.setFloat("minDist", -5.0f);
	fogshader.setFloat("maxDist", 5.0f);
}

void Game::LinkToonShaderData(Shading toonshader)
{
	toonshader.setVec3("lightDir", glm::vec3(1, 5, 1));
}

void Game::LinkRimLightingShaderData(Shading rimshader)
{
	rimshader.setVec3("lightDir", glm::vec3(0, 0, 3));
	rimshader.setMat4("m", mesh1.getMM());
}

void Game::GameLoop()
{
	while (gameState == GameState::PLAYING)
	{
		audio.PlayBackgroundMusic();
		ProcessUserInputs();
		UpdateDisplay();

		DetectCollision(mesh1.boundingSphere.GetPosition(), mesh1.boundingSphere.GetRadius(), mesh2.boundingSphere.GetPosition(), mesh2.boundingSphere.GetRadius());
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
	LinkRimLightingShaderData(rimshader);
	rimshader.UseShader();

	//MESH1
	//shader.UpdateTransform(mesh1.transform, camera);
	//fogshader.UpdateTransform(mesh1.transform, camera);
	//fogshader.setMat4("ModelMatrix", mesh1.transform.GetModel());
	//toonshader.UpdateTransform(mesh1.transform, camera);
	rimshader.UpdateTransform(mesh1.transform, camera);
	texture.UseTexture(0);
	mesh1.Display(-1.0, 0.0, 0.0, counter, 0.0, 0.0, 1.0, camera);

	//MESH2
	//shader.UpdateTransform(mesh2.transform, camera);
	//fogshader.UpdateTransform(mesh2.transform, camera);
	//fogshader.setMat4("ModelMatrix", mesh2.transform.GetModel());
	//toonshader.UpdateTransform(mesh2.transform, camera);
	rimshader.UpdateTransform(mesh2.transform, camera);
	texture.UseTexture(1);
	mesh2.Display(0.0, sinf(counter) * 5, 0.0, 0.0, 0.0, 0.0, 0.1, camera);

	//MESH3
	//shader.UpdateTransform(mesh3.transform, camera);
	//fogshader.UpdateTransform(mesh3.transform, camera);
	//fogshader.setMat4("ModelMatrix", mesh3.transform.GetModel());
	//toonshader.UpdateTransform(mesh3.transform, camera);
	rimshader.UpdateTransform(mesh3.transform, camera);
	texture.UseTexture(2);
	mesh3.Display(3.0, 0.0, sinf(counter) * 15, 0.0, counter, 0.0, 1.0, camera);

	counter += 0.01f;

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
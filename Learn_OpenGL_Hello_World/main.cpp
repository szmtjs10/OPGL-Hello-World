#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <camera.h>
#include <Model.h>
#include <Mesh.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void setupMovingCubes(glm::vec3(&cubePositions)[12], Shader lightingShader);
void setupLampObject(Shader lampShader, glm::vec3 lightColor);
void setupCubeObjects(Shader lightingShader, glm::vec3 lightColor, glm::vec3 pl_ambientIntensity, glm::vec3 pl_diffuseIntensity, glm::vec3 pl_specularIntensity, glm::vec3 fl_ambientIntensity, glm::vec3 fl_diffuseIntensity, glm::vec3 fl_specularIntensity, glm::vec3 dl_ambientIntensity, glm::vec3 dl_diffuseIntensity, glm::vec3 dl_specularIntensity);
void setupModelObject(Shader modelShader, glm::vec3 lightColor, glm::vec3 pl_ambientIntensity, glm::vec3 pl_diffuseIntensity, glm::vec3 pl_specularIntensity, glm::vec3 fl_ambientIntensity, glm::vec3 fl_diffuseIntensity, glm::vec3 fl_specularIntensity, glm::vec3 dl_ambientIntensity, glm::vec3 dl_diffuseIntensity, glm::vec3 dl_specularIntensity);

// Global variables
const unsigned int SCREEN_WIDTH = 800 * 1.4;
const unsigned int SCREEN_HEIGHT = 600 * 1.4;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// Create camera object with starting position
Camera camera(cameraPos);
float lastCursorX = SCREEN_WIDTH/2.0f;
float lastCursorY = SCREEN_HEIGHT/2.0f;
float yaw   = 0.0f;
float pitch = 0.0f;
bool firstMouseCapture = true;
bool isMovingLight = false;

// Positions
glm::vec3 pointLightPos(1.2f, 1.0f, 2.0f);
glm::vec3 movingLightPos = pointLightPos;
glm::vec3 backpackPos = glm::vec3(-1.8f, 0.0f, 2.0f);

bool isFlashlightOn = false;
bool isOutlineOn = false;

float deltaTime = 0.0f; // Time to render last frame
float lastFrame = 0.0f; // Time of last frame
float startTime = glfwGetTime();
float timeSinceLastPrintf = 0.0f;

int main() {
	// Setup version (using OpenGL v3.3 in core-profile mode)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create an OpenGL window using GLFW library implementation
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Init GLAD (manages OpenGL function pointers)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Give OpenGL dimensions of window
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// Hide and capture mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Callback function for window being resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Callback function for mouse cursor movement
	glfwSetCursorPosCallback(window, mouse_callback);

	// Callback function for scrolling zoom
	glfwSetScrollCallback(window, scroll_callback);

	// Callback function for mouse buttons
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// vertices of triangles in object space
	// Ensure all triangles are counter-clockwise winding order
	float vertices_old[] = {
		// first triangle
		0.5f, 0.5f, 0.0f, // top right
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, 0.5f, 0.0f, // top left
		// second triangle
		0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f // top left
	};

	float vertices[] = {
		0.5f, 0.5f, 0.0f, // Cube back right
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,  
		-0.5f, 0.5f, 0.0f, // Cube back left
		0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,

		0.5f, 0.5f, 1.0f, // Cube front right
		-0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 1.0f, // Cube front left
		-0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 1.0f,

		0.5f, 0.5f, 0.0f, // Cube right right
		0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f, // Cube right left
		0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, 1.0f,

		-0.5f, 0.5f, 0.0f, // Cube left right
		-0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 1.0f,
		-0.5f, 0.5f, 0.0f, // Cube left left
		-0.5f, -0.5f, 1.0f,
		-0.5f, 0.5f, 1.0f,

		0.5f, 0.5f, 0.0f, // Cube top right
		-0.5f, 0.5f, 1.0f,
		0.5f, 0.5f, 1.0f,
		0.5f, 0.5f, 0.0f, // Cube top left
		-0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 1.0f,

		0.5f, -0.5f, 0.0f, // Cube bottom right
		0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 0.0f, // Cube bottom left
		-0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, 0.0f,
	};

	float surfaceNormals[] = {
		0.0f, 0.0f, -1.0f, // Cube back face
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, 1.0f, // Cube front face
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Cube right face
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f, // Cube left face
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, // Cube top face
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f, // Cube top face
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};

	float colours[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, // Other cubes faces
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,


	};

	float textureCoordsContainer[] = {
		1.0f, 1.0f, // back
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f, // front
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f, // right
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f, // left
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f, // top
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f, // bottom
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};

	float textureCoordsFace[] = {
		
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	unsigned int indices[] = {
		0, 1, 2, // Cube back right
		3, 4, 5, // Cube back left
		0, 2, 3, 
		5, 2, 3,  

		6, 7, 8, // Cube front
		9, 10, 11,

		12, 13, 14, // Cube right
		15, 16, 17,

		18, 19, 20, // Cube left
		21, 22, 23,

		24, 25, 26, // Cube top
		27, 28, 29,

		30, 31, 32, // Cube bottom
		33, 34, 35,
	};

	Shader shaderProgram_original = Shader("vertex_shader_src.glsl", "fragment_shader_src.glsl");
	Shader lightingShader = Shader("vertex_shader_lighting_src.glsl", "fragment_shader_lighting_src.glsl");
	Shader lampShader = Shader("vertex_shader_light_src.glsl", "fragment_shader_light_src.glsl");
	Shader modelShader = Shader("vertex_shader_model_src.glsl", "fragment_shader_model_src.glsl");
	Shader outlineShader = Shader("vertex_shader_model_src.glsl", "fragment_shader_object_outline_src.glsl");

	// -------------------------------------------------------------------------------------------------------------------------
	// Generate, bind, and fill main Vertex Array Object (VAO) and Vertex Buffer Objects (VBOs)
	unsigned int VAO_cube, VBO_vertices, VBO_normals, VBO_colours, 
		VBO_containerTexCoords, VBO_faceTexCoords, VBO_metalBorderTexCoords;
	glGenVertexArrays(1, &VAO_cube);
	glGenBuffers(1, &VBO_vertices);
	glGenBuffers(1, &VBO_normals);
	glGenBuffers(1, &VBO_colours);
	glGenBuffers(1, &VBO_containerTexCoords);
	glGenBuffers(1, &VBO_faceTexCoords);
	glGenBuffers(1, &VBO_metalBorderTexCoords);
	// Order: Bind the VAO first, then bind and set vertex buffers, and then configure vertex attributes
	glBindVertexArray(VAO_cube);
	// Vertices VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Colours VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_colours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Texture VBOs
	// Container texture coords
	glBindBuffer(GL_ARRAY_BUFFER, VBO_containerTexCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordsContainer), textureCoordsContainer, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Smiling face texture coords
	glBindBuffer(GL_ARRAY_BUFFER, VBO_faceTexCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordsFace), textureCoordsFace, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Metal border container texture coords
	glBindBuffer(GL_ARRAY_BUFFER, VBO_metalBorderTexCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordsContainer), textureCoordsContainer, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Surface normals VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(surfaceNormals), surfaceNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable attributes for VAO
	glEnableVertexAttribArray(0); // Vertices
	glEnableVertexAttribArray(1); // Colours
	glEnableVertexAttribArray(2); // Container texture coords
	glEnableVertexAttribArray(3); // Face texture coords
	glEnableVertexAttribArray(4); // Metal border container tex coords
	glEnableVertexAttribArray(5); // Normals
	// Create Element Buffer Object
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), indices, GL_STATIC_DRAW);
	// Bind EBO to VAO as well
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Unbinding VAO and buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Second VAO for lighting cube
	unsigned int VAO_light;
	glGenVertexArrays(1, &VAO_light);
	glBindVertexArray(VAO_light);
	// Bind to VBO containing vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	// Set and enable the vertex attributes pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Unbinding VAO and buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// -------------------------------------------------------------------------------------------------------------------------

	// Texture loading -- metal border container
	// Generate an OpenGL texture and add data from the loaded .jpg image
	unsigned int metalBorderTexture = loadTexture("metal_border_container_texture.png");

	// Set textures in shader
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 7); // set metalBorderTexture

	// Create copies of the cube at different x,y,z locations
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(-4.8f, -0.1f, -6.0f),
		glm::vec3(-4.4f, 3.0f, -6.0f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	// Flip texture along y axis before loading
	stbi_set_flip_vertically_on_load(true);

	// Load models
	modelShader.use();
	Model backpackModel = Model((char*)"models/backpack/backpack.obj");

	// Flashlight properties
	glm::vec3 flashlightColour = glm::vec3(0.7f);
	glm::vec3 fl_diffuseIntensity = glm::vec3(1.0f);
	glm::vec3 fl_ambientIntensity = glm::vec3(0.2f);
	glm::vec3 fl_specularIntensity = glm::vec3(0.4f);
	glm::vec3 fl_diffuseColor = flashlightColour * fl_diffuseIntensity;
	glm::vec3 fl_ambientColor = fl_diffuseColor * fl_ambientIntensity;
	// Directional light properties
	glm::vec3 dl_lightColour = glm::vec3(1.0f);
	glm::vec3 dl_diffuseIntensity = glm::vec3(0.5f);
	glm::vec3 dl_ambientIntensity = glm::vec3(0.2f);
	glm::vec3 dl_specularIntensity = glm::vec3(0.2f);
	glm::vec3 dl_diffuseColor = dl_lightColour * dl_diffuseIntensity;
	glm::vec3 dl_ambientColor = dl_diffuseColor * dl_ambientIntensity;
	
	// Enable face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT); // cull front faces
	glCullFace(GL_BACK); // cull back faces
	glFrontFace(GL_CCW); // tell OpenGL that front faces have CCW winding order

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Display graphics loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		timeSinceLastPrintf += deltaTime;

		// user key input processing
		processInput(window);

		// Enable OpenGL z-buffer depth comparisons
		glEnable(GL_DEPTH_TEST);
		// Render only those fragments with lower depth values
		glDepthFunc(GL_LESS);

		// Enable OpenGL stencil buffer
		glEnable(GL_STENCIL_TEST);
		// Tell OpenGL that whenever the stencil value of a fragment is not equal to 1, it should be discarded
		// glStencilFunc(GL_EQUAL, 1, 0xFF);
		// glStencilOp parameters:
		// - sfail: action to take if the stencil test fails
		// - dpfail : action to take if the stencil test passes, but the depth test fails
		// - dppass : action to take if both the stenciland the depth test pass
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		// set all fragments to update the stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// bitwise AND operator with each stencil buffer write
		glStencilMask(0xFF);  // enable stencil buffer writing
		// Next draw the objects normally before switching stencil buffer properties and shader

		// Lamp point light colour
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 1.0f) / 2.0f + 0.7f;
		lightColor.y = sin(glfwGetTime() * 0.5f) / 2.0f + 0.7f;
		lightColor.z = sin(glfwGetTime() * 0.4f) / 2.0f + 0.7f;

		// Set clear colour
		glClearColor(lightColor.x / 10.0f, lightColor.y / 10.0f, lightColor.z / 10.0f, 1.0f);
		// Clear colour and z-buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Point light properties
		glm::vec3 pl_diffuseIntensity = glm::vec3(0.9f);
		glm::vec3 pl_ambientIntensity = glm::vec3(0.4f);
		glm::vec3 pl_specularIntensity = glm::vec3(0.8f);
		glm::vec3 pl_diffuseColor = lightColor * pl_diffuseIntensity;
		glm::vec3 pl_ambientColor = pl_diffuseColor * pl_ambientIntensity;

		// Lamp object rendering
		setupLampObject(lampShader, lightColor);
		glBindVertexArray(VAO_light);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Cube rendering
		setupCubeObjects(lightingShader, lightColor, pl_ambientIntensity, pl_diffuseIntensity, pl_specularIntensity,
			fl_ambientColor, fl_diffuseColor, fl_specularIntensity, dl_ambientColor, dl_diffuseColor, dl_specularIntensity);
		// Bind metal border texture diffuse map 
		// TODO figure out why the cube is getting wrong texture
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, metalBorderTexture);
		glBindVertexArray(VAO_cube);
		//glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, 0);

		// set all fragments to NOT update the stencil buffer
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		// Setup and render moving cube objects
		setupMovingCubes(cubePositions, lightingShader);

		// set all fragments to update the stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// Setup and render the loaded backpack model
		setupModelObject(modelShader, lightColor, pl_ambientIntensity, pl_diffuseIntensity, pl_specularIntensity,
			fl_ambientColor, fl_diffuseColor, fl_specularIntensity, dl_ambientColor, dl_diffuseColor, dl_specularIntensity);
		backpackModel.Draw(modelShader);

		if (isOutlineOn) {
			// TODO figure out why cubes are rendered over outline 
			// (likely need to turn off writing to stencil buffer before rendering the cubes)
			outlineShader.use();
			// Draw outline around objects
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00); // disable writing to the stencil buffer
			glDisable(GL_DEPTH_TEST);
			// 
			// View/Projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			outlineShader.setMatrix4("proj", projection);
			outlineShader.setMatrix4("view", view);
			//
			// Render the outline (scaled model)
			glm::mat4 model_outline_matrix = glm::mat4(1.0f);
			model_outline_matrix = glm::translate(model_outline_matrix, glm::vec3(backpackPos));
			// Scale by factor larger than before
			model_outline_matrix = glm::scale(model_outline_matrix, glm::vec3(0.51f));
			outlineShader.setMatrix4("model", model_outline_matrix);
			backpackModel.Draw(outlineShader);
		}

		// Print FPS
		float fps = 1.0f / deltaTime;
		if (timeSinceLastPrintf > 1.0) {
			printf("%f seconds per frame\n", deltaTime);
			printf("%f fps =  1 / secs per frame \n\n", fps);
			timeSinceLastPrintf = 0.0f;
		}

		// Check events and swap frame buffers (avoids flickering)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Print max number of attribute pointers supported on system
	/*int numAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
	std::cout << "Maximum number of vertex attributes supported: " << numAttributes << std::endl;*/

	// Release GLFW resources before exiting
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteVertexArrays(1, &VAO_light);
	glDeleteBuffers(1, &VBO_vertices);
	glDeleteBuffers(1, &VBO_colours);
	glDeleteBuffers(1, &VBO_containerTexCoords);
	glDeleteBuffers(1, &VBO_faceTexCoords);
	glDeleteBuffers(1, &VBO_metalBorderTexCoords);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void setupMovingCubes(glm::vec3 (&cubePositions)[12], Shader lightingShader)
{
	for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
	{
		// Model: Render copies of cube with differing model matrices
		glm::mat4 model_matrix(1.0f);
		model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
		glm::vec3 movingCubePos = (glm::vec3)cubePositions[i] * (float)(sin(glfwGetTime()) / 2.0f + 0.5f);
		model_matrix = glm::translate(model_matrix, movingCubePos);
		float twistSpeed = i / 2.0f + 7.0f;
		model_matrix = glm::rotate(model_matrix, twistSpeed * (float)(sin(glfwGetTime()) / 2.0f + 0.5f), glm::vec3(0.1f, 0.1f, 0.15f));
		// View: Translate scene in reverse direction from camera
		glm::mat4 view_matrix = camera.GetViewMatrix();
		// Proj: Zoom/Field of View (FOV), set aspect ratio, front and back clipping of view frustum 
		glm::mat4 projection_matrix(1.0f);
		projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		// Set uniforms in shader program
		lightingShader.setMatrix4("model", model_matrix);
		lightingShader.setMatrix4("view", view_matrix);
		lightingShader.setMatrix4("proj", projection_matrix);
		// Draw each cube
		glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, 0);
	}
}

void setupCubeObjects(Shader lightingShader, glm::vec3 lightColor, glm::vec3 pl_ambientColor, glm::vec3 pl_diffuseColor, glm::vec3 pl_specularIntensity, glm::vec3 fl_ambientColor, glm::vec3 fl_diffuseColor, glm::vec3 fl_specularIntensity, glm::vec3 dl_ambientColor, glm::vec3 dl_diffuseColor, glm::vec3 dl_specularIntensity)
{
	lightingShader.use();
	// Set uniforms in shader program
	// Model matrix for world-centered cube
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::mat4(1.0f);
	lightingShader.setMatrix4("model", model_matrix);
	// Use same view and proj matrices as for lamp in setupLampObject()
	lightingShader.setMatrix4("view", camera.GetViewMatrix());
	glm::mat4 projection_matrix(1.0f);
	projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	lightingShader.setMatrix4("proj", projection_matrix);
	lightingShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	// Set material struct properties
	lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	lightingShader.setFloat("material.shininess", 16.0f);
	//
	// Point light properties
	lightingShader.setVec3("pointLights[0].ambient", pl_ambientColor);
	lightingShader.setVec3("pointLights[0].diffuse", pl_diffuseColor);
	lightingShader.setVec3("pointLights[0].specular", pl_specularIntensity);
	// Point light attenuation properties
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.09f);
	lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
	// Point light position
	lightingShader.setVec3("pointLights[0].position", movingLightPos);
	//
	// Flashlight properties
	lightingShader.setBool("flashlight.on", isFlashlightOn);
	lightingShader.setVec3("flashlight.ambient", fl_ambientColor);
	lightingShader.setVec3("flashlight.diffuse", fl_diffuseColor);
	lightingShader.setVec3("flashlight.specular", fl_specularIntensity);
	// Flashlight attenuation properties
	lightingShader.setFloat("flashlight.constant", 1.0f);
	lightingShader.setFloat("flashlight.linear", 0.09f);
	lightingShader.setFloat("flashlight.quadratic", 0.032f);
	// Flashlight position and direction
	lightingShader.setVec3("flashlight.position", camera.Position);
	lightingShader.setVec3("flashlight.direction", camera.Front);
	// Flashlight cutOff angle
	lightingShader.setFloat("flashlight.cutOff", glm::cos(glm::radians(5.0f)));
	lightingShader.setFloat("flashlight.outerCutOff", glm::cos(glm::radians(20.0f)));
	//
	// Directional light properties
	lightingShader.setVec3("dirLights[0].ambient", dl_ambientColor);
	lightingShader.setVec3("dirLights[0].diffuse", dl_diffuseColor);
	lightingShader.setVec3("dirLights[0].specular", dl_specularIntensity);
	// Directional light direction
	lightingShader.setVec3("dirLights[0].direction", glm::vec3(-1.0f, -1.0f, 0.0f));
}

void setupLampObject(Shader lampShader, glm::vec3 lightColor)
{
	lampShader.use();
	// Model matrix: Translate and scale the light object
	glm::mat4 model_matrix = glm::mat4(1.0f);
	movingLightPos = pointLightPos;
	if (isMovingLight) {
		movingLightPos.x *= (float)(sin(glfwGetTime()) * 3.0f);
		movingLightPos.y *= (float)(cos(glfwGetTime()) * 3.0f);
	}
	model_matrix = glm::translate(model_matrix, movingLightPos);
	model_matrix = glm::scale(model_matrix, glm::vec3(0.2f));
	// View matrix: camera
	glm::mat4 view_matrix = camera.GetViewMatrix();
	// Proj matrix: Zoom/Field of View (FOV), set aspect ratio, front and back clipping of view frustum 
	glm::mat4 projection_matrix(1.0f);
	projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	// Set uniforms in shader program
	// Model, view, projection matrices
	lampShader.setMatrix4("model", model_matrix);
	lampShader.setMatrix4("view", view_matrix);
	lampShader.setMatrix4("proj", projection_matrix);
	// Light colour uniform
	lampShader.setVec3("lampColor", lightColor * 0.8f);
}

void setupModelObject(Shader modelShader, glm::vec3 lightColor, glm::vec3 pl_ambientColor, glm::vec3 pl_diffuseColor, glm::vec3 pl_specularIntensity, glm::vec3 fl_ambientColor, glm::vec3 fl_diffuseColor, glm::vec3 fl_specularIntensity, glm::vec3 dl_ambientColor, glm::vec3 dl_diffuseColor, glm::vec3 dl_specularIntensity)
{
	// Use the model shader
	modelShader.use();

	// View/Projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	modelShader.setMatrix4("proj", projection);
	modelShader.setMatrix4("view", view);

	// Render the loaded model
	glm::mat4 loaded_model_matrix = glm::mat4(1.0f);
	loaded_model_matrix = glm::translate(loaded_model_matrix, glm::vec3(backpackPos));
	loaded_model_matrix = glm::scale(loaded_model_matrix, glm::vec3(0.5f));
	modelShader.setMatrix4("model", loaded_model_matrix);
	modelShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	// Set material struct properties
	modelShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	modelShader.setFloat("material.shininess", 16.0f);
	//
	// Point light properties
	modelShader.setVec3("pointLights[0].ambient", pl_ambientColor);
	modelShader.setVec3("pointLights[0].diffuse", pl_diffuseColor);
	modelShader.setVec3("pointLights[0].specular", pl_specularIntensity);
	// Point light attenuation properties
	modelShader.setFloat("pointLights[0].constant", 1.0f);
	modelShader.setFloat("pointLights[0].linear", 0.09f);
	modelShader.setFloat("pointLights[0].quadratic", 0.032f);
	// Point light position
	modelShader.setVec3("pointLights[0].position", movingLightPos);
	//
	// Flashlight properties
	modelShader.setBool("flashlight.on", isFlashlightOn);
	modelShader.setVec3("flashlight.ambient", fl_ambientColor);
	modelShader.setVec3("flashlight.diffuse", fl_diffuseColor);
	modelShader.setVec3("flashlight.specular", fl_specularIntensity);
	// Flashlight attenuation properties
	modelShader.setFloat("flashlight.constant", 1.0f);
	modelShader.setFloat("flashlight.linear", 0.09f);
	modelShader.setFloat("flashlight.quadratic", 0.032f);
	// Flashlight position and direction
	modelShader.setVec3("flashlight.position", camera.Position);
	modelShader.setVec3("flashlight.direction", camera.Front);
	// Flashlight cutOff angle
	modelShader.setFloat("flashlight.cutOff", glm::cos(glm::radians(5.0f)));
	modelShader.setFloat("flashlight.outerCutOff", glm::cos(glm::radians(20.0f)));
	//
	// Directional light properties
	modelShader.setVec3("dirLights[0].ambient", dl_ambientColor);
	modelShader.setVec3("dirLights[0].diffuse", dl_diffuseColor);
	modelShader.setVec3("dirLights[0].specular", dl_specularIntensity);
	// Directional light direction
	modelShader.setVec3("dirLights[0].direction", glm::vec3(1.0f, -0.5f, -1.0f));
}

void processInput(GLFWwindow* window)
{
	// Escape key to exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Toggle between modes
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Camera position movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// Light position movement
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		isMovingLight = true;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		isMovingLight = false;

	// Flashight on/off
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		isFlashlightOn = true;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		isFlashlightOn = false;

	// Outline on/off
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		isOutlineOn = true;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		isOutlineOn = false;

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	// Fixes first mouse cursor capture by OpenGL window
	if (firstMouseCapture)
	{
		lastCursorX = xpos;
		lastCursorY = ypos;
		firstMouseCapture = false;
	}
	float xOffset = xpos - lastCursorX;
	float yOffset = lastCursorY - ypos; // reverse the y-coordinates
	float cursorSensitivity = 0.08f;
	xOffset *= cursorSensitivity;
	yOffset *= cursorSensitivity;
	yaw += xOffset;
	pitch += yOffset;
	lastCursorX = xpos;
	lastCursorY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		isFlashlightOn = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		isFlashlightOn = false;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
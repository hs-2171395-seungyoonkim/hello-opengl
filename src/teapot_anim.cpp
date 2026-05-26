#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>

#include <teapot_loader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.5f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2171395 seungyoonkim", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader lampShader("shaders/14.2.lamp.vs", "shaders/14.2.lamp.fs");
	Shader teapotShader("shaders/14.2.teapot.vs", "shaders/14.2.teapot.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};


	// first, configure the lamp's VAO and VBO
	unsigned int VBO, lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(lightVAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// teapot VAO and VBO
	std::vector <float> data;
	Teapot teapot("teapot.vbo", data, 8);

	unsigned int teapotVBO, teapotVAO;
	glGenVertexArrays(1, &teapotVAO);
	glGenBuffers(1, &teapotVBO);
	glBindBuffer(GL_ARRAY_BUFFER, teapotVBO);
	glBufferData(GL_ARRAY_BUFFER, teapot.nVertNum * teapot.nVertFloats * sizeof(float), &data[0], GL_STATIC_DRAW);

	glBindVertexArray(teapotVAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, teapot.nVertFloats * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, teapot.nVertFloats * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, teapot.nVertFloats * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// --- keyframe animation setup ---
	// base rotation: teapot lies on its side by default, rotate -90 around X to stand upright
	glm::quat qBase = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// keyframe 0: upright, facing forward (no extra rotation)
	glm::quat qRot0 = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// keyframe 1: rotated 120 degrees around world Y
	glm::quat qRot1 = glm::angleAxis(glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// keyframe 2: rotated 180 degrees around Y + 45 degrees tilt around Z (using quat_cast like j11)
	glm::mat4 m2 = glm::mat4(1.0f);
	m2 = glm::rotate(m2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m2 = glm::rotate(m2, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::quat qRot2 = glm::quat_cast(m2);

	// keyframe 3: rotated 300 degrees around Y + 30 degrees tilt around X (using quat_cast like j11)
	glm::mat4 m3 = glm::mat4(1.0f);
	m3 = glm::rotate(m3, glm::radians(300.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m3 = glm::rotate(m3, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qRot3 = glm::quat_cast(m3);

	// combined keyframe quaternions: extra world-space rotation * base rotation
	// (apply base first, then extra rotation on top)
	const int NUM_KF = 4;
	glm::quat kfQ[NUM_KF] = {
		qRot0 * qBase,
		qRot1 * qBase,
		qRot2 * qBase,
		qRot3 * qBase
	};

	// keyframe translation positions (bonus: include translation in animation)
	glm::vec3 kfPos[NUM_KF] = {
		glm::vec3( 0.0f, 0.0f,  0.0f),  // keyframe 0: center
		glm::vec3( 1.5f, 0.5f,  0.0f),  // keyframe 1: right + slightly up
		glm::vec3( 0.0f, 1.2f, -0.5f),  // keyframe 2: top center + slightly back
		glm::vec3(-1.5f, 0.5f,  0.0f)   // keyframe 3: left + slightly up
	};

	const float SEG_DURATION = 2.0f; // seconds per keyframe segment
	const float TOTAL_DURATION = NUM_KF * SEG_DURATION;


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// be sure to activate shader when setting uniforms/drawing objects
		teapotShader.use();
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		teapotShader.setMat4("projection", projection);
		teapotShader.setMat4("view", view);
		// eye position
		teapotShader.setVec3("eyePos", camera.Position);

		// --- quaternion keyframe animation ---
		// compute which segment we are in and the local t in [0,1]
		float animTime = fmod(currentFrame, TOTAL_DURATION);
		int seg = (int)(animTime / SEG_DURATION);
		float t = (animTime - seg * SEG_DURATION) / SEG_DURATION;
		int nextSeg = (seg + 1) % NUM_KF;

		// rotation: spherical linear interpolation (slerp) between keyframe quaternions
		// using glm::mix for quaternions (same as j11.quaternion.cpp)
		glm::quat q = glm::mix(kfQ[seg], kfQ[nextSeg], t);

		// translation: linear interpolation between keyframe positions
		glm::vec3 pos = glm::mix(kfPos[seg], kfPos[nextSeg], t);

		// build model matrix: translate to animated position, then apply animated rotation
		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(q);

		// draw the animated teapot
		// light properties
		glm::vec3 lightColor(1.0f, 0.7f, 1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		teapotShader.setVec3("light.ambient", ambientColor);
		teapotShader.setVec3("light.diffuse", diffuseColor);
		teapotShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		teapotShader.setVec3("light.position", lightPos);
		// material properties
		teapotShader.setVec3("material.ambient", 0.25f, 0.1f, 0.3f);
		teapotShader.setVec3("material.diffuse", 0.55f, 0.3f, 0.65f);
		teapotShader.setVec3("material.specular", 0.9f, 0.8f, 0.95f); // specular lighting doesn't have full effect on this object's material
		teapotShader.setFloat("material.shininess", 48.0f);
		// world transformation
		teapotShader.setMat4("model", model);
		// render the teapot
		glBindVertexArray(teapotVAO);
		glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);

		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		// world transformation
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &teapotVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &teapotVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

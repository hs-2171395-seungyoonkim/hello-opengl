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

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lampShader("shaders/14.2.lamp.vs", "shaders/14.2.lamp.fs");
	Shader teapotShader1("shaders/14.2.teapot.vs", "shaders/14.2.teapot.fs");

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
		teapotShader1.use();
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		teapotShader1.setMat4("projection", projection);
		teapotShader1.setMat4("view", view);
		// eye position
		teapotShader1.setVec3("eyePos", camera.Position);

		// draw the teapot object 1
		// light properties
		glm::vec3 lightColor(1.0f, 0.7f, 1.0f);;
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence 
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
		teapotShader1.setVec3("light.ambient", ambientColor);
		teapotShader1.setVec3("light.diffuse", diffuseColor);
		teapotShader1.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		teapotShader1.setVec3("light.position", lightPos);
		// material properties
		teapotShader1.setVec3("material.ambient", 0.25f, 0.1f, 0.3f);
		teapotShader1.setVec3("material.diffuse", 0.55f, 0.3f, 0.65f);
		teapotShader1.setVec3("material.specular", 0.9f, 0.8f, 0.95f); // specular lighting doesn't have full effect on this object's material
		teapotShader1.setFloat("material.shininess", 48.0f);
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		teapotShader1.setMat4("model", model);
		// render the teapot
		glBindVertexArray(teapotVAO);
		glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);

		// draw the teapot object 2
		// light properties
		lightColor = glm::vec3(0.4f, 0.7f, 1.0f);;
		diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence 
		ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
		teapotShader1.setVec3("light.ambient", ambientColor);
		teapotShader1.setVec3("light.diffuse", diffuseColor);
		teapotShader1.setVec3("light.specular", 0.05f, 1.0f, 1.0f);
		teapotShader1.setVec3("light.position", lightPos);
		// material properties 과제 여기 바꿔서 주전자 6개 만들기
		teapotShader1.setVec3("material.ambient", 0.05f, 0.10f, 0.25f);
		teapotShader1.setVec3("material.diffuse", 0.15f, 0.35f, 0.85f);
		teapotShader1.setVec3("material.specular", 0.95f, 0.98f, 1.00f); // specular lighting doesn't have full effect on this object's material
		teapotShader1.setFloat("material.shininess", 128.0f);
		// world transformation
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		teapotShader1.setMat4("model", model);
		// render the teapot
		glBindVertexArray(teapotVAO);
		glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);

		// draw the teapot object 3
		// light properties
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence 
		ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
		teapotShader1.setVec3("light.ambient", ambientColor);
		teapotShader1.setVec3("light.diffuse", diffuseColor);
		teapotShader1.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		teapotShader1.setVec3("light.position", lightPos);
		// material properties
		teapotShader1.setVec3("material.ambient", 0.35f, 0.25f, 0.05f);
		teapotShader1.setVec3("material.diffuse", 1.00f, 0.5f, 0.18f);
		teapotShader1.setVec3("material.specular", 1.00f, 0.95f, 0.70f); // specular lighting doesn't have full effect on this object's material
		teapotShader1.setFloat("material.shininess", 10.0f);
		// world transformation
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		teapotShader1.setMat4("model", model);
		// render the teapot
		glBindVertexArray(teapotVAO);
		glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);

				// draw the teapot object 4
		// light properties
		lightColor = glm::vec3(0.45f, 0.45f, 0.45f);
		diffuseColor = lightColor * glm::vec3(0.55f);
		ambientColor = diffuseColor * glm::vec3(0.15f);
		teapotShader1.setVec3("light.ambient", ambientColor);
		teapotShader1.setVec3("light.diffuse", diffuseColor);
		teapotShader1.setVec3("light.specular", 0.5f, 0.5f, 0.5f);
		teapotShader1.setVec3("light.position", lightPos);

		// material properties
		teapotShader1.setVec3("material.ambient", 0.04f, 0.04f, 0.04f);
		teapotShader1.setVec3("material.diffuse", 0.10f, 0.10f, 0.10f);
		teapotShader1.setVec3("material.specular", 0.35f, 0.35f, 0.35f);
		teapotShader1.setFloat("material.shininess", 24.0f);

		// world transformation
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		teapotShader1.setMat4("model", model);

		// render the teapot
		glBindVertexArray(teapotVAO);
		glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);


		// draw the teapot object 5
		// light properties
		lightColor = glm::vec3(1.0f, 0.8f, 0.75f);
		diffuseColor = lightColor * glm::vec3(0.7f);
		ambientColor = diffuseColor * glm::vec3(0.25f);
		teapotShader1.setVec3("light.ambient", ambientColor);
		teapotShader1.setVec3("light.diffuse", diffuseColor);
		teapotShader1.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		teapotShader1.setVec3("light.position", lightPos);

		// material properties
		teapotShader1.setVec3("material.ambient", 0.45f, 0.20f, 0.22f);
		teapotShader1.setVec3("material.diffuse", 0.95f, 0.50f, 0.55f);
		teapotShader1.setVec3("material.specular", 1.0f, 0.92f, 0.95f);
		teapotShader1.setFloat("material.shininess", 96.0f);

		// world transformation
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		teapotShader1.setMat4("model", model);

		// render the teapot
		glBindVertexArray(teapotVAO);
		glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);


		// draw the teapot object 6
		// light properties
		lightColor = glm::vec3(1.0f, 0.88f, 0.72f);
		diffuseColor = lightColor * glm::vec3(0.6f);
		ambientColor = diffuseColor * glm::vec3(0.22f);
		teapotShader1.setVec3("light.ambient", ambientColor);
		teapotShader1.setVec3("light.diffuse", diffuseColor);
		teapotShader1.setVec3("light.specular", 0.4f, 0.4f, 0.4f);
		teapotShader1.setVec3("light.position", lightPos);

		// material properties
		teapotShader1.setVec3("material.ambient", 0.38f, 0.28f, 0.16f);
		teapotShader1.setVec3("material.diffuse", 0.72f, 0.58f, 0.30f);
		teapotShader1.setVec3("material.specular", 0.15f, 0.15f, 0.15f);
		teapotShader1.setFloat("material.shininess", 6.0f);

		// world transformation
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		teapotShader1.setMat4("model", model);

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



/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// vvvvvvvvvvvvvvvvvvvvvvvvvv Globals vvvvvvvvvvvvvvvvvvvvvvvvvv
// Globals generally are prefixed with 'g' in this application.

// Screen Dimensions
int gScreenWidth 						= 640;
int gScreenHeight 						= 480;
SDL_Window* gGraphicsApplicationWindow 	= nullptr;
SDL_GLContext gOpenGLContext			= nullptr;

// Main loop flag
bool gQuit = false; // If this is quit = 'true' then the program terminates.

// shader
// The following stores the a unique id for the graphics pipeline
// program object that will be used for our OpenGL draw calls.
GLuint gGraphicsPipelineShaderProgram	= 0;

// OpenGL Objects
// Vertex Array Object (VAO)
// Vertex array objects encapsulate all of the items needed to render an object.
// For example, we may have multiple vertex buffer objects (VBO) related to rendering one
// object. The VAO allows us to setup the OpenGL state to render that object using the
// correct layout and correct buffers with one call after being setup.
GLuint gVertexArrayObject					= 0;
// Vertex Buffer Object (VBO)
// Vertex Buffer Objects store information relating to vertices (e.g. positions, normals, textures)
// VBOs are our mechanism for arranging geometry on the GPU.
GLuint 	gVertexBufferObject					= 0;


// Declare a scale matrix
glm::mat4 scaleMatrix = glm::mat4(1.0f);

// Declare a offset matrix
glm::mat4 offsetMatrix = glm::mat4(1.0f);

// Declare a rotation matrix
glm::mat4 rotationMatrix = glm::mat4(1.0f);

// Shaders
// Here we setup two shaders, a vertex shader and a fragment shader.
// At a minimum, every Modern OpenGL program needs a vertex and a fragment
// shader.

// ^^^^^^^^^^^^^^^^^^^^^^^^ Globals ^^^^^^^^^^^^^^^^^^^^^^^^^^^


/**
* LoadShaderAsString takes a filepath as an argument and will read line by line a file and return a string that is meant to be compiled at runtime for a vertex, fragment, geometry, tesselation, or compute shader.
* e.g.
*       LoadShaderAsString("./shaders/filepath");
*
* @param filename Path to the shader file
* @return Entire file stored as a single string 
*/
std::string LoadShaderAsString(const std::string& filename){
    // Resulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if(myFile.is_open()){
        while(std::getline(myFile, line)){
            result += line + '\n';
        }
        myFile.close();

    }

    return result;
}


/**
* CompileShader will compile any valid vertex, fragment, geometry, tesselation, or compute shader.
* e.g.
*	    Compile a vertex shader: 	CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
*       Compile a fragment shader: 	CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
*
* @param type We use the 'type' field to determine which shader we are going to compile.
* @param source : The shader source code.
* @return id of the shaderObject
*/
GLuint CompileShader(GLuint type, const std::string& source){
	// Compile our shaders
	GLuint shaderObject;

	// Based on the type passed in, we create a shader object specifically for that
	// type.
	if(type == GL_VERTEX_SHADER){
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER){
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	// The source of our shader
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Now compile our shader
	glCompileShader(shaderObject);

	// Retrieve the result of our compilation
	int result;
	// Our goal with glGetShaderiv is to retrieve the compilation status
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE){
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; // Could also use alloca here.
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if(type == GL_VERTEX_SHADER){
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}else if(type == GL_FRAGMENT_SHADER){
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		// Reclaim our memory
		delete[] errorMessages;

		// Delete our broken shader
		glDeleteShader(shaderObject);

		return 0;
	}

  return shaderObject;
}



/**
* Creates a graphics program object (i.e. graphics pipeline) with a Vertex Shader and a Fragment Shader
*
* @param vertexShaderSource Vertex source code as a string
* @param fragmentShaderSource Fragment shader source code as a string
* @return id of the program Object
*/
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile our shaders
    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Link our two shader programs together.
	// Consider this the equivalent of taking two .cpp files, and linking them into
	// one executable file.
    glAttachShader(programObject,myVertexShader);
    glAttachShader(programObject,myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    // Once our final program Object has been created, we can
	// detach and then delete our individual shaders.
    glDetachShader(programObject,myVertexShader);
    glDetachShader(programObject,myFragmentShader);
	// Delete the individual shaders once we are done
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}


/**
* Create the graphics pipeline
*
* @return void
*/
void CreateGraphicsPipeline(){

    std::string vertexShaderSource      = LoadShaderAsString("../shaders/vert.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("../shaders/frag.glsl");

	gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);
	GLint offsetLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "offset");
}


/**
* Initialization of the graphics application. Typically this will involve setting up a window
* and the OpenGL Context (with the appropriate version)
*
* @return void
*/
void InitializeProgram(){
	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}
	
	// Setup the OpenGL Context
	// Use OpenGL 4.1 core or greater
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	// We want to request a double buffer for smooth updating.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create an application window using OpenGL that supports SDL
	gGraphicsApplicationWindow = SDL_CreateWindow( "OpenGL First Program",
													SDL_WINDOWPOS_UNDEFINED,
													SDL_WINDOWPOS_UNDEFINED,
													gScreenWidth,
													gScreenHeight,
													SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	// Check if Window did not create.
	if( gGraphicsApplicationWindow == nullptr ){
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Create an OpenGL Graphics Context
	gOpenGLContext = SDL_GL_CreateContext( gGraphicsApplicationWindow );
	if( gOpenGLContext == nullptr){
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Initialize GLAD Library
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		std::cout << "glad did not initialize" << std::endl;
		exit(1);
	}
	
}

/**
* Setup your geometry during the vertex specification step
*
* @return void
*/
void VertexSpecification(){

	// Geometry Data
	// Here we are going to store x,y, and z position attributes within vertexPositons for the data.
	// For now, this information is just stored in the CPU, and we are going to store this data
	// on the GPU shortly, in a call to glBufferData which will store this information into a
	// vertex buffer object.
	// Note: That I have segregated the data from the OpenGL calls which follow in this function.
	//       It is not strictly necessary, but I find the code is cleaner if OpenGL (GPU) related
	//       functions are packed closer together versus CPU operations.
	const std::vector<GLfloat> vertexData
	{
		-0.4f, -0.4f, 0.0f, 
		
		1.0f, 0.0f, 0.0f,	// Left vertex position
													// TODO: Interleave your color data in the vertex data
		0.4f, -0.4f, 0.0f, 
		
		0.0f, 1.0f, 0.0f, 	// right vertex position
													// TODO: Interleave your color data in the vertex data
		0.0f,  0.4f, 0.0f, 
		
		0.0f, 0.0f, 1.0f,	// Top vertex position
													// TODO: Interleave your color data in the vertex data
	};


	// Vertex Arrays Object (VAO) Setup
	// Note: We can think of the VAO as a 'wrapper around' all of the Vertex Buffer Objects,
	//       in the sense that it encapsulates all VBO state that we are setting up.
	//       Thus, it is also important that we glBindVertexArray (i.e. select the VAO we want to use)
	//       before our vertex buffer object operations.
	glGenVertexArrays(1, &gVertexArrayObject);
	// We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
	glBindVertexArray(gVertexArrayObject);

	// Vertex Buffer Object (VBO) creation
	// Create a new vertex buffer object
	// Note:  We’ll see this pattern of code often in OpenGL of creating and binding to a buffer.
	glGenBuffers(1, &gVertexBufferObject);
	// Next we will do glBindBuffer.
	// Bind is equivalent to 'selecting the active buffer object' that we want to
	// work with in OpenGL.
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	// Now, in our currently binded buffer, we populate the data from our
	// 'vertexPositions' (which is on the CPU), onto a buffer that will live
	// on the GPU.
	glBufferData(GL_ARRAY_BUFFER, 								// Kind of buffer we are working with 
																// (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
				 vertexData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
				 vertexData.data(), 						// Raw array of data
				 GL_STATIC_DRAW);								// How we intend to use the data

	// For our Given Vertex Array Object, we need to tell OpenGL
	// 'how' the information in our buffer will be used.
	glEnableVertexAttribArray(0);
	// For the specific attribute in our vertex specification, we use
	// 'glVertexAttribPointer' to figure out how we are going to move
	// through the data.
    glVertexAttribPointer(0,  		// Attribute 0 corresponds to the enabled glEnableVertexAttribArray
							  		// In the future, you'll see in our vertex shader this also correspond
							  		// to (layout=0) which selects these attributes.
                          3,  		// The number of components (e.g. x,y,z = 3 components)
                          GL_FLOAT, // Type
                          GL_FALSE, // Is the data normalized
                          sizeof(GL_FLOAT)*6, 		// Stride
                         (void*)0	// Offset
    );

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*6, (GLvoid*)(sizeof(GL_FLOAT)*3));


    // Now linking up the attributes in our VAO
    // Color information
		// TODO: Setup your vertex attribute point for color
		// 		   Make sure to setup the stride and offset positions appropriately


	// Unbind our currently bound Vertex Array Object
	glBindVertexArray(0);
	// Disable any attributes we opened in our Vertex Attribute Arrray,
	// as we do not want to leave them open. 
	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1); // TODO: Disable the attribute '1' for the vertex colors
}


/**
* PreDraw
* Typically we will use this for setting some sort of 'state'
* Note: some of the calls may take place at different stages (post-processing) of the
* 		 pipeline.
* @return void
*/
void PreDraw(){
	// Disable depth test and face culling.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor( 1.f, 1.f, 0.f, 1.f );

    //Clear color buffer and Depth Buffer
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Use our shader
	glUseProgram(gGraphicsPipelineShaderProgram);
}



/**
* Draw
* The render function gets called once per loop.
* Typically this includes 'glDraw' related calls, and the relevant setup of buffers
* for those calls.
*
* @return void
*/
void Draw(){

	GLuint scaleLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "scaleMatrix");

	glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, glm::value_ptr(scaleMatrix));

	GLuint offsetLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "offsetMatrix");

	glUniformMatrix4fv(offsetLocation, 1, GL_FALSE, glm::value_ptr(offsetMatrix));

	GLuint rotationLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "rotationMatrix");

	glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));

    // Enable our attributes
	glBindVertexArray(gVertexArrayObject);

	// Select the vertex buffer object we want to enable
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    //Render data
    glDrawArrays(GL_TRIANGLES,0,3);

	// Stop using our current graphics pipeline
	// Note: This is not necessary if we only have one graphics pipeline.
    glUseProgram(0);

}


/**
* Helper Function to get OpenGL Version Information
*
* @return void
*/
void getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}


/**
* Function called in the main application loop to handle user input
*
* @return void
*/
void Input(){
	// Event handler that handles various events in SDL
	// that are related to input and output
	const float move_speed = 0.1f;
	SDL_Event e;
	//Handle events on queue
	while(SDL_PollEvent( &e ) != 0){
		// If users posts an event to quit
		// An example is hitting the "x" in the corner of the window.
		if(e.type == SDL_QUIT){
			std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
			gQuit = true;
		}
		else if(e.type == SDL_KEYDOWN){
			switch(e.key.keysym.sym){
				case SDLK_ESCAPE:
					std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
					gQuit = true;
					break;
				case SDLK_LEFT:
					std::cout << "LEFT" << std::endl;
					offsetMatrix = glm::translate(offsetMatrix, glm::vec3(-move_speed, 0.0f, 0.0f));
					break;
				case SDLK_RIGHT:
					std::cout << "RIGHT" << std::endl;
					offsetMatrix = glm::translate(offsetMatrix, glm::vec3(move_speed, 0.0f, 0.0f));
					break;
				case SDLK_UP:
					std::cout << "UP" << std::endl;
					offsetMatrix = glm::translate(offsetMatrix, glm::vec3(0.0f, move_speed, 0.0f));
					break;
				case SDLK_DOWN:
					std::cout << "DOWN" << std::endl;
					offsetMatrix = glm::translate(offsetMatrix, glm::vec3(0.0f, -move_speed, 0.0f));
					break;
				case SDLK_q:
					std::cout << "Scale Up" << std::endl;
					scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1.1f, 1.1f, 1.1f));
					break;
				case SDLK_e:
					std::cout << "Scale Down" << std::endl;
					scaleMatrix = glm::scale(scaleMatrix, glm::vec3(0.9f, 0.9f, 0.9f));
					break;
				case SDLK_a:
					std::cout << "Rotate Left" << std::endl;
					rotationMatrix = glm::rotate(rotationMatrix, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case SDLK_d:
					std::cout << "Rotate Right" << std::endl;
					rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case SDLK_w:
					std::cout << "Rotate Up" << std::endl;
					rotationMatrix = glm::rotate(rotationMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				case SDLK_s:
					std::cout << "Rotate Down" << std::endl;
					rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				default:
					break;
			}
		}
	}
}


/**
* Main Application Loop
* This is an infinite loop
*
* @return void
*/
void MainLoop(){

	// While application is running
	while(!gQuit){
		// Handle Input
		Input();
		// Setup anything (i.e. OpenGL State) that needs to take
		// place before draw calls
		PreDraw();
		// Draw Calls in OpenGL
		Draw();
		//Update screen of our specified window
		SDL_GL_SwapWindow(gGraphicsApplicationWindow);
	}
}



/**
* The last function called in the program
* This functions responsibility is to destroy any global
* objects in which we have create dmemory.
*
* @return void
*/
void CleanUp(){
	//Destroy our SDL2 Window
	SDL_DestroyWindow(gGraphicsApplicationWindow );
	gGraphicsApplicationWindow = nullptr;

    // Delete our OpenGL Objects
    glDeleteBuffers(1, &gVertexBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

	// Delete our Graphics pipeline
    glDeleteProgram(gGraphicsPipelineShaderProgram);

	//Quit SDL subsystems
	SDL_Quit();
}


/**
* The main entry point into our C++ programs.
*
* @return program status
*/
int main( int argc, char* args[] ){

	// 1. Setup the graphics program
	InitializeProgram();
	
	// 2. Setup our geometry
	VertexSpecification();
	
	// 3. Create our graphics pipeline
	// 	- At a minimum, this means the vertex and fragment shader
	CreateGraphicsPipeline();
	
	// 4. Call the main application loop
	MainLoop();	

	// 5. Call the cleanup function when our program terminates
	CleanUp();

	return 0;
}

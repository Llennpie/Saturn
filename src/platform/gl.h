#if   __defined(TARGET_WINDOWS) || __defined(TARGET_OSX)
#define GLEW_STATIC
#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>
#elif __defined(TARGET_LINUX)
#include <gl/gl.h>
#elif __defined(TARGET_SWITCH)
#include <glad/glad.h>
#endif
#include "init.h"

static void initSDL(const char*, int, int);
static void initAudioContext(void);
static void cleanup(void);

static bool debugMode = false;

void initGame(const char* windowName, int windowWidth, int windowHeight) {
  initSDL(windowName, windowWidth, windowHeight);
  initSounds();
  initFonts();

  // Assigns the callback function to clean up the
  // SDL context when closing the program.
  atexit(cleanup);
}

void toggleDebugMode(bool db) {
  debugMode = db;
}

/*
 * Initializes the SDL context, renderer, and
 * window.
 */
static void initSDL(const char* windowName, int windowWidth, int windowHeight) {
  int rendererFlags;
  int windowFlags;

  rendererFlags = SDL_RENDERER_ACCELERATED;
  windowFlags   = 0;

  if (debugMode) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initialization started.");
  }

  memset(&app, 0, sizeof(App));

  // Initialize SDL and exit if we fail.
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL: %s.\n", SDL_GetError());
    exit(EXIT_ERROR);
  }

  // Initialize the SDL window.
  app.window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, windowFlags);

  if (!app.window) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not open window. %s.\n", SDL_GetError());
    exit(EXIT_ERROR);
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  // Create renderer with the default graphics context.
  app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

  if (!app.renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize renderer: %s.\n", SDL_GetError());
    exit(EXIT_ERROR);
  }

  if (debugMode) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initialization Completed.");
  }

  //  Initialize SDL to accept both JPG and PNGs.
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

  //  Remove cursor.
  SDL_ShowCursor(false);

  initAudioContext();
}

/*
 *
 */
static void initAudioContext(void) {
  if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) == -1) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL Mixer.\n");
    exit(EXIT_ERROR);
  }

  Mix_AllocateChannels(MAX_SND_CHANNELS);
}

/*
 * Cleans up the SDL context and game upon closing the application.
 */
static void cleanup(void) {
  if (debugMode) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Cleaning up.");
  }
	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();

  if (debugMode) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Program quit.");
  }
}
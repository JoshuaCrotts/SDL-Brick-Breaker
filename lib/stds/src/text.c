#include "../include/draw.h"

#define DEFAULT_FONT_COLOR 0xFF

static char         text_buffer[MAX_LINE_LENGTH];
static SDL_Surface *message_surface;
static SDL_Texture *message_texture;
static SDL_Rect     message_rect;

static TTF_Font *get_font( const char *, uint16_t );
static void      load_fonts();
static void      add_font( const char *, uint16_t );

/**
 * Initializes the TTF font library for use.
 * 
 * @param void.
 * 
 * @return void.
 */
void
init_fonts( void ) {
  app.font_tail = &app.font_head;
  if ( TTF_Init() == -1 ) {
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize TTF_Init: %s.\n",
                 SDL_GetError() );
    exit( EXIT_FAILURE );
  }

  load_fonts();
}

/**
 * Draws a string of text specified by the const char *parameter, supplemented
 * by whatever formatting arguments are necessary.
 *
 * @param float x coordinate (top-left) of string.
 * @param float y coordinate (top-left) of string.
 * @param uint8_t red color value (0-255).
 * @param uint8_t green color value (0-255).
 * @param uint8_t blue color value (0-255).
 * @param const char *font name (use the file name itself with the extension).
 * @param uint16_t font size.
 * @param const char *string to draw.
 * @param ... formatting args.
 *
 * To center a string, call get_string_size() or get_string_sizeFont (if using a non
 * standard font), and draw the string at SCREEN_WIDTH / 2 - fontWidth / 2.
 *
 * @return void.
 */
void
draw_text( float x, float y, uint8_t r, uint8_t g, uint8_t b, const char *font_string,
           uint16_t font_size, const char *text, ... ) {
  message_rect.x = ( uint16_t ) x;
  message_rect.y = ( uint16_t ) y;

  va_list args;
  memset( &text_buffer, '\0', sizeof( text_buffer ) );

  va_start( args, text );
  vsprintf( text_buffer, text, args );
  va_end( args );

  SDL_Color textColor = {r, g, b};
  TTF_Font *font      = get_font( font_string, font_size );
  message_surface     = TTF_RenderText_Solid( font, text_buffer, textColor );
  TTF_SizeText( font, text_buffer, &message_rect.w, &message_rect.h );

  if ( message_surface == NULL ) {
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Failed to write message: %s.\n", SDL_GetError() );
    exit( EXIT_ERROR );
  }

  message_texture = SDL_CreateTextureFromSurface( app.renderer, message_surface );
  SDL_RenderCopy( app.renderer, message_texture, NULL, &message_rect );
  SDL_DestroyTexture( message_texture );
  SDL_FreeSurface( message_surface );
}

/**
 * Frees the fonts that are in use by the standards library.
 * 
 * @param void.
 * 
 * @return void.
 */
void
free_fonts() {
  SDL_DestroyTexture( message_texture );
  SDL_FreeSurface( message_surface );

  font_t *f;
  f = app.font_head.next;

  while ( f != NULL ) {
    f                  = app.font_head.next;
    app.font_head.next = f->next;
    printf("Freeing font.\n");
    free( f );
  }

  TTF_Quit();
}

/**
 * Computes the size of the string with the default font were it to be drawn
 * to the screen in pixels. This is useful for positioning the string in the middle
 * of the screen if need-be.
 *
 * @param const char *string.
 * @param const char *font name.
 * @param uint16_t font size.
 * @param pointer to integer (int) where the width of the string is stored.
 * @param pointer to integer (int) where the height of the string is stored.
 *
 * @return void.
 */
void
get_string_size( const char *s, const char *font, uint16_t size, int *w, int *h ) {
  TTF_Font *f;
  f = get_font( font, size );

  if ( f != NULL ) {
    TTF_SizeText( f, s, w, h );
  } else {
    exit( EXIT_FAILURE );
  }
}

/**
 *
 * 
 * @param
 * @param
 * 
 * @return void.
 */
static TTF_Font *
get_font( const char *font_str, uint16_t font_size ) {
  font_t *f;

  for ( f = app.font_head.next; f != NULL; f = f->next ) {
    if ( strcmp( f->name, font_str ) == 0 && f->size == font_size ) {
      return f->font;
    }
  }

  if ( f == NULL ) {
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Could not find font %s, %d.", font_str, font_size );
  }

  return NULL;
}

/**
 *
 * 
 * @param void.
 * 
 * @return void.
 */
static void
load_fonts( void ) {
  add_font( "res/fonts/nes.ttf", 12 );
  add_font( "res/fonts/nes.ttf", 18 );
  add_font( "res/fonts/nes.ttf", 24 );
}

/**
 * 
 * 
 * @param 
 * @param
 *
 * @return void.
 */
static void
add_font( const char *font_file, uint16_t size ) {
  font_t *f;
  f = malloc( sizeof( font_t ) );

  if ( f == NULL ) {
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Could not allocate memory for font_t. %s.\n",
                 SDL_GetError() );
    exit( EXIT_FAILURE );
  }

  memset( f, 0, sizeof( font_t ) );

  f->font = TTF_OpenFont( font_file, size );

  if ( f->font == NULL ) {
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Could not load font_t %s, %d. Is the path correct?",
                 font_file, size );
  }

  strcpy( f->name, font_file );
  f->size = size;

  app.font_tail->next = f;
  app.font_tail       = f;
}
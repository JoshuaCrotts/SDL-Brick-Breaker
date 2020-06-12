#include "main.h"

static void initScene(void);
static void draw(void);
static void tick(void);
static void cleanupStage(void);

static void createEmitter(int32_t, int32_t, uint32_t, uint32_t);
static void updateEmitters();
static void updateEntities();
static void drawEmitters();
static void drawEntities();
static void ballHitPaddle(Entity*);

// Barebones game. This is the minimum amount of code
// necessary to run a window.
int main(int argc, char* argv[]) {
  initGame("Standards C Library", SCREEN_WIDTH, SCREEN_HEIGHT);
  initScene();
  loop();

  atexit(cleanupStage);
  return 0;
}

/*
 * Initializes the delegation structure to use the
 * tick and draw methods for rendering and updating
 * the game. These methods must be defined because the
 * delegation structure uses them in the loop function.
 *
 * Sprites and structs are also initalized here.
 */
static void initScene(void) {
  app.delegate.tick = tick;
  app.delegate.draw = draw;

  memset(&stage, 0, sizeof(Stage));
  app.textureTail = &app.textureHead;
  stage.emitterTail = &stage.emitterHead;
  stage.entityTail = &stage.entityHead;

  init_paddle();
  init_background();
}

/*
 *
 */
static void tick(void) {
  //updateCamera(player);
  background_update();
  updateEmitters();
  updateEntities();
  paddle_update();

  if (app.mouse.button[SDL_BUTTON_LEFT]) {
    Entity* b = add_ball(app.mouse.x, app.mouse.y);
    stage.entityTail->next = b;
    stage.entityTail = b;
    //spawnColorfulParticles(NULL, app.mouse.x, app.mouse.y, 16, ID_P_ANIMATED_PARTICLE_MASK);
    app.mouse.button[SDL_BUTTON_LEFT] = 0;
  }
}

/*
 *
 */
static void draw(void) {
  background_draw();
  drawEmitters();
  drawEntities();
  paddle_draw();
}

/*
 *
 */
static void createEmitter(int32_t x, int32_t y, uint32_t maxParticles, uint32_t flags) {
  Emitter* em;

  em = create_emitter(x, y, maxParticles, flags);

  stage.emitterTail->next = em;
  stage.emitterTail = em;
}

/*
 *
 */
static void updateEmitters(void) {
  Emitter* em;
  Emitter* prev;

  prev = &stage.emitterHead;

  for (em = stage.emitterHead.next; em != NULL; em = em->next) {
    emitter_update(em);

    if (em->flags & DEATH_MASK) {
      if (em == stage.emitterTail) {
        stage.emitterTail = prev;
      }

      prev->next = em->next;
      free(em);
      em = prev;
    }
    prev = em;
  }
}

/*
 *
 */
static void updateEntities(void) {
  Entity* e;
  Entity* prev;

  prev = &stage.entityHead;

  for (e = stage.entityHead.next; e != NULL; e = e->next) {
    if (e->idFlags & ID_PARTICLE_MASK) 
    {
      particle_tick(e);
    } 
    else if (e->idFlags & ID_BALL_MASK) 
    {
      ballHitPaddle(e);
      ball_update(e);
    }
    else if (e->tick) 
    {
      e->tick(e);
    }

    if (e->flags & DEATH_MASK) {
      if (e == stage.entityTail) {
          stage.entityTail = prev;
      }

      prev->next = e->next;
      free(e);
      e = prev;
    }
    prev = e;
  }
}

/*
 *
 */
static void drawEmitters(void) {
  Emitter* em;

  for (em = stage.emitterHead.next; em != NULL; em = em->next) {
    emitter_draw(em);
  }
}

/*
 *
 */
static void drawEntities(void) {
  Entity* e;

  for (e = stage.entityHead.next; e != NULL; e = e->next) {
    if (e->idFlags & ID_PARTICLE_MASK) 
    {
      particle_draw(e);
    } 
    else if (e->idFlags & ID_BALL_MASK) 
    {
      ball_draw(e);
    }
    else if (e->draw) 
    {
      e->draw(e);
    }
  }
}

/*
 *
 */
static void ballHitPaddle(Entity* b) {

  if (collision((int32_t) paddle->x, (int32_t) paddle->y, (int32_t) paddle->w, (int32_t) paddle->h, 
                (int32_t) b->x, (int32_t) b->y, (int32_t) b->w, (int32_t) b->h)) {
    b->dy = -b->dy;
    return;
  }
}

/*
 *
 */
static void cleanupStage(void) {
  Animation* a;
  Emitter* em;
  Entity* en;

  while (stage.animationHead.next) {
    a = stage.animationHead.next;
    stage.animationHead.next = a->next;
    free(a);
  }

   while (stage.emitterHead.next) {
    em = stage.emitterHead.next;
    stage.emitterHead.next = em->next;
    free(em);
  }

  while (stage.entityHead.next) {
    en = stage.entityHead.next;
    stage.entityHead.next = en->next;
    free(en);
  }

  paddle_die();
  background_die();
}
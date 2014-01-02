/**
 * This is a demo programm for the "pebble_three_dee.h" which have been developed
 * by me as a spare time project. 
 *
 * Feel free to use it for your projects and to extend it. I would appreciate if you mention the
 * library as "pebble_three_dee lib by Daniel Hug." on your project site and/or in your app.
 *
 * (C) Daniel Hug., January 2014, Germany
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <pebble.h>
#include <pebble_three_dee.h>

static Window *window;
static Layer *layer;

int frames = 0;

// WORLD
Mesh *cube1;
Mesh *cube2;
Mesh *cube3;

Camera *cam;
Matrix *viewMatrix;
Matrix *projectionMatrix;
Matrix *transformMatrix;
Matrix *worldMatrix;
Matrix *translateMatrix;

//
// UPDATE OBJECT PROPERTIES
//

static void update_world(){
  cube1->rotation.x += 0.05;
  cube1->rotation.y += 0.05;
  
  cube2->rotation.x -= 0.06;
  cube2->rotation.y -= 0.07;
  
  cube3->rotation.x += 0.02;
  cube3->rotation.z -= 0.04;
}

//
// DRAW
//

static void draw(Layer *me, GContext* ctx) {
  const GRect bounds = layer_get_bounds(me);
  const uint8_t h = bounds.size.h;
  const uint8_t w = bounds.size.w;
  
  // neccessary precalculations
  calc_pre_drawing(cam, w, h, viewMatrix, projectionMatrix);
  
  draw_mesh(ctx, cube1, viewMatrix, projectionMatrix, w, h, worldMatrix, translateMatrix, transformMatrix);
  draw_mesh(ctx, cube2, viewMatrix, projectionMatrix, w, h, worldMatrix, translateMatrix, transformMatrix);
  draw_mesh(ctx, cube3, viewMatrix, projectionMatrix, w, h, worldMatrix, translateMatrix, transformMatrix);
  
  // count frames
  ++frames;
}

// convenient method to create simple cubes
static void make_basic_cube(Mesh* mesh, float side_length){
  // init position and rotation to (0,0,0)
  mesh->position = null_vector;
  mesh->rotation = null_vector;
  
  // create 8 vertices of a cube of the given side length
  const float num = 0.5 * side_length;
  mesh->num_vertices = 8;
  mesh->vertices = malloc(8 * sizeof(Vector));
  mesh->proj = malloc(8 * sizeof(Vector2));
  
  set_vector(&mesh->vertices[0], -num, num, num);
  set_vector(&mesh->vertices[1], num, num, num);
  set_vector(&mesh->vertices[2], -num, -num, num);
  set_vector(&mesh->vertices[3], num, -num, num);
  
  set_vector(&mesh->vertices[4], -num, num, -num);
  set_vector(&mesh->vertices[5], num, num, -num);
  set_vector(&mesh->vertices[6], num, -num, -num);
  set_vector(&mesh->vertices[7], -num, -num, -num);
  
  // create the faces out of adjacent vertices
  mesh->num_faces = 12;
  mesh->faces = malloc(12 * sizeof(Triangle));
  
  set_triangle(&mesh->faces[0], 0, 1, 2);
  set_triangle(&mesh->faces[1], 1, 2, 3);
  set_triangle(&mesh->faces[2], 1, 3, 6);
  set_triangle(&mesh->faces[3], 1, 5, 6);
  set_triangle(&mesh->faces[4], 0, 1, 4);
  set_triangle(&mesh->faces[5], 1, 4, 5);
  
  set_triangle(&mesh->faces[6], 2, 3, 7);
  set_triangle(&mesh->faces[7], 3, 6, 7);
  set_triangle(&mesh->faces[8], 0, 2, 7);
  set_triangle(&mesh->faces[9], 0, 4, 7);
  set_triangle(&mesh->faces[10], 4, 5, 6);
  set_triangle(&mesh->faces[11], 4, 6, 7);
}

//
// INIT
//

static void init_world(){
  // create matrixes
  viewMatrix = malloc(sizeof(Matrix));
  projectionMatrix = malloc(sizeof(Matrix));
  transformMatrix = malloc(sizeof(Matrix));
  worldMatrix = malloc(sizeof(Matrix));
  translateMatrix = malloc(sizeof(Matrix));

  // create camera
  cam = malloc(sizeof(Camera));
  cam->target = null_vector;
  set_vector(&(cam->position), 0, 0, 15);

  // create cubes  
  cube1 = malloc(sizeof(Mesh));
  make_basic_cube(cube1, 2.0); // top
  cube1->position.y = -1;
  
  cube2 = malloc(sizeof(Mesh));
  make_basic_cube(cube2, 1.2);  // bottom left
  cube2->position.x = +1.3;
  cube2->position.y = +1.85;
  
  cube3 = malloc(sizeof(Mesh));
  make_basic_cube(cube3, 1.2);  // bottom right
  cube3->position.x = -1.3;
  cube3->position.y = +1.85;
}

static void deinit_world(){
  free(cam);
  
  free_mesh(cube1);
  free_mesh(cube2);
  free_mesh(cube3);
  
  free(viewMatrix);
  free(projectionMatrix);
  free(transformMatrix);
  free(worldMatrix);
  free(translateMatrix);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);
  layer_set_update_proc(layer, draw);
  layer_add_child(window_layer, layer);
}

static void window_unload(Window *window) {
  layer_destroy(layer);
}

#define TIMER_INTERVAL 33

void handle_timer(void *data){
  app_timer_register(TIMER_INTERVAL, handle_timer, NULL);
  update_world();
  layer_mark_dirty(layer); // this will call our draw method :)
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "FPS: %d", frames);
  frames = 0;
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_fullscreen(window, true);
  window_stack_push(window, false);
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick); // used for outputting fps
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init_world();
  init();

  app_timer_register(TIMER_INTERVAL, handle_timer, NULL);
  app_event_loop();
  deinit();
  deinit_world();
}

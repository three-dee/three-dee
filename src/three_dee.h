/**
 * This is a demo programm for the "three_dee lib" which have been developed
 * by me as a fun project in winter 2013/2014.
 *
 * https://github.com/three-dee/three-dee
 *
 * It allows to create basic 3d objects (mesh) in a 3d world and render them on
 * the pebble watch in real-time. Currently the rendering is limited to wire-frame models.
 *
 * Quickstart: See the three_dee.c demo application.
 *
 * The vector/matrix operations have been implemented with reference of the babylon.math.js file
 * of the babylon.js framework. The fast math and fast drawing methods are inspired by wikipedia.org
 *
 * Feel free to use it for your projects and to extend it. I would appreciate if you mention the
 * library as "three_dee lib by Daniel Hug." on your project site and/or in your app.
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


//
// FAST MATH OPERATIONS ON FLOATS
//

#define __THREE_DEE_VERSION__ "0.1beta"

#define PI_1 3.14159265
#define PI_2 6.28318531

static float sin_fast(float x){    
  float sin;
  if (x < -PI_1)
    x += PI_2;
  if (x >  PI_1)
    x -= PI_2;
  
  if (x < 0)  {
    sin = 1.2732395 * x + .40528473 * x * x;

    if (sin < 0)
      sin = .225 * (sin *-sin - sin) + sin;
    else
      sin = .225 * (sin * sin - sin) + sin;
  } else {
    sin = 1.2732395 * x - .40528473 * x * x;

    if (sin < 0)
      sin = .225 * (sin *-sin - sin) + sin;
    else
      sin = .225 * (sin * sin - sin) + sin;
  }
  return sin;
}

inline static float cos_fast(float x){
  return sin_fast(x + 1.57079632);
}

// See http://en.wikipedia.org/wiki/Fast_inverse_square_root for more details;
// In our last step we multiply with 'x' in order to make '1/sqrt(x)' to 'sqrt(x)'.
#define SQRT_MAGIC_F 0x5f3759df 
inline float  sqrt_fast(const float x){
  union
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1); 
  
  // newton step for increased accuracy
  return x * u.x * (1.5f - 0.5f * x * u.x * u.x);
}

inline int abs(const int i){
  return (i < 0) ? -i : i;
}

inline float absf(const float i){
  return (i < 0.0) ? -i : i;
}

//
// FAST DRAWING HELPER
//

inline void draw_line(GContext* ctx, int x0, int y0, int x1, int y1){
  // Bresenham
  const int dx = abs(x1 - x0);
  const int dy = abs(y1 - y0);
  const int sx = (x0 < x1) ? 1 : -1;
  const int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  int e2;
  while (1) {
    graphics_draw_pixel(ctx, (GPoint) {x0,y0});

    if ((x0 == x1) && (y0 == y1)) break;
    e2 = err << 1;
    if (e2 > -dy) { err -= dy; x0 += sx; }
    if (e2 < dx) { err += dx; y0 += sy; }
  }
}

//
// VECTOR
//

typedef struct Vector{
  float x;
  float y;
  float z;
} Vector;

typedef struct Vector2{
  float x;
  float y;
} Vector2;

typedef struct Vector2_int{
  int x;
  int y;
} Vector2_int;

#define null_vector ((Vector){(float)0.0,(float)0.0,(float)0.0})
#define up_vector ((Vector){(float)0.0,(float)1.0,(float)0.0})

inline float length(Vector* v){
  const float x = v->x; const float y = v->y; const float z = v->z;
  return sqrt_fast(x*x+y*y+z*z);
}

inline void normalize(Vector* v){
  const float scale = 1.0 / length(v);
  v->x *= scale; v->y *= scale; v->z *= scale;
}

inline void set_vector(Vector* v, float x, float y, float z){
  v->x=x,v->y=y,v->z=z;
}

// c = a - b;
inline void substract_to_ref(Vector* a, Vector* b, Vector *c){
  c->x = a->x - b->x;
  c->y = a->y - b->y;
  c->z = a->z - b->z;
}

// c = a x b
inline void cross_to_ref(Vector* a, Vector* b, Vector *c){
  c->x = a->y * b->z - a->z * b->y;
  c->y = a->z * b->x - a->x * b->z;
  c->z = a->x * b->y - a->y * b->x;
}

inline float dot_product(Vector* a, Vector* b){
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

//
// MATRIX
//

typedef struct {
  float m11, m12, m13, m14;
  float m21, m22, m23, m24;
  float m31, m32, m33, m34;
  float m41, m42, m43, m44;
} Matrix;

#define identity_matrix ((Matrix){(float)1.0,(float)0.0,(float)0.0,(float)0.0,(float)0.0,(float)1.0,(float)0.0,(float)0.0,(float)0.0,(float)0.0,(float)1.0,(float)0.0,(float)0.0,(float)0.0,(float)0.0,(float)1.0})

inline void set_matrix(Matrix* m,
  float m11, float m12, float m13, float m14, 
  float m21, float m22, float m23, float m24, 
  float m31, float m32, float m33, float m34, 
  float m41, float m42, float m43, float m44){
  
  m->m11=m11;m->m12=m12;m->m13=m13;m->m14=m14;
  m->m21=m21;m->m22=m22;m->m23=m23;m->m24=m24;
  m->m31=m31;m->m32=m32;m->m33=m33;m->m34=m34;
  m->m41=m41;m->m42=m42;m->m43=m43;m->m44=m44;
};

void look_at_LH_to_ref(Vector* eye, Vector* target, Vector* up, Matrix* result){
  static Vector xAxis;
  static Vector yAxis;
  static Vector zAxis;
  
  substract_to_ref(target, eye, &zAxis);
  normalize(&zAxis);
  
  cross_to_ref(up, &zAxis, &xAxis);
  normalize(&xAxis);
  
  cross_to_ref(&zAxis, &xAxis, &yAxis);
  normalize(&yAxis);
  
  float x = -dot_product(&xAxis, eye);
  float y = -dot_product(&yAxis, eye);
  float z = -dot_product(&zAxis, eye);
  
  set_matrix(result,
    xAxis.x, yAxis.x, zAxis.x, 0,
    xAxis.y, yAxis.y, zAxis.y, 0,
    xAxis.z, yAxis.z, zAxis.z, 0,
    x, y, z, 1);
}

void perspective_for_LH_to_ref(float fov, float aspect, float znear, float zfar, Matrix* m){
  const float tan = 2.56;
  m->m11 = tan / aspect;
  m->m12 = m->m13 = m->m14 = 0.0;
  
  m->m22 = tan;
  m->m21 = m->m23 = m->m24 = 0.0;
  
  m->m31 = m->m32 = 0.0;
  m->m33 = -zfar / (znear - zfar);
  m->m34 = 1.0;
  
  m->m41 = m->m42 = m->m43 = 0.0;
  m->m44 = (znear * zfar) / (znear- zfar);
}

void matrix_multiply_to_ref(Matrix* a, Matrix* b, Matrix* c){
  c->m11 = a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31 + a->m14 * b->m41;
  c->m12 = a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32 + a->m14 * b->m42;
  c->m13 = a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33 + a->m14 * b->m43;
  c->m14 = a->m11 * b->m14 + a->m12 * b->m24 + a->m13 * b->m34 + a->m14 * b->m44;
  
  c->m21 = a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31 + a->m24 * b->m41;
  c->m22 = a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32 + a->m24 * b->m42;
  c->m23 = a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33 + a->m24 * b->m43;
  c->m24 = a->m21 * b->m14 + a->m22 * b->m24 + a->m23 * b->m34 + a->m24 * b->m44;
  
  c->m31 = a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31 + a->m34 * b->m41;
  c->m32 = a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32 + a->m34 * b->m42;
  c->m33 = a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33 + a->m34 * b->m43;
  c->m34 = a->m31 * b->m14 + a->m32 * b->m24 + a->m33 * b->m34 + a->m34 * b->m44;
  
  c->m41 = a->m41 * b->m11 + a->m42 * b->m21 + a->m43 * b->m31 + a->m44 * b->m41;
  c->m42 = a->m41 * b->m12 + a->m42 * b->m22 + a->m43 * b->m32 + a->m44 * b->m42;
  c->m43 = a->m41 * b->m13 + a->m42 * b->m23 + a->m43 * b->m33 + a->m44 * b->m43;
  c->m44 = a->m41 * b->m14 + a->m42 * b->m24 + a->m43 * b->m34 + a->m44 * b->m44;  
}

void yaw_pitch_roll_to_ref(float yaw, float Pitch, float roll, Matrix* m){
  const float halfRoll = roll * 0.5;
  const float halfPitch = Pitch * 0.5;
  const float halfYaw = yaw * 0.5;

  const float sinRoll = sin_fast(halfRoll);
  const float cosRoll = cos_fast(halfRoll);
  const float sinPitch = sin_fast(halfPitch);
  const float cosPitch = cos_fast(halfPitch);
  const float sinYaw = sin_fast(halfYaw);
  const float cosYaw = cos_fast(halfYaw);

  const float x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
  const float y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
  const float z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
  const float w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
      
  const float xx = x * x;
  const float yy = y * y;
  const float zz = z * z;
  const float xy = x * y;
  const float zw = z * w;
  const float zx = z * x;
  const float yw = y * w;
  const float yz = y * z;
  const float xw = x * w;

  m->m11 = 1.0 - (2.0 * (yy + zz));
  m->m12 = 2.0 * (xy + zw);
  m->m13 = 2.0 * (zx - yw);
  m->m14 = 0.0;
  
  m->m21 = 2.0 * (xy - zw);
  m->m22 = 1.0 - (2.0 * (zz + xx));
  m->m23 = 2.0 * (yz + xw);
  m->m24 = 0.0;
  
  m->m31 = 2.0 * (zx + yw);
  m->m32 = 2.0 * (yz - xw);
  m->m33 = 1.0 - (2.0 * (yy + xx));
  m->m34 = 0.0;
  
  m->m41 = 0.0;
  m->m42 = 0.0;
  m->m43 = 0.0;
  m->m44 = 1.0;
}

void translation_to_ref(float x, float y, float z, Matrix* m){
  m->m11 = 1.0;
  m->m12 = 0.0;
  m->m13 = 0.0;
  m->m14 = 0.0;
  
  m->m21 = 0.0;  
  m->m22 = 1.0;
  m->m23 = 0.0;
  m->m24 = 0.0;
  
  m->m31 = 0.0;
  m->m32 = 0.0;
  m->m33 = 1.0;
  m->m34 = 0.0;
  
  m->m41 = x;
  m->m42 = y;
  m->m43 = z;
  m->m44 = 1.0;
}

void transform_coords(Vector* v, Matrix* m, Vector2* r){
  r->x = v->x * m->m11 + v->y * m->m21 + v->z * m->m31 + m->m41;
  r->y = v->x * m->m12 + v->y * m->m22 + v->z * m->m32 + m->m42;
  const float w = v->x * m->m14 + v->y * m->m24 + v->z * m->m34 + m->m44;
  
  r->x /= w;
  r->y /= w;
}

//
// CAMERA
//

typedef struct {
  Vector position;
  Vector target;
} Camera;

//
// MESH
//

typedef struct {
  uint8_t a; uint8_t b; uint8_t c;
} Triangle;

inline void set_triangle(Triangle* t, uint8_t a, uint8_t b, uint8_t c){t->a=a;t->b=b;t->c=c;};

typedef struct {
  Vector position;
  Vector rotation;
  
  // vertices
  uint8_t num_vertices;  
  Vector* vertices;
  
  // faces = triangels
  uint8_t num_faces;
  Triangle* faces;
  
  Vector2* proj; /* contains cached values for display PI_1xels */  
} Mesh;

static void free_mesh(Mesh* mesh){  
  free(mesh->vertices);
  free(mesh->faces);
  free(mesh->proj);
  free(mesh);
}

// 
// SCREEN DRAWING METHODS
//

// takes the camera, width and height and calculates approperiate view and projection matrizes
inline void calc_pre_drawing(Camera* cam, int w, int h, Matrix* viewMatrix, Matrix* projectionMatrix){
  // Calc View Matrix
  look_at_LH_to_ref(&cam->position, &cam->target, &up_vector, viewMatrix);
  
  // Calc Projection Matrix
  perspective_for_LH_to_ref(0.78, ((float)w)/((float)h), 0.01, 1.0, projectionMatrix);  
}

inline void fix_mesh_rotation(Mesh* mesh){
  while (mesh->rotation.x < -PI_1)
      mesh->rotation.x += PI_2;
  while (mesh->rotation.x > -PI_1)
      mesh->rotation.x -= PI_2;
      
  while (mesh->rotation.y < -PI_1)
      mesh->rotation.y += PI_2;
  while (mesh->rotation.y > -PI_1)
      mesh->rotation.y -= PI_2;
      
  while (mesh->rotation.z < -PI_1)
      mesh->rotation.z += PI_2;
  while (mesh->rotation.z > -PI_1)
      mesh->rotation.z -= PI_2;
}

// Draws a mesh on a GContext of a layer
//
// You have to specify everything except the worldMatrix, the translateMatrix and the transformMatrix which are calculated
// here (however you are responsible for providing memory for them)
void draw_mesh(
  GContext* ctx, 
  Mesh* mesh, 
  Matrix* viewMatrix, Matrix* projectionMatrix, 
  int w, int h, 
  Matrix* worldMatrix, Matrix* translateMatrix, Matrix* transformMatrix){
  
  uint8_t i;
  const uint8_t num_vertices = mesh->num_vertices;
  const uint8_t num_faces = mesh->num_faces;
  
  uint8_t a, b, c;
  uint8_t ax, ay, bx, by, cx, cy;
  
  // fixes the rotation constants when they are outside the range [-PI_1/2, +PI_1/2]
  // this is required by our fast-math sinus/cosinus functions
  fix_mesh_rotation(mesh);
  
  // calc coordinates on screen of vertices
  Vector2 proj;
  for(i=0; i<num_vertices; i++){
    // get rotation matrix
    yaw_pitch_roll_to_ref(mesh->rotation.x, mesh->rotation.y, mesh->rotation.z, worldMatrix);
    
    // apply translation on it (multiplication)
    translation_to_ref(mesh->position.x, mesh->position.y, mesh->position.z, translateMatrix);
    matrix_multiply_to_ref(worldMatrix, translateMatrix, transformMatrix);
    
    // apply view matrix (multiplication)
    matrix_multiply_to_ref(transformMatrix, viewMatrix, transformMatrix);
    
    // apply projection matrix (multiplication)
    matrix_multiply_to_ref(transformMatrix, projectionMatrix, transformMatrix);
    
    // project coordinates in two-dimensional space
    transform_coords(&mesh->vertices[i], transformMatrix, &proj);
    
    mesh->proj[i].x = (proj.x * w + w * 0.5);
    mesh->proj[i].y = (proj.y * h + h * 0.5);
    //graphics_draw_pixel(ctx, (GPoint) {mesh->proj[i].x, mesh->proj[i].y});
  }
  
  // draw faces (requires the mesh->proj[i] vectors to be precompted
  // TODO: allow custom 'color' using: graphics_context_set_stroke_color(ctx, color);
  for(i=0; i<num_faces; i++){
    a = mesh->faces[i].a;
    b = mesh->faces[i].b;
    c = mesh->faces[i].c;
    
    ax = (mesh->proj[a].x);
    ay = (mesh->proj[a].y);
    bx = (mesh->proj[b].x);
    by = (mesh->proj[b].y);
    cx = (mesh->proj[c].x);
    cy = (mesh->proj[c].y);
    
    //draw a-b
    draw_line(ctx, ax, ay, bx, by);
    
    //draw a-c
    draw_line(ctx, ax, ay, cx, cy);
    
    //draw b-c
    draw_line(ctx, bx, by, cx, cy);
  }  
}

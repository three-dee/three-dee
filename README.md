three-dee
=========

A soft 3D engine for the Pebble(R) smart watch using the SDK 2.0. Thanks to the babylon.js team for their inspiring babylon.math.js file which helped me a lot.

Usage
-----

Copy the t'hree_dee.h' file in your src folder and include it:

  <three_dee.h>

Then you create and initialize the camera, the 3d objects and matrixes you need. One can find an example in the 'src/three_dee.c' file.

Features
--------

 * Rendering of 3D meshes in wireframe mode (including position, rotation, vertices, triangles)
 * Camera and perspective (LOV)
 * View, world and projection matrixes at your finger tips
 * Fast math operations for cos, sin, sqrt
 * Fast draw_line method

Limitations and known Bugs
--------------------------
 
 * Sometimes freezes the Pebble smartwatch (try to avoid frame rates over 30)
 * Can lead to a complete lock of the keys (happened once) - for recovering your watch initiate a firmware update from the pebble app on your smart phone.

License
-------

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If you like it, feel free to use it in your projects (see header of the 'three_dee.h' file for additional info).



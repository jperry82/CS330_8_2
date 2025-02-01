## **Brick Collision Simulation**
### **Overview**
This project is a **simple OpenGL-based 2D simulation** that demonstrates **brick collision mechanics** using **GLFW** for rendering. The program features **circles (moving objects) and bricks (static objects)** where:
- **Reflective bricks** bounce the circles in random directions.
- **Destructible bricks** gradually fade after multiple hits before disappearing.

---

## **Features**
- **Basic physics simulation**
  - Circles move in random directions and change color on impact.
  - Bricks react based on their **type (reflective/destructible)**.
- **Collision detection**
  - **Circle-to-brick detection** using **bounding box checks**.
  - **Circle-to-circle merging** when two circles collide.
- **Dynamic rendering**
  - Objects are drawn using **OpenGL polygons**.
  - **Real-time color updates** based on interaction.

---

## **How It Works**
1. **Launching the Program**
   - The program initializes a **480x480 pixel GLFW window**.
   - OpenGL sets up a **2D environment** for drawing.
   
2. **User Input**
   - `SPACEBAR` → Releases a new circle into the scene.
   - `ESCAPE` → Closes the application.

3. **Physics Mechanics**
   - Circles move in **randomized directions**.
   - When a circle **hits a brick**, its behavior changes:
     - **Reflective bricks**: Change the circle’s direction.
     - **Destructible bricks**: Change color and disappear after 10 hits.
   - Circles **merge** when they collide.

---

## **Code Structure**
### **Key Components**
| File | Description |
|------|-------------|
| `MainCode.cpp` | Core logic for simulation (movement, collision, rendering). |
| `GLFW/glfw3.h` | Library for handling OpenGL windows and input. |
| `linmath.h` | Lightweight matrix math operations for transformations. |

### **Main Classes**
#### **1️⃣ Brick Class (`Brick`)**
- Represents a **static rectangular block**.
- **Properties:**
  - `BrickType` (reflective or destructible)
  - `hit_count` (tracks hits)
  - `status` (active or removed)
- **Key Functions:**
  - `drawBrick()` → Renders the brick with its current color.
  - `isHit()` → Determines if a circle collided with the brick.

#### **2️⃣ Circle Class (`Circle`)**
- Represents a **moving object** that interacts with bricks.
- **Properties:**
  - `x, y, radius` → Position and size
  - `speed` → Movement speed
  - `direction` → Movement direction (1-8)
  - `red, green, blue` → Color properties
- **Key Functions:**
  - `MoveOneStep()` → Moves the circle based on direction.
  - `CheckCollision(Brick& brk)` → Handles circle-brick interaction.
  - `isColliding(Circle& other)` → Detects circle-to-circle collisions.
  - `mergeWith(Circle& other)` → Merges two circles into one.

---

## **Dependencies**
To run this project, you need:
- **C++ Compiler** (GCC, Clang, or MSVC)
- **GLFW Library**
  - Install on Windows:  
    ```sh
    vcpkg install glfw3
    ```
  - Install on Linux:  
    ```sh
    sudo apt-get install libglfw3-dev
    ```

---

## **How to Compile & Run**
### **🔧 Compilation**
1. Open a terminal and navigate to the project folder:
   ```sh
   cd path/to/project
   ```
2. Compile the program using g++:
   ```sh
   g++ -o BrickCollision MainCode.cpp -lglfw -lGL
   ```

### **▶️ Run the Program**
```sh
./BrickCollision
```

---

## **Known Issues & Limitations**
❌ **Inefficient collision detection**  
   - Uses **O(n²) complexity**, making it slow for large numbers of circles.  
❌ **Basic shading model**  
   - Uses **flat shading**, lacks smooth transitions.  
❌ **No advanced physics**  
   - No gravity, friction, or realistic bounces.  

---

## **Potential Enhancements**
🔹 **Optimization of movement and collision detection**  
🔹 **Implementation of spatial partitioning for efficiency**  
🔹 **Integration of Phong shading for better lighting effects**  
🔹 **Refactoring rendering code for improved modularity**  

---

## **License**
This project is **open-source** under the **MIT License**. Feel free to modify and improve it!

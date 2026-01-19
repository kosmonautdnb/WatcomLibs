/*******************************************************************************************
*
*   Physac - Physics demo
*
*   NOTE 1: Physac requires multi-threading, when InitPhysics() a second thread 
*           is created to manage physics calculations.
*   NOTE 2: Physac requires static C library linkage to avoid dependency 
*           on MinGW DLL (-static -lpthread)
*
*   Compile this program using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -s ..\icon\physac_icon -I. -I../src 
*           -I../src/external/raylib/src -static -lraylib -lopengl32 -lgdi32 -pthread -std=c99
*   
*   Copyright (c) 2016-2025 Victor Fisac (github: @victorfisac)
*
********************************************************************************************/
//----
/* MIT License

Copyright (c) 2022 Victor Fisac

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
//----
// MIT license from the Physac github repository
// modified on 19.01.2026 by Stefan Mader to compile with WatcomC 11.0/WatcomGL
//----

#include "GL.H"

#define PHYSAC_IMPLEMENTATION
#include "PHYSAC.HPP"

#include <stdio.h>

double mouseX = 400;
double mouseY = 300;

Vector2 GetMousePosition() {
  return Vector2::with(mouseX,mouseY);
}

double GetRandomValue(double low, double high) {
  return (double)rand()*(high-low)/(double)RAND_MAX+low;
}

int main()
{
  // Initialization
  //--------------------------------------------------------------------------------------
  int screenWidth = 800;
  int screenHeight = 600;

  printf("Physac library reduced for WatcomC++ 11.0.\n");
  printf("-------\n");
  printf("Left mouse button to create a polygon.\n");
  printf("Right mouse button to create a circle.\n");
  printf("-------\n");

  glWatcomPrecisionTimer(GL_TRUE); // better to have a more precise timer here in WatcomC
  
  if (!glVesa(screenWidth,screenHeight,32)) glVGA();
  
  // Initialize physics and default physics bodies
  InitPhysics();
  
  // Create floor rectangle physics body
  PhysicsBody floor = CreatePhysicsBodyRectangle(Vector2::with( screenWidth/2, screenHeight ), 500, 100, 10);
  floor->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
  
  // Create obstacle circle physics body
  PhysicsBody circle = CreatePhysicsBodyCircle(Vector2::with( screenWidth/2, screenHeight/2 ), 45, 10);
  circle->enabled = false; // Disable body state to convert it to static (no dynamics, but collisions)
  
  //SetTargetFPS(60);
  //--------------------------------------------------------------------------------------
  
  // Main game loop
  bool quit = false;
  while (!quit)    // Detect window close button or ESC key
  {
    if (glNextKey() == GL_VK_ESCAPE) quit = true;
    double mdx,mdy;
    glNextMouseDelta(&mdx,&mdy);
    mouseX += mdx;
    mouseY += mdy;
    if (mouseX < 0) mouseX = 0;
    if (mouseY < 0) mouseY = 0;
    if (mouseX > 799) mouseX = 799;
    if (mouseY > 599) mouseY = 599;
    
    // Update
    //----------------------------------------------------------------------------------
    // Physics body creation inputs
    if (glMouseButtons() & GL_MOUSE_BUTTON_LEFT)
      CreatePhysicsBodyPolygon(GetMousePosition(), GetRandomValue(20, 80), GetRandomValue(3, 8), 10);
    else if (glMouseButtons() & GL_MOUSE_BUTTON_RIGHT)
      CreatePhysicsBodyCircle(GetMousePosition(), GetRandomValue(10, 45), 10);
    
    // Destroy falling physics bodies
    int bodiesCount = GetPhysicsBodiesCount();
    for (int i = bodiesCount - 1; i >= 0; i--)
    {
      PhysicsBody body = GetPhysicsBody(i);
      
      if ((body != NULL) && (body->position.y > screenHeight*2))
        DestroyPhysicsBody(body);
    }
    //----------------------------------------------------------------------------------
    
    // Draw
    //----------------------------------------------------------------------------------
    //BeginDrawing();
    
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0,800,600,0,-1,1);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      
      //ClearBackground(BLACK);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT);
      
      //DrawFPS(screenWidth - 90, screenHeight - 30);
      
      glBegin(GL_LINES);
      glColor4f(1,1,1,1);
      glVertex2f(-4+mouseX,mouseY);glVertex2f(+3+mouseX,mouseY);
      glVertex2f(mouseX,-4+mouseY);glVertex2f(mouseX,+3+mouseY);

      glColor4f(0,1,0,1);
      // Draw created physics bodies
      bodiesCount = GetPhysicsBodiesCount();
      {for (int i = 0; i < bodiesCount; i++)
      {
        PhysicsBody body = GetPhysicsBody(i);
        
        if (body != NULL)
        {
          int vertexCount = GetPhysicsShapeVerticesCount(i);
          for (int j = 0; j < vertexCount; j++)
          {
            // Get physics bodies shape vertices to draw lines
            // Note: GetPhysicsShapeVertex() already calculates rotation transformations
            Vector2 vertexA = GetPhysicsShapeVertex(body, j);
            
            int jj = (((j + 1) < vertexCount) ? (j + 1) : 0);   // Get next vertex or first to close the shape
            Vector2 vertexB = GetPhysicsShapeVertex(body, jj);
            
            //DrawLineV(vertexA, vertexB, GREEN);     // Draw a line between two vertex positions
            glVertex2f(vertexA.x,vertexA.y);
            glVertex2f(vertexB.x,vertexB.y);
          }
        }
      }}
      glEnd();
      
      //DrawText("Left mouse button to create a polygon", 10, 10, 10, WHITE);
      //DrawText("Right mouse button to create a circle", 10, 25, 10, WHITE);
      
      //DrawText("Physac", logoX, logoY, 30, WHITE);
      //DrawText("Powered by", logoX + 50, logoY - 7, 10, WHITE);
    
    //EndDrawing();
    RunPhysicsStep(); // actually the author advised against calling this per frame/Vsync however we don't have multicore/multithreading
    glRefresh();
    //----------------------------------------------------------------------------------
  }
  
  // De-Initialization
  //--------------------------------------------------------------------------------------   
  ClosePhysics();       // Unitialize physics
  
  //CloseWindow();        // Close window and OpenGL context
  glDone();
  //--------------------------------------------------------------------------------------
  
  return 0;
}
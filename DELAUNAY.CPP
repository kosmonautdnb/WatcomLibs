#include "DELAUNAY.HPP"
#include "DELAUNTR.HPP"
#include "OBJECT.CPP"
#include "STRING.CPP"

#include "glimpl.cpp"
#include <stdio.h>

double mouseX = 320, mouseY = 240;

Array<double> points;
Array<size_t> triangles;

int main(int argc, const char **argv) {
  glVesa(640,480,32);
  while(1) {
    if (glNextKey()!=0) break;
    double mdx, mdy;
    glNextMouseDelta(&mdx, &mdy);
    static int la = glMouseButtons();
    int a = glMouseButtons();
    if (a == 0 && la != 0) {
      points.push_back(mouseX);
      points.push_back(mouseY);
      Delaunator delau(points);
      triangles = delau.triangles;
    }
    la = a;
    mouseX += mdx;
    mouseY += mdy;
    if (mouseX < 0) mouseX = 0;
    if (mouseY < 0) mouseY = 0;
    if (mouseX > 640) mouseX = 640;
    if (mouseY > 480) mouseY = 480;
    glLoadIdentity();
    glOrtho(0,640,480,0,-1,1);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);
    glColor4f(1,1,1,1);
    glVertex2f(mouseX, mouseY);
    {for (int i = 0; i < points.size(); i++) {
      glVertex2f(points[i*2+0],points[i*2+1]);
    }}
    glEnd();
    glBegin(GL_LINES);
    {for (int i = 0; i < triangles.size()/3; i++) {
      for (int k = 0; k < 3; k++) {
        glVertex2f(points[triangles[i*3+k]*2+0],points[triangles[i*3+k]*2+1]);
        glVertex2f(points[triangles[i*3+((k+1)%3)]*2+0],points[triangles[i*3+((k+1)%3)]*2+1]);
      }
    }}
    glEnd();
    glRefresh();
  }

  glDone();
  return 0;
}

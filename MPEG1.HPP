#ifndef __MPEG1_HPP__
#define __MPEG1_HPP__

unsigned int mpeg1_playMovie(const char *movieName); // returns opengl texture
bool mpeg1_update(); // if finished delete the texture on your own
void mpeg1_display(unsigned int texture);

#endif // __MPEG1_HPP__

#pragma once
#include <string>
#include "../support/vec3.h"

class Sprite {
private:
  int gfx_handle;
  int width;
  int height;
  std::string file_name;

public:
  Sprite() {}
  ~Sprite();
  void load( std::string _file_name );
  void render( t2k::vec3 _position );

};

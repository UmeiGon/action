#include "DxLib.h"
#include "sprite.h"

Sprite::~Sprite() {
  DeleteGraph( gfx_handle );
}

void Sprite::load( std::string _file_name ) {

  file_name = _file_name ;
  gfx_handle = LoadGraph( file_name.c_str(), false ) ;
  GetGraphSize( gfx_handle, &width, &height );

}

void Sprite::render( t2k::vec3 _position ) {

  DrawGraph(_position.x, _position.y, gfx_handle, true);

}


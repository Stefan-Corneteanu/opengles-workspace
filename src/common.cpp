#include <common.hpp>

namespace opengles_workspace{

    bool Pos::operator==(Pos& other){
        return this->x == other.x && this->y == other.y;
    }

    bool Pos::operator!=(Pos& other){
        return this->x != other.x || this->y != other.y;
    }
}
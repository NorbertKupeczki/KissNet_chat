#include "MyPacket.hpp"

MyPacket::MyPacket(size_t capacity)
{
    _data.reserve(capacity);
}

bool MyPacket::isEnd() const
{
    return read_pos >= _data.size();
}

const MyPacket::bytes &MyPacket::data() const
{
    return _data;
}

MyPacket::bytes &MyPacket::data()
{
    return _data;
}

size_t MyPacket::length() const
{
    return _data.size();
}

void MyPacket::reset_read_head()
{
    read_pos = 0;
}

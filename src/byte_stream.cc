#include "byte_stream.hh"

#include <algorithm>
#include <deque>
#include <string>
using namespace std;

ByteStream::ByteStream(const size_t capa){
  capacity = capa;
  written = 0;
  input_flag = false;
  _error = false;
}

size_t ByteStream::write(const string &data) {
  if(input_ended() || _error){
    return 0;
  }
  size_t bytesWritten = 0;
  for(char byte : data){
    if(buffer_size() < capacity){
      buffer.push_back(byte);
      bytesWritten++;
    }
    else{
      break;
    }
  }
  written = written + bytesWritten;
  return bytesWritten;
}

string ByteStream::peek_output(const size_t len) const {
  string output = "";
  if(_error){
    return output;
  }
  size_t len_effective = min(len, buffer_size());
  for(int i=0; i<len_effective; i++){
    output = output + buffer[i];
  }
  return output;
}

void ByteStream::pop_output(const size_t len) {
  if(len > buffer_size()){
    set_error();
    return;
  }
  if(_error){
    return;
  }
  size_t len_effective = min(len, buffer_size());
  for(int i=0; i<len_effective; i++){
    buffer.pop_front();
  }
}

std::string ByteStream::read(const size_t len) {
  if(len > buffer_size()){
    set_error();
    return "";
  }
  if(_error){
    return "";
  }
  string output = peek_output(len);
  pop_output(len);
  return output;
}

void ByteStream::end_input() { 
  input_flag = true;
}

bool ByteStream::input_ended() const {
  return input_flag;
}

size_t ByteStream::buffer_size() const {
  return buffer.size();
}

bool ByteStream::buffer_empty() const {
  if(buffer_size() == 0){
    return true;
  }
  return false;
}

bool ByteStream::eof() const {
  if(input_ended() && buffer_empty()){
    return true;
  }
  return false;
}

size_t ByteStream::bytes_written() const {
  return written;
}

size_t ByteStream::bytes_read() const {
  return written - buffer_size();
}

size_t ByteStream::remaining_capacity() const {
  return capacity - buffer_size();
}

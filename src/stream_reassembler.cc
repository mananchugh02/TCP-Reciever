#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity)
{
    capacity_max = capacity;
    next_index = 0; 
    unassembled = 0;
    eof_index = -1;
}

void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if(eof){
        eof_index = index + data.size();
    }
    if(index < next_index && index + data.size() > next_index){
        for(size_t i = next_index-index-1; i < data.size(); i++){
            if(_output.remaining_capacity() > 0 && buffer.find(index+i) == buffer.end() && index+i <= capacity_max){
                buffer[index+i] = data[i];
                unassembled++;
            }
        }
    }
    else if(index > next_index){
        size_t cap = _output.remaining_capacity();
        if(data.size() < cap){
            for(size_t i = 0; i < data.size(); i++){
                if(buffer.find(index + i) == buffer.end() && index+i <= capacity_max){
                    buffer[index+i] = data[i];
                    unassembled++;
                }
            }
        }
        else{
            for(size_t i = 0; i < cap; i++){
                if(buffer.find(index + i) == buffer.end() && index+i <= capacity_max){
                    buffer[index+i] = data[i];
                    unassembled++;
                }
            } 
        }
    }
    else if(next_index == index){
        size_t cap = _output.remaining_capacity();
        if(data.size() < cap){
            for(size_t i = 0; i < data.size(); i++){
                string temp = "";
                temp = temp + data[i];
                _output.write(temp);
                next_index++;
                if (buffer.find(index + i) != buffer.end()) {
                    auto it = buffer.find(index + i);
                    buffer.erase(it);
                    unassembled--;
                }
            }
        }
        else{
            for(size_t i = 0; i < cap; i++){
                string temp = "";
                temp = temp + data[i];
                _output.write(temp);
                next_index++;
                if(buffer.find(index + i) != buffer.end()){
                    auto it = buffer.find(index + i);
                    buffer.erase(it);
                    unassembled--;
                }
            }
        }
    }
    while(buffer.find(next_index) != buffer.end()) {
        char c = buffer[next_index];
        auto it = buffer.find(next_index);
        buffer.erase(it);
        unassembled--;
        string temp = "";
        temp += c;
        _output.write(temp);
        next_index++;
    }
    if(next_index == eof_index){
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return unassembled;
}

bool StreamReassembler::empty() const {
    if(buffer.size() == 0){
        return true;
    }
    return false;
}

size_t StreamReassembler::ack_index() const {
    return next_index;
}
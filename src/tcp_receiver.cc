#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    bool syn = head.syn;
    bool fin = head.fin;
    // ...
    if(syn == false){
        if(_synReceived == false){
            return;
        }
    }
    else if(syn == true){
        if(_synReceived == false){
            _synReceived = true;
            _isn = head.seqno;
        }
        return;
    }
    // --- Hint ------
        // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    // --- Hint ------  

    string data = seg.payload().copy();
    _reassembler.push_substring(data, stream_idx, fin);
    _finReceived = (fin || _finReceived);

    // ... 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_synReceived){
        if (_finReceived && _reassembler.unassembled_bytes() == 0) {
            return wrap(_reassembler.stream_out().bytes_written() + 2, _isn);
        }
        return wrap(_reassembler.stream_out().bytes_written() + 1, _isn);
    }
    return nullopt;
}


size_t TCPReceiver::window_size() const { 
    return _capacity - _reassembler.stream_out().buffer_size(); 
}

#include "frame.hh"

Frame &active_frame() {
    static Frame frame;
    return frame;
}

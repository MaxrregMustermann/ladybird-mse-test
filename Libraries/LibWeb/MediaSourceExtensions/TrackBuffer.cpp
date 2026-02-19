#include <LibWeb/MediaSourceExtensions/TrackBuffer.h>

namespace Web::MediaSourceExtensions {

void TrackBuffer::insert_coded_frame(Frame)
{
    // FIXME: Implement
}

void TrackBuffer::remove_frames_in_range(double, double)
{
    // FIXME: Implement
}

TrackBuffer::Frame const* TrackBuffer::get_frame_at(double)
{
    // FIXME: Implement
    return nullptr;
}

JS::NonnullGCPtr<HTML::TimeRanges> TrackBuffer::compute_buffered_ranges(JS::Realm&) const
{
    // FIXME: Implement
    return HTML::TimeRanges::create(realm);
}

void TrackBuffer::reset_parser_state()
{
    // FIXME: Implement
}

void TrackBuffer::evict_coded_frames(double, size_t)
{
    // FIXME: Implement
}

}

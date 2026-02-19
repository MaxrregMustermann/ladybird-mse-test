#include <LibWeb/MediaSourceExtensions/CodedFrameProcessor.h>
#include <LibWeb/MediaSourceExtensions/SourceBuffer.h>
#include <LibWeb/MediaSourceExtensions/TrackBuffer.h>

namespace Web::MediaSourceExtensions {

CodedFrameProcessor::CodedFrameProcessor(SourceBuffer& source_buffer)
    : m_source_buffer(source_buffer)
{
}

void CodedFrameProcessor::process(TrackBuffer& track_buffer, SegmentParser::CodedFrame const& frame)
{
    // This is an implementation of the "Coded Frame Processing" algorithm from the W3C MSE spec.
    // https://www.w3.org/TR/media-source/#sourcebuffer-coded-frame-processing

    // 1. Let presentation timestamp be the result of applying the timestamp offset to the frame's presentation timestamp.
    double presentation_timestamp = frame.presentation_timestamp + m_source_buffer.timestamp_offset();

    // 2. Let decode timestamp be the result of applying the timestamp offset to the frame's decode timestamp.
    double decode_timestamp = frame.decode_timestamp + m_source_buffer.timestamp_offset();

    // 3. If the presentation timestamp or the decode timestamp is less than the append window start,
    //    then silently drop the frame and abort these steps.
    if (presentation_timestamp < m_source_buffer.append_window_start() || decode_timestamp < m_source_buffer.append_window_start()) {
        m_need_random_access_point = true;
        return;
    }

    // 4. If the presentation timestamp is greater than or equal to the append window end, then
    //    silently drop the frame and abort these steps.
    if (presentation_timestamp >= m_source_buffer.append_window_end()) {
        return;
    }

    // 5-7. Random access point handling.
    if (m_need_random_access_point) {
        if (!frame.is_key_frame)
            return;
        m_need_random_access_point = false;
    }

    double frame_end_timestamp = presentation_timestamp + frame.duration;

    // 8. Remove track buffer ranges.
    track_buffer.remove_frames_in_range(presentation_timestamp, frame_end_timestamp);

    // 9. Add the coded frame to the track buffer.
    TrackBuffer::Frame new_frame;
    new_frame.pts = presentation_timestamp;
    new_frame.dts = decode_timestamp;
    new_frame.duration = frame.duration;
    new_frame.is_keyframe = frame.is_key_frame;
    new_frame.encoded_data = frame.data;
    track_buffer.insert_coded_frame(move(new_frame));

    // FIXME: Implement remaining steps (10-12) of the algorithm,
    // which involve updating MediaSource duration and handling buffer full states.
}

}

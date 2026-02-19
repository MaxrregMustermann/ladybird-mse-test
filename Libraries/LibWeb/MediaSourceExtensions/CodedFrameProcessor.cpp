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
    //    then run the end of stream algorithm with the error parameter set to "decode", and abort these steps.
    if (presentation_timestamp < m_source_buffer.append_window_start() || decode_timestamp < m_source_buffer.append_window_start()) {
        // FIXME: Run end of stream algorithm with "decode" error.
        return;
    }

    // 4. If the presentation timestamp is greater than or equal to the append window end, then run the
    //    end of stream algorithm with the error parameter set to "decode", and abort these steps.
    if (presentation_timestamp >= m_source_buffer.append_window_end()) {
        // FIXME: Run end of stream algorithm with "decode" error.
        return;
    }

    // FIXME: 5. Let frame end timestamp equal the sum of the presentation timestamp and the frame's duration.

    // FIXME: 6. If mode equals "sequence" and the group start timestamp is set, then set the group end timestamp
    //           to the frame end timestamp.

    // FIXME: 7. If the need random access point flag is true, then
        // FIXME: 7.1. If the frame is not a random access point, then drop the frame and abort these steps.
        // FIXME: 7.2. Set the need random access point flag to false.

    // FIXME: 8. Remove track buffer ranges.

    // FIXME: 9. Add the coded frame to the track buffer.

    // FIXME: 10. If the frame is the last frame in a media segment, ...

    // FIXME: 11. Update the highest presentation timestamp.

    // FIXME: 12. If the track buffer is full, ...
}

}

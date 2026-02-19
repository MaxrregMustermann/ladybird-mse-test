#pragma once
#include <AK/ByteBuffer.h>
#include <AK/Vector.h>
#include <LibGfx/Bitmap.h>
#include <LibWeb/HTML/TimeRanges.h>

namespace Web::MediaSourceExtensions {

class TrackBuffer {
public:
    struct Frame {
        double pts;       // presentation timestamp (seconds)
        double dts;       // decode timestamp (seconds)
        double duration;
        bool   is_keyframe;
        ByteBuffer encoded_data; // still-encoded frame bytes
        // OR, after decode:
        RefPtr<Gfx::Bitmap> decoded_video;
        Vector<float> decoded_audio_samples;
    };

    void insert_coded_frame(Frame);
    void remove_frames_in_range(double start, double end);

    Frame const* get_frame_at(double presentation_time);
    JS::NonnullGCPtr<HTML::TimeRanges> compute_buffered_ranges(JS::Realm&) const;

    // Called when SourceBuffer.abort() is invoked
    void reset_parser_state();

    // Buffer eviction: remove already-played frames and frames >30s ahead
    void evict_coded_frames(double current_playback_time, size_t needed_bytes);

private:
    // Sorted by PTS
    Vector<Frame> m_frames;
    // Random access point index (keyframes) for seeking
    Vector<size_t> m_keyframe_indices;
};

}

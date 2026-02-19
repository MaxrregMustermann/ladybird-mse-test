#pragma once
#include <AK/ByteBuffer.h>
#include <AK/Error.h>
#include <AK/OwnPtr.h>
#include <AK/String.h>
#include <AK/Vector.h>

namespace Web::MediaSourceExtensions {

class SegmentParser {
public:
    struct TrackInfo {
        u32 track_id;
        bool is_video;
        bool is_audio;
        String codec_string; // e.g. "avc1.64001f"
        u32 width, height;         // video
        u32 sample_rate, channels; // audio
        ByteBuffer codec_private_data; // SPS/PPS for h264, etc.
    };

    struct CodedFrame {
        u32 track_id;
        bool is_key_frame;
        double presentation_timestamp;  // PTS in seconds
        double decode_timestamp;        // DTS in seconds
        double duration;
        ByteBuffer data;
    };

    struct ParsedSegment {
        bool is_initialization_segment { false };
        Vector<TrackInfo> tracks;       // populated for init segments
        Vector<CodedFrame> coded_frames; // populated for media segments
    };

    virtual ~SegmentParser() = default;

    // Feed raw bytes; may be called multiple times (streaming input).
    // Returns parsed results when enough data is available.
    virtual ErrorOr<Vector<ParsedSegment>> append(ReadonlyBytes) = 0;
    virtual void reset() = 0;

    static OwnPtr<SegmentParser> create_for_mime_type(StringView mime_type);
};

} // namespace Web::MediaSourceExtensions

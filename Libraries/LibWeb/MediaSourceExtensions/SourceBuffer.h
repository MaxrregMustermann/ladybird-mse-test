#pragma once
#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/MediaSourceExtensions/TrackBuffer.h>
#include <LibWeb/MediaSourceExtensions/SegmentParser.h>

namespace Web::MediaSourceExtensions {

class SourceBuffer final : public DOM::EventTarget {
    WEB_PLATFORM_OBJECT(SourceBuffer, DOM::EventTarget);

public:
    // Core state
    bool updating() const { return m_updating; }
    GC::Ref<HTML::TimeRanges> buffered() const;

    Bindings::AppendMode mode() const { return m_mode; }
    WebIDL::ExceptionOr<void> set_mode(Bindings::AppendMode);

    double timestamp_offset() const { return m_timestamp_offset; }
    WebIDL::ExceptionOr<void> set_timestamp_offset(double);

    double append_window_start() const { return m_append_window_start; }
    double append_window_end() const { return m_append_window_end; }
    WebIDL::ExceptionOr<void> set_append_window_start(double);
    WebIDL::ExceptionOr<void> set_append_window_end(double);

    // JS-exposed methods
    WebIDL::ExceptionOr<void> append_buffer(GC::Root<WebIDL::BufferSource>);
    WebIDL::ExceptionOr<void> abort();
    WebIDL::ExceptionOr<void> remove(double start, double end);
    WebIDL::ExceptionOr<void> change_type(String const& type);

    // Internal: called by the decode pipeline
    Optional<TrackBuffer::Frame> get_next_video_frame(double current_time);
    Optional<TrackBuffer::AudioSamples> get_next_audio_samples(double current_time);

private:
    SourceBuffer(JS::Realm&, MediaSource&, String mime_type);

    MediaSource& m_parent_media_source;
    String m_mime_type;
    Bindings::AppendMode m_mode { Bindings::AppendMode::Segments };

    bool m_updating { false };
    double m_timestamp_offset { 0.0 };
    double m_append_window_start { 0.0 };
    double m_append_window_end { INFINITY };

    // The heart of the SourceBuffer
    OwnPtr<SegmentParser> m_segment_parser;
    OwnPtr<TrackBuffer> m_video_track_buffer;
    OwnPtr<TrackBuffer> m_audio_track_buffer;

    // Internal algorithm runners
    void run_append_buffer_algorithm(ByteBuffer data);
    void run_coded_frame_processing_algorithm(SegmentParser::ParsedSegment const&);
    void run_buffer_eviction_algorithm();

    void set_updating(bool);
    void queue_task_to_fire_event(FlyString const& event_name);
};

} // namespace Web::MediaSourceExtensions

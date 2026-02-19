#pragma once
#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/MediaSourceExtensions/SourceBufferList.h>

namespace Web::MediaSourceExtensions {

class MediaSource final : public DOM::EventTarget {
    WEB_PLATFORM_OBJECT(MediaSource, DOM::EventTarget);

public:
    static WebIDL::ExceptionOr<GC::Ref<MediaSource>> construct_impl(JS::Realm&);

    // readyState
    enum class ReadyState { Closed, Open, Ended };
    ReadyState ready_state() const { return m_ready_state; }

    // sourceBuffers / activeSourceBuffers
    GC::Ref<SourceBufferList> source_buffers();
    GC::Ref<SourceBufferList> active_source_buffers();

    // duration
    WebIDL::ExceptionOr<void> set_duration(double);
    double duration() const;

    // JS-exposed methods
    WebIDL::ExceptionOr<GC::Ref<SourceBuffer>> add_source_buffer(String const& type);
    WebIDL::ExceptionOr<void> remove_source_buffer(GC::Ref<SourceBuffer>);
    WebIDL::ExceptionOr<void> end_of_stream(Optional<Bindings::EndOfStreamError>);

    static bool is_type_supported(JS::VM&, String const& type);

    // Internal: called by HTMLMediaElement when src is a MediaSource blob URL
    void attach_to_media_element(HTML::HTMLMediaElement&);
    void detach_from_media_element();

    // Internal: called by SourceBuffer when it has data ready
    void notify_source_buffer_updated(SourceBuffer&);

private:
    MediaSource(JS::Realm&);

    ReadyState m_ready_state { ReadyState::Closed };
    GC::Ref<SourceBufferList> m_source_buffers;
    GC::Ref<SourceBufferList> m_active_source_buffers;
    GC::Ptr<HTML::HTMLMediaElement> m_attached_element;
    double m_duration { NAN };

    void transition_to_open();
    void transition_to_ended();
    void transition_to_closed();
};

} // namespace Web::MediaSourceExtensions

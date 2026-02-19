#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Bindings/MediaSourcePrototype.h>
#include <LibWeb/MediaSourceExtensions/MediaSource.h>
#include <LibWeb/MediaSourceExtensions/SourceBufferList.h>

namespace Web::MediaSourceExtensions {

JS_DEFINE_ALLOCATOR(MediaSource);

WebIDL::ExceptionOr<JS::NonnullGCPtr<MediaSource>> MediaSource::construct_impl(JS::Realm& realm)
{
    auto source_buffers = TRY(SourceBufferList::construct_impl(realm));
    auto active_source_buffers = TRY(SourceBufferList::construct_impl(realm));
    return realm.heap().allocate<MediaSource>(realm, realm, source_buffers, active_source_buffers);
}

MediaSource::MediaSource(JS::Realm& realm, JS::NonnullGCPtr<SourceBufferList> source_buffers, JS::NonnullGCPtr<SourceBufferList> active_source_buffers)
    : DOM::EventTarget(realm)
    , m_source_buffers(source_buffers)
    , m_active_source_buffers(active_source_buffers)
{
}

MediaSource::~MediaSource() = default;

void MediaSource::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    set_prototype(&ensure_web_prototype<Bindings::MediaSourcePrototype>(realm, "MediaSource"));
}

void MediaSource::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_source_buffers);
    visitor.visit(m_active_source_buffers);
    visitor.visit(m_attached_element);
}

} // namespace Web::MediaSourceExtensions

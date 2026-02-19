#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Bindings/SourceBufferPrototype.h>
#include <LibWeb/MediaSourceExtensions/SourceBuffer.h>

namespace Web::MediaSourceExtensions {

JS_DEFINE_ALLOCATOR(SourceBuffer);

WebIDL::ExceptionOr<JS::NonnullGCPtr<SourceBuffer>> SourceBuffer::construct_impl(JS::Realm& realm, MediaSource& media_source, String mime_type)
{
    return realm.heap().allocate<SourceBuffer>(realm, realm, media_source, move(mime_type));
}

SourceBuffer::SourceBuffer(JS::Realm& realm, MediaSource& media_source, String mime_type)
    : DOM::EventTarget(realm)
    , m_parent_media_source(media_source)
    , m_mime_type(move(mime_type))
{
}

SourceBuffer::~SourceBuffer() = default;

void SourceBuffer::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    set_prototype(&ensure_web_prototype<Bindings::SourceBufferPrototype>(realm, "SourceBuffer"));
}

void SourceBuffer::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_parent_media_source);
}

} // namespace Web::MediaSourceExtensions

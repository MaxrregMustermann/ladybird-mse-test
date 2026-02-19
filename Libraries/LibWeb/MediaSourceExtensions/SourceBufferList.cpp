#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Bindings/SourceBufferListPrototype.h>
#include <LibWeb/MediaSourceExtensions/SourceBufferList.h>

namespace Web::MediaSourceExtensions {

JS_DEFINE_ALLOCATOR(SourceBufferList);

WebIDL::ExceptionOr<JS::NonnullGCPtr<SourceBufferList>> SourceBufferList::construct_impl(JS::Realm& realm)
{
    return realm.heap().allocate<SourceBufferList>(realm, realm);
}

SourceBufferList::SourceBufferList(JS::Realm& realm)
    : DOM::EventTarget(realm)
{
}

SourceBufferList::~SourceBufferList() = default;

void SourceBufferList::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
    set_prototype(&ensure_web_prototype<Bindings::SourceBufferListPrototype>(realm, "SourceBufferList"));
}

void SourceBufferList::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    for (auto& buffer : m_source_buffers)
        visitor.visit(buffer);
}

}

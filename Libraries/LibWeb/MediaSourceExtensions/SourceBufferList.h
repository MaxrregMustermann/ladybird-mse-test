#pragma once

#include <LibWeb/DOM/EventTarget.h>
#include <LibWeb/Forward.h>

namespace Web::MediaSourceExtensions {

class SourceBufferList final : public DOM::EventTarget {
    WEB_PLATFORM_OBJECT(SourceBufferList, DOM::EventTarget);
    JS_DECLARE_ALLOCATOR(SourceBufferList);

public:
    static WebIDL::ExceptionOr<JS::NonnullGCPtr<SourceBufferList>> construct_impl(JS::Realm&);
    virtual ~SourceBufferList() override;

private:
    explicit SourceBufferList(JS::Realm&);

    virtual void initialize(JS::Realm&) override;
    virtual void visit_edges(Cell::Visitor&) override;

    Vector<JS::Handle<SourceBuffer>> m_source_buffers;
};

}

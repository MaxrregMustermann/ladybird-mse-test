#pragma once
#include <LibWeb/DOM/EventTarget.h>

namespace Web::MediaSourceExtensions {

class SourceBufferList final : public DOM::EventTarget {
    WEB_PLATFORM_OBJECT(SourceBufferList, DOM::EventTarget);
public:
    static WebIDL::ExceptionOr<GC::Ref<SourceBufferList>> construct_impl(JS::Realm&);
    ~SourceBufferList();

private:
    explicit SourceBufferList(JS::Realm&);
};

}

// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: xrpc.proto

#include "xrpc.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace top {
class xrpc_requestDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<xrpc_request> _instance;
} _xrpc_request_default_instance_;
class xrpc_replyDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<xrpc_reply> _instance;
} _xrpc_reply_default_instance_;
}  // namespace top
static void InitDefaultsscc_info_xrpc_reply_xrpc_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::top::_xrpc_reply_default_instance_;
    new (ptr) ::top::xrpc_reply();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::top::xrpc_reply::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_xrpc_reply_xrpc_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_xrpc_reply_xrpc_2eproto}, {}};

static void InitDefaultsscc_info_xrpc_request_xrpc_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::top::_xrpc_request_default_instance_;
    new (ptr) ::top::xrpc_request();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::top::xrpc_request::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_xrpc_request_xrpc_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_xrpc_request_xrpc_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_xrpc_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_xrpc_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_xrpc_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_xrpc_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::top::xrpc_request, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::top::xrpc_request, action_),
  PROTOBUF_FIELD_OFFSET(::top::xrpc_request, body_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::top::xrpc_reply, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::top::xrpc_reply, result_),
  PROTOBUF_FIELD_OFFSET(::top::xrpc_reply, body_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::top::xrpc_request)},
  { 7, -1, sizeof(::top::xrpc_reply)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::top::_xrpc_request_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::top::_xrpc_reply_default_instance_),
};

const char descriptor_table_protodef_xrpc_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\nxrpc.proto\022\003top\",\n\014xrpc_request\022\016\n\006act"
  "ion\030\001 \001(\t\022\014\n\004body\030\002 \001(\t\"*\n\nxrpc_reply\022\016\n"
  "\006result\030\001 \001(\t\022\014\n\004body\030\002 \001(\t2t\n\014xrpc_serv"
  "ice\022,\n\004call\022\021.top.xrpc_request\032\017.top.xrp"
  "c_reply\"\000\0226\n\014table_stream\022\021.top.xrpc_req"
  "uest\032\017.top.xrpc_reply\"\0000\001B6\n\033io.grpc.exa"
  "mples.helloworldB\017HelloWorldProtoP\001\242\002\003HL"
  "Wb\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_xrpc_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_xrpc_2eproto_sccs[2] = {
  &scc_info_xrpc_reply_xrpc_2eproto.base,
  &scc_info_xrpc_request_xrpc_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_xrpc_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_xrpc_2eproto = {
  false, false, descriptor_table_protodef_xrpc_2eproto, "xrpc.proto", 289,
  &descriptor_table_xrpc_2eproto_once, descriptor_table_xrpc_2eproto_sccs, descriptor_table_xrpc_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_xrpc_2eproto::offsets,
  file_level_metadata_xrpc_2eproto, 2, file_level_enum_descriptors_xrpc_2eproto, file_level_service_descriptors_xrpc_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_xrpc_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_xrpc_2eproto)), true);
namespace top {

// ===================================================================

void xrpc_request::InitAsDefaultInstance() {
}
class xrpc_request::_Internal {
 public:
};

xrpc_request::xrpc_request(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:top.xrpc_request)
}
xrpc_request::xrpc_request(const xrpc_request& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  action_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_action().empty()) {
    action_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_action(),
      GetArena());
  }
  body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_body().empty()) {
    body_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_body(),
      GetArena());
  }
  // @@protoc_insertion_point(copy_constructor:top.xrpc_request)
}

void xrpc_request::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_xrpc_request_xrpc_2eproto.base);
  action_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

xrpc_request::~xrpc_request() {
  // @@protoc_insertion_point(destructor:top.xrpc_request)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void xrpc_request::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  action_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  body_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void xrpc_request::ArenaDtor(void* object) {
  xrpc_request* _this = reinterpret_cast< xrpc_request* >(object);
  (void)_this;
}
void xrpc_request::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void xrpc_request::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const xrpc_request& xrpc_request::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_xrpc_request_xrpc_2eproto.base);
  return *internal_default_instance();
}


void xrpc_request::Clear() {
// @@protoc_insertion_point(message_clear_start:top.xrpc_request)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  action_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  body_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* xrpc_request::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string action = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_action();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "top.xrpc_request.action"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string body = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_body();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "top.xrpc_request.body"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* xrpc_request::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:top.xrpc_request)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string action = 1;
  if (this->action().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_action().data(), static_cast<int>(this->_internal_action().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "top.xrpc_request.action");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_action(), target);
  }

  // string body = 2;
  if (this->body().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_body().data(), static_cast<int>(this->_internal_body().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "top.xrpc_request.body");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_body(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:top.xrpc_request)
  return target;
}

size_t xrpc_request::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:top.xrpc_request)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string action = 1;
  if (this->action().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_action());
  }

  // string body = 2;
  if (this->body().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_body());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void xrpc_request::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:top.xrpc_request)
  GOOGLE_DCHECK_NE(&from, this);
  const xrpc_request* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<xrpc_request>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:top.xrpc_request)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:top.xrpc_request)
    MergeFrom(*source);
  }
}

void xrpc_request::MergeFrom(const xrpc_request& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:top.xrpc_request)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.action().size() > 0) {
    _internal_set_action(from._internal_action());
  }
  if (from.body().size() > 0) {
    _internal_set_body(from._internal_body());
  }
}

void xrpc_request::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:top.xrpc_request)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void xrpc_request::CopyFrom(const xrpc_request& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:top.xrpc_request)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool xrpc_request::IsInitialized() const {
  return true;
}

void xrpc_request::InternalSwap(xrpc_request* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  action_.Swap(&other->action_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  body_.Swap(&other->body_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

::PROTOBUF_NAMESPACE_ID::Metadata xrpc_request::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void xrpc_reply::InitAsDefaultInstance() {
}
class xrpc_reply::_Internal {
 public:
};

xrpc_reply::xrpc_reply(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:top.xrpc_reply)
}
xrpc_reply::xrpc_reply(const xrpc_reply& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  result_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_result().empty()) {
    result_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_result(),
      GetArena());
  }
  body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_body().empty()) {
    body_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_body(),
      GetArena());
  }
  // @@protoc_insertion_point(copy_constructor:top.xrpc_reply)
}

void xrpc_reply::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_xrpc_reply_xrpc_2eproto.base);
  result_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

xrpc_reply::~xrpc_reply() {
  // @@protoc_insertion_point(destructor:top.xrpc_reply)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void xrpc_reply::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  result_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  body_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void xrpc_reply::ArenaDtor(void* object) {
  xrpc_reply* _this = reinterpret_cast< xrpc_reply* >(object);
  (void)_this;
}
void xrpc_reply::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void xrpc_reply::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const xrpc_reply& xrpc_reply::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_xrpc_reply_xrpc_2eproto.base);
  return *internal_default_instance();
}


void xrpc_reply::Clear() {
// @@protoc_insertion_point(message_clear_start:top.xrpc_reply)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  result_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  body_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* xrpc_reply::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string result = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_result();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "top.xrpc_reply.result"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string body = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_body();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "top.xrpc_reply.body"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* xrpc_reply::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:top.xrpc_reply)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string result = 1;
  if (this->result().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_result().data(), static_cast<int>(this->_internal_result().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "top.xrpc_reply.result");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_result(), target);
  }

  // string body = 2;
  if (this->body().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_body().data(), static_cast<int>(this->_internal_body().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "top.xrpc_reply.body");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_body(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:top.xrpc_reply)
  return target;
}

size_t xrpc_reply::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:top.xrpc_reply)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string result = 1;
  if (this->result().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_result());
  }

  // string body = 2;
  if (this->body().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_body());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void xrpc_reply::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:top.xrpc_reply)
  GOOGLE_DCHECK_NE(&from, this);
  const xrpc_reply* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<xrpc_reply>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:top.xrpc_reply)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:top.xrpc_reply)
    MergeFrom(*source);
  }
}

void xrpc_reply::MergeFrom(const xrpc_reply& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:top.xrpc_reply)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.result().size() > 0) {
    _internal_set_result(from._internal_result());
  }
  if (from.body().size() > 0) {
    _internal_set_body(from._internal_body());
  }
}

void xrpc_reply::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:top.xrpc_reply)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void xrpc_reply::CopyFrom(const xrpc_reply& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:top.xrpc_reply)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool xrpc_reply::IsInitialized() const {
  return true;
}

void xrpc_reply::InternalSwap(xrpc_reply* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  result_.Swap(&other->result_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  body_.Swap(&other->body_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

::PROTOBUF_NAMESPACE_ID::Metadata xrpc_reply::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace top
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::top::xrpc_request* Arena::CreateMaybeMessage< ::top::xrpc_request >(Arena* arena) {
  return Arena::CreateMessageInternal< ::top::xrpc_request >(arena);
}
template<> PROTOBUF_NOINLINE ::top::xrpc_reply* Arena::CreateMaybeMessage< ::top::xrpc_reply >(Arena* arena) {
  return Arena::CreateMessageInternal< ::top::xrpc_reply >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>

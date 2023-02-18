// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto_parameters.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_proto_5fparameters_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_proto_5fparameters_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3012000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3012004 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "proto_basic.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_proto_5fparameters_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_proto_5fparameters_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_proto_5fparameters_2eproto;
namespace top {
namespace evm_engine {
namespace parameters {
class FunctionCallArgs;
class FunctionCallArgsDefaultTypeInternal;
extern FunctionCallArgsDefaultTypeInternal _FunctionCallArgs_default_instance_;
class SubmitResult;
class SubmitResultDefaultTypeInternal;
extern SubmitResultDefaultTypeInternal _SubmitResult_default_instance_;
}  // namespace parameters
}  // namespace evm_engine
}  // namespace top
PROTOBUF_NAMESPACE_OPEN
template<> ::top::evm_engine::parameters::FunctionCallArgs* Arena::CreateMaybeMessage<::top::evm_engine::parameters::FunctionCallArgs>(Arena*);
template<> ::top::evm_engine::parameters::SubmitResult* Arena::CreateMaybeMessage<::top::evm_engine::parameters::SubmitResult>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace top {
namespace evm_engine {
namespace parameters {

// ===================================================================

class SubmitResult PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:top.evm_engine.parameters.SubmitResult) */ {
 public:
  inline SubmitResult() : SubmitResult(nullptr) {};
  virtual ~SubmitResult();

  SubmitResult(const SubmitResult& from);
  SubmitResult(SubmitResult&& from) noexcept
    : SubmitResult() {
    *this = ::std::move(from);
  }

  inline SubmitResult& operator=(const SubmitResult& from) {
    CopyFrom(from);
    return *this;
  }
  inline SubmitResult& operator=(SubmitResult&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const SubmitResult& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SubmitResult* internal_default_instance() {
    return reinterpret_cast<const SubmitResult*>(
               &_SubmitResult_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(SubmitResult& a, SubmitResult& b) {
    a.Swap(&b);
  }
  inline void Swap(SubmitResult* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(SubmitResult* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline SubmitResult* New() const final {
    return CreateMaybeMessage<SubmitResult>(nullptr);
  }

  SubmitResult* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<SubmitResult>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const SubmitResult& from);
  void MergeFrom(const SubmitResult& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(SubmitResult* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "top.evm_engine.parameters.SubmitResult";
  }
  protected:
  explicit SubmitResult(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_proto_5fparameters_2eproto);
    return ::descriptor_table_proto_5fparameters_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kLogsFieldNumber = 5,
    kStatusDataFieldNumber = 3,
    kVersionFieldNumber = 1,
    kTransactionStatusFieldNumber = 2,
    kGasUsedFieldNumber = 4,
  };
  // repeated .top.evm_engine.basic.ResultLog logs = 5;
  int logs_size() const;
  private:
  int _internal_logs_size() const;
  public:
  void clear_logs();
  ::top::evm_engine::basic::ResultLog* mutable_logs(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::top::evm_engine::basic::ResultLog >*
      mutable_logs();
  private:
  const ::top::evm_engine::basic::ResultLog& _internal_logs(int index) const;
  ::top::evm_engine::basic::ResultLog* _internal_add_logs();
  public:
  const ::top::evm_engine::basic::ResultLog& logs(int index) const;
  ::top::evm_engine::basic::ResultLog* add_logs();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::top::evm_engine::basic::ResultLog >&
      logs() const;

  // bytes status_data = 3;
  void clear_status_data();
  const std::string& status_data() const;
  void set_status_data(const std::string& value);
  void set_status_data(std::string&& value);
  void set_status_data(const char* value);
  void set_status_data(const void* value, size_t size);
  std::string* mutable_status_data();
  std::string* release_status_data();
  void set_allocated_status_data(std::string* status_data);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_status_data();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_status_data(
      std::string* status_data);
  private:
  const std::string& _internal_status_data() const;
  void _internal_set_status_data(const std::string& value);
  std::string* _internal_mutable_status_data();
  public:

  // uint32 version = 1;
  void clear_version();
  ::PROTOBUF_NAMESPACE_ID::uint32 version() const;
  void set_version(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_version() const;
  void _internal_set_version(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 transaction_status = 2;
  void clear_transaction_status();
  ::PROTOBUF_NAMESPACE_ID::uint32 transaction_status() const;
  void set_transaction_status(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_transaction_status() const;
  void _internal_set_transaction_status(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint64 gas_used = 4;
  void clear_gas_used();
  ::PROTOBUF_NAMESPACE_ID::uint64 gas_used() const;
  void set_gas_used(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_gas_used() const;
  void _internal_set_gas_used(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // @@protoc_insertion_point(class_scope:top.evm_engine.parameters.SubmitResult)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::top::evm_engine::basic::ResultLog > logs_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr status_data_;
  ::PROTOBUF_NAMESPACE_ID::uint32 version_;
  ::PROTOBUF_NAMESPACE_ID::uint32 transaction_status_;
  ::PROTOBUF_NAMESPACE_ID::uint64 gas_used_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_proto_5fparameters_2eproto;
};
// -------------------------------------------------------------------

class FunctionCallArgs PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:top.evm_engine.parameters.FunctionCallArgs) */ {
 public:
  inline FunctionCallArgs() : FunctionCallArgs(nullptr) {};
  virtual ~FunctionCallArgs();

  FunctionCallArgs(const FunctionCallArgs& from);
  FunctionCallArgs(FunctionCallArgs&& from) noexcept
    : FunctionCallArgs() {
    *this = ::std::move(from);
  }

  inline FunctionCallArgs& operator=(const FunctionCallArgs& from) {
    CopyFrom(from);
    return *this;
  }
  inline FunctionCallArgs& operator=(FunctionCallArgs&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const FunctionCallArgs& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const FunctionCallArgs* internal_default_instance() {
    return reinterpret_cast<const FunctionCallArgs*>(
               &_FunctionCallArgs_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(FunctionCallArgs& a, FunctionCallArgs& b) {
    a.Swap(&b);
  }
  inline void Swap(FunctionCallArgs* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(FunctionCallArgs* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline FunctionCallArgs* New() const final {
    return CreateMaybeMessage<FunctionCallArgs>(nullptr);
  }

  FunctionCallArgs* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<FunctionCallArgs>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const FunctionCallArgs& from);
  void MergeFrom(const FunctionCallArgs& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(FunctionCallArgs* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "top.evm_engine.parameters.FunctionCallArgs";
  }
  protected:
  explicit FunctionCallArgs(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_proto_5fparameters_2eproto);
    return ::descriptor_table_proto_5fparameters_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kInputFieldNumber = 4,
    kAddressFieldNumber = 2,
    kValueFieldNumber = 3,
    kGasLimitFieldNumber = 5,
    kVersionFieldNumber = 1,
  };
  // bytes input = 4;
  void clear_input();
  const std::string& input() const;
  void set_input(const std::string& value);
  void set_input(std::string&& value);
  void set_input(const char* value);
  void set_input(const void* value, size_t size);
  std::string* mutable_input();
  std::string* release_input();
  void set_allocated_input(std::string* input);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_input();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_input(
      std::string* input);
  private:
  const std::string& _internal_input() const;
  void _internal_set_input(const std::string& value);
  std::string* _internal_mutable_input();
  public:

  // .top.evm_engine.basic.ProtoAddress address = 2;
  bool has_address() const;
  private:
  bool _internal_has_address() const;
  public:
  void clear_address();
  const ::top::evm_engine::basic::ProtoAddress& address() const;
  ::top::evm_engine::basic::ProtoAddress* release_address();
  ::top::evm_engine::basic::ProtoAddress* mutable_address();
  void set_allocated_address(::top::evm_engine::basic::ProtoAddress* address);
  private:
  const ::top::evm_engine::basic::ProtoAddress& _internal_address() const;
  ::top::evm_engine::basic::ProtoAddress* _internal_mutable_address();
  public:
  void unsafe_arena_set_allocated_address(
      ::top::evm_engine::basic::ProtoAddress* address);
  ::top::evm_engine::basic::ProtoAddress* unsafe_arena_release_address();

  // .top.evm_engine.basic.WeiU256 value = 3;
  bool has_value() const;
  private:
  bool _internal_has_value() const;
  public:
  void clear_value();
  const ::top::evm_engine::basic::WeiU256& value() const;
  ::top::evm_engine::basic::WeiU256* release_value();
  ::top::evm_engine::basic::WeiU256* mutable_value();
  void set_allocated_value(::top::evm_engine::basic::WeiU256* value);
  private:
  const ::top::evm_engine::basic::WeiU256& _internal_value() const;
  ::top::evm_engine::basic::WeiU256* _internal_mutable_value();
  public:
  void unsafe_arena_set_allocated_value(
      ::top::evm_engine::basic::WeiU256* value);
  ::top::evm_engine::basic::WeiU256* unsafe_arena_release_value();

  // uint64 gas_limit = 5;
  void clear_gas_limit();
  ::PROTOBUF_NAMESPACE_ID::uint64 gas_limit() const;
  void set_gas_limit(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_gas_limit() const;
  void _internal_set_gas_limit(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // uint32 version = 1;
  void clear_version();
  ::PROTOBUF_NAMESPACE_ID::uint32 version() const;
  void set_version(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_version() const;
  void _internal_set_version(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // @@protoc_insertion_point(class_scope:top.evm_engine.parameters.FunctionCallArgs)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr input_;
  ::top::evm_engine::basic::ProtoAddress* address_;
  ::top::evm_engine::basic::WeiU256* value_;
  ::PROTOBUF_NAMESPACE_ID::uint64 gas_limit_;
  ::PROTOBUF_NAMESPACE_ID::uint32 version_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_proto_5fparameters_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SubmitResult

// uint32 version = 1;
inline void SubmitResult::clear_version() {
  version_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 SubmitResult::_internal_version() const {
  return version_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 SubmitResult::version() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.SubmitResult.version)
  return _internal_version();
}
inline void SubmitResult::_internal_set_version(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  version_ = value;
}
inline void SubmitResult::set_version(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_version(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.SubmitResult.version)
}

// uint32 transaction_status = 2;
inline void SubmitResult::clear_transaction_status() {
  transaction_status_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 SubmitResult::_internal_transaction_status() const {
  return transaction_status_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 SubmitResult::transaction_status() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.SubmitResult.transaction_status)
  return _internal_transaction_status();
}
inline void SubmitResult::_internal_set_transaction_status(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  transaction_status_ = value;
}
inline void SubmitResult::set_transaction_status(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_transaction_status(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.SubmitResult.transaction_status)
}

// bytes status_data = 3;
inline void SubmitResult::clear_status_data() {
  status_data_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& SubmitResult::status_data() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.SubmitResult.status_data)
  return _internal_status_data();
}
inline void SubmitResult::set_status_data(const std::string& value) {
  _internal_set_status_data(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.SubmitResult.status_data)
}
inline std::string* SubmitResult::mutable_status_data() {
  // @@protoc_insertion_point(field_mutable:top.evm_engine.parameters.SubmitResult.status_data)
  return _internal_mutable_status_data();
}
inline const std::string& SubmitResult::_internal_status_data() const {
  return status_data_.Get();
}
inline void SubmitResult::_internal_set_status_data(const std::string& value) {
  
  status_data_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void SubmitResult::set_status_data(std::string&& value) {
  
  status_data_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:top.evm_engine.parameters.SubmitResult.status_data)
}
inline void SubmitResult::set_status_data(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  status_data_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:top.evm_engine.parameters.SubmitResult.status_data)
}
inline void SubmitResult::set_status_data(const void* value,
    size_t size) {
  
  status_data_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:top.evm_engine.parameters.SubmitResult.status_data)
}
inline std::string* SubmitResult::_internal_mutable_status_data() {
  
  return status_data_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* SubmitResult::release_status_data() {
  // @@protoc_insertion_point(field_release:top.evm_engine.parameters.SubmitResult.status_data)
  return status_data_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void SubmitResult::set_allocated_status_data(std::string* status_data) {
  if (status_data != nullptr) {
    
  } else {
    
  }
  status_data_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), status_data,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:top.evm_engine.parameters.SubmitResult.status_data)
}
inline std::string* SubmitResult::unsafe_arena_release_status_data() {
  // @@protoc_insertion_point(field_unsafe_arena_release:top.evm_engine.parameters.SubmitResult.status_data)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return status_data_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void SubmitResult::unsafe_arena_set_allocated_status_data(
    std::string* status_data) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (status_data != nullptr) {
    
  } else {
    
  }
  status_data_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      status_data, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:top.evm_engine.parameters.SubmitResult.status_data)
}

// uint64 gas_used = 4;
inline void SubmitResult::clear_gas_used() {
  gas_used_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 SubmitResult::_internal_gas_used() const {
  return gas_used_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 SubmitResult::gas_used() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.SubmitResult.gas_used)
  return _internal_gas_used();
}
inline void SubmitResult::_internal_set_gas_used(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  
  gas_used_ = value;
}
inline void SubmitResult::set_gas_used(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_gas_used(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.SubmitResult.gas_used)
}

// repeated .top.evm_engine.basic.ResultLog logs = 5;
inline int SubmitResult::_internal_logs_size() const {
  return logs_.size();
}
inline int SubmitResult::logs_size() const {
  return _internal_logs_size();
}
inline ::top::evm_engine::basic::ResultLog* SubmitResult::mutable_logs(int index) {
  // @@protoc_insertion_point(field_mutable:top.evm_engine.parameters.SubmitResult.logs)
  return logs_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::top::evm_engine::basic::ResultLog >*
SubmitResult::mutable_logs() {
  // @@protoc_insertion_point(field_mutable_list:top.evm_engine.parameters.SubmitResult.logs)
  return &logs_;
}
inline const ::top::evm_engine::basic::ResultLog& SubmitResult::_internal_logs(int index) const {
  return logs_.Get(index);
}
inline const ::top::evm_engine::basic::ResultLog& SubmitResult::logs(int index) const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.SubmitResult.logs)
  return _internal_logs(index);
}
inline ::top::evm_engine::basic::ResultLog* SubmitResult::_internal_add_logs() {
  return logs_.Add();
}
inline ::top::evm_engine::basic::ResultLog* SubmitResult::add_logs() {
  // @@protoc_insertion_point(field_add:top.evm_engine.parameters.SubmitResult.logs)
  return _internal_add_logs();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::top::evm_engine::basic::ResultLog >&
SubmitResult::logs() const {
  // @@protoc_insertion_point(field_list:top.evm_engine.parameters.SubmitResult.logs)
  return logs_;
}

// -------------------------------------------------------------------

// FunctionCallArgs

// uint32 version = 1;
inline void FunctionCallArgs::clear_version() {
  version_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 FunctionCallArgs::_internal_version() const {
  return version_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 FunctionCallArgs::version() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.FunctionCallArgs.version)
  return _internal_version();
}
inline void FunctionCallArgs::_internal_set_version(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  version_ = value;
}
inline void FunctionCallArgs::set_version(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_version(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.FunctionCallArgs.version)
}

// .top.evm_engine.basic.ProtoAddress address = 2;
inline bool FunctionCallArgs::_internal_has_address() const {
  return this != internal_default_instance() && address_ != nullptr;
}
inline bool FunctionCallArgs::has_address() const {
  return _internal_has_address();
}
inline const ::top::evm_engine::basic::ProtoAddress& FunctionCallArgs::_internal_address() const {
  const ::top::evm_engine::basic::ProtoAddress* p = address_;
  return p != nullptr ? *p : *reinterpret_cast<const ::top::evm_engine::basic::ProtoAddress*>(
      &::top::evm_engine::basic::_ProtoAddress_default_instance_);
}
inline const ::top::evm_engine::basic::ProtoAddress& FunctionCallArgs::address() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.FunctionCallArgs.address)
  return _internal_address();
}
inline void FunctionCallArgs::unsafe_arena_set_allocated_address(
    ::top::evm_engine::basic::ProtoAddress* address) {
  if (GetArena() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(address_);
  }
  address_ = address;
  if (address) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:top.evm_engine.parameters.FunctionCallArgs.address)
}
inline ::top::evm_engine::basic::ProtoAddress* FunctionCallArgs::release_address() {
  auto temp = unsafe_arena_release_address();
  if (GetArena() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
  return temp;
}
inline ::top::evm_engine::basic::ProtoAddress* FunctionCallArgs::unsafe_arena_release_address() {
  // @@protoc_insertion_point(field_release:top.evm_engine.parameters.FunctionCallArgs.address)
  
  ::top::evm_engine::basic::ProtoAddress* temp = address_;
  address_ = nullptr;
  return temp;
}
inline ::top::evm_engine::basic::ProtoAddress* FunctionCallArgs::_internal_mutable_address() {
  
  if (address_ == nullptr) {
    auto* p = CreateMaybeMessage<::top::evm_engine::basic::ProtoAddress>(GetArena());
    address_ = p;
  }
  return address_;
}
inline ::top::evm_engine::basic::ProtoAddress* FunctionCallArgs::mutable_address() {
  // @@protoc_insertion_point(field_mutable:top.evm_engine.parameters.FunctionCallArgs.address)
  return _internal_mutable_address();
}
inline void FunctionCallArgs::set_allocated_address(::top::evm_engine::basic::ProtoAddress* address) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArena();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(address_);
  }
  if (address) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(address)->GetArena();
    if (message_arena != submessage_arena) {
      address = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, address, submessage_arena);
    }
    
  } else {
    
  }
  address_ = address;
  // @@protoc_insertion_point(field_set_allocated:top.evm_engine.parameters.FunctionCallArgs.address)
}

// .top.evm_engine.basic.WeiU256 value = 3;
inline bool FunctionCallArgs::_internal_has_value() const {
  return this != internal_default_instance() && value_ != nullptr;
}
inline bool FunctionCallArgs::has_value() const {
  return _internal_has_value();
}
inline const ::top::evm_engine::basic::WeiU256& FunctionCallArgs::_internal_value() const {
  const ::top::evm_engine::basic::WeiU256* p = value_;
  return p != nullptr ? *p : *reinterpret_cast<const ::top::evm_engine::basic::WeiU256*>(
      &::top::evm_engine::basic::_WeiU256_default_instance_);
}
inline const ::top::evm_engine::basic::WeiU256& FunctionCallArgs::value() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.FunctionCallArgs.value)
  return _internal_value();
}
inline void FunctionCallArgs::unsafe_arena_set_allocated_value(
    ::top::evm_engine::basic::WeiU256* value) {
  if (GetArena() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(value_);
  }
  value_ = value;
  if (value) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:top.evm_engine.parameters.FunctionCallArgs.value)
}
inline ::top::evm_engine::basic::WeiU256* FunctionCallArgs::release_value() {
  auto temp = unsafe_arena_release_value();
  if (GetArena() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
  return temp;
}
inline ::top::evm_engine::basic::WeiU256* FunctionCallArgs::unsafe_arena_release_value() {
  // @@protoc_insertion_point(field_release:top.evm_engine.parameters.FunctionCallArgs.value)
  
  ::top::evm_engine::basic::WeiU256* temp = value_;
  value_ = nullptr;
  return temp;
}
inline ::top::evm_engine::basic::WeiU256* FunctionCallArgs::_internal_mutable_value() {
  
  if (value_ == nullptr) {
    auto* p = CreateMaybeMessage<::top::evm_engine::basic::WeiU256>(GetArena());
    value_ = p;
  }
  return value_;
}
inline ::top::evm_engine::basic::WeiU256* FunctionCallArgs::mutable_value() {
  // @@protoc_insertion_point(field_mutable:top.evm_engine.parameters.FunctionCallArgs.value)
  return _internal_mutable_value();
}
inline void FunctionCallArgs::set_allocated_value(::top::evm_engine::basic::WeiU256* value) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArena();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(value_);
  }
  if (value) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(value)->GetArena();
    if (message_arena != submessage_arena) {
      value = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, value, submessage_arena);
    }
    
  } else {
    
  }
  value_ = value;
  // @@protoc_insertion_point(field_set_allocated:top.evm_engine.parameters.FunctionCallArgs.value)
}

// bytes input = 4;
inline void FunctionCallArgs::clear_input() {
  input_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& FunctionCallArgs::input() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.FunctionCallArgs.input)
  return _internal_input();
}
inline void FunctionCallArgs::set_input(const std::string& value) {
  _internal_set_input(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.FunctionCallArgs.input)
}
inline std::string* FunctionCallArgs::mutable_input() {
  // @@protoc_insertion_point(field_mutable:top.evm_engine.parameters.FunctionCallArgs.input)
  return _internal_mutable_input();
}
inline const std::string& FunctionCallArgs::_internal_input() const {
  return input_.Get();
}
inline void FunctionCallArgs::_internal_set_input(const std::string& value) {
  
  input_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void FunctionCallArgs::set_input(std::string&& value) {
  
  input_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:top.evm_engine.parameters.FunctionCallArgs.input)
}
inline void FunctionCallArgs::set_input(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  input_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:top.evm_engine.parameters.FunctionCallArgs.input)
}
inline void FunctionCallArgs::set_input(const void* value,
    size_t size) {
  
  input_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:top.evm_engine.parameters.FunctionCallArgs.input)
}
inline std::string* FunctionCallArgs::_internal_mutable_input() {
  
  return input_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* FunctionCallArgs::release_input() {
  // @@protoc_insertion_point(field_release:top.evm_engine.parameters.FunctionCallArgs.input)
  return input_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void FunctionCallArgs::set_allocated_input(std::string* input) {
  if (input != nullptr) {
    
  } else {
    
  }
  input_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), input,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:top.evm_engine.parameters.FunctionCallArgs.input)
}
inline std::string* FunctionCallArgs::unsafe_arena_release_input() {
  // @@protoc_insertion_point(field_unsafe_arena_release:top.evm_engine.parameters.FunctionCallArgs.input)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return input_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void FunctionCallArgs::unsafe_arena_set_allocated_input(
    std::string* input) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (input != nullptr) {
    
  } else {
    
  }
  input_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      input, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:top.evm_engine.parameters.FunctionCallArgs.input)
}

// uint64 gas_limit = 5;
inline void FunctionCallArgs::clear_gas_limit() {
  gas_limit_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 FunctionCallArgs::_internal_gas_limit() const {
  return gas_limit_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 FunctionCallArgs::gas_limit() const {
  // @@protoc_insertion_point(field_get:top.evm_engine.parameters.FunctionCallArgs.gas_limit)
  return _internal_gas_limit();
}
inline void FunctionCallArgs::_internal_set_gas_limit(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  
  gas_limit_ = value;
}
inline void FunctionCallArgs::set_gas_limit(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_gas_limit(value);
  // @@protoc_insertion_point(field_set:top.evm_engine.parameters.FunctionCallArgs.gas_limit)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace parameters
}  // namespace evm_engine
}  // namespace top

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_proto_5fparameters_2eproto

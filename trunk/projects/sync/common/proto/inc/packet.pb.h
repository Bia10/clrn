// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: packet.proto

#ifndef PROTOBUF_packet_2eproto__INCLUDED
#define PROTOBUF_packet_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "job.pb.h"
// @@protoc_insertion_point(includes)

namespace packets {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_packet_2eproto();
void protobuf_AssignDesc_packet_2eproto();
void protobuf_ShutdownFile_packet_2eproto();

class Packet;
class Packet_Destination;
class Packet_Trace;

enum Packet_Destination_Type {
  Packet_Destination_Type_ONE = 1,
  Packet_Destination_Type_RANGE = 2,
  Packet_Destination_Type_ALL = 3
};
bool Packet_Destination_Type_IsValid(int value);
const Packet_Destination_Type Packet_Destination_Type_Type_MIN = Packet_Destination_Type_ONE;
const Packet_Destination_Type Packet_Destination_Type_Type_MAX = Packet_Destination_Type_ALL;
const int Packet_Destination_Type_Type_ARRAYSIZE = Packet_Destination_Type_Type_MAX + 1;

const ::google::protobuf::EnumDescriptor* Packet_Destination_Type_descriptor();
inline const ::std::string& Packet_Destination_Type_Name(Packet_Destination_Type value) {
  return ::google::protobuf::internal::NameOfEnum(
    Packet_Destination_Type_descriptor(), value);
}
inline bool Packet_Destination_Type_Parse(
    const ::std::string& name, Packet_Destination_Type* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Packet_Destination_Type>(
    Packet_Destination_Type_descriptor(), name, value);
}
enum Packet_PacketType {
  Packet_PacketType_REQUEST = 1,
  Packet_PacketType_REPLY = 2,
  Packet_PacketType_ERR = 3,
  Packet_PacketType_PING = 4,
  Packet_PacketType_ACK = 5,
  Packet_PacketType_CONNECT = 6
};
bool Packet_PacketType_IsValid(int value);
const Packet_PacketType Packet_PacketType_PacketType_MIN = Packet_PacketType_REQUEST;
const Packet_PacketType Packet_PacketType_PacketType_MAX = Packet_PacketType_CONNECT;
const int Packet_PacketType_PacketType_ARRAYSIZE = Packet_PacketType_PacketType_MAX + 1;

const ::google::protobuf::EnumDescriptor* Packet_PacketType_descriptor();
inline const ::std::string& Packet_PacketType_Name(Packet_PacketType value) {
  return ::google::protobuf::internal::NameOfEnum(
    Packet_PacketType_descriptor(), value);
}
inline bool Packet_PacketType_Parse(
    const ::std::string& name, Packet_PacketType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Packet_PacketType>(
    Packet_PacketType_descriptor(), name, value);
}
// ===================================================================

class Packet_Destination : public ::google::protobuf::Message {
 public:
  Packet_Destination();
  virtual ~Packet_Destination();
  
  Packet_Destination(const Packet_Destination& from);
  
  inline Packet_Destination& operator=(const Packet_Destination& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Packet_Destination& default_instance();
  
  void Swap(Packet_Destination* other);
  
  // implements Message ----------------------------------------------
  
  Packet_Destination* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet_Destination& from);
  void MergeFrom(const Packet_Destination& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  typedef Packet_Destination_Type Type;
  static const Type ONE = Packet_Destination_Type_ONE;
  static const Type RANGE = Packet_Destination_Type_RANGE;
  static const Type ALL = Packet_Destination_Type_ALL;
  static inline bool Type_IsValid(int value) {
    return Packet_Destination_Type_IsValid(value);
  }
  static const Type Type_MIN =
    Packet_Destination_Type_Type_MIN;
  static const Type Type_MAX =
    Packet_Destination_Type_Type_MAX;
  static const int Type_ARRAYSIZE =
    Packet_Destination_Type_Type_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Type_descriptor() {
    return Packet_Destination_Type_descriptor();
  }
  static inline const ::std::string& Type_Name(Type value) {
    return Packet_Destination_Type_Name(value);
  }
  static inline bool Type_Parse(const ::std::string& name,
      Type* value) {
    return Packet_Destination_Type_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required .packets.Packet.Destination.Type type = 1 [default = ONE];
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline ::packets::Packet_Destination_Type type() const;
  inline void set_type(::packets::Packet_Destination_Type value);
  
  // optional uint32 from = 2;
  inline bool has_from() const;
  inline void clear_from();
  static const int kFromFieldNumber = 2;
  inline ::google::protobuf::uint32 from() const;
  inline void set_from(::google::protobuf::uint32 value);
  
  // optional int32 to = 3;
  inline bool has_to() const;
  inline void clear_to();
  static const int kToFieldNumber = 3;
  inline ::google::protobuf::int32 to() const;
  inline void set_to(::google::protobuf::int32 value);
  
  // optional bytes filter = 4;
  inline bool has_filter() const;
  inline void clear_filter();
  static const int kFilterFieldNumber = 4;
  inline const ::std::string& filter() const;
  inline void set_filter(const ::std::string& value);
  inline void set_filter(const char* value);
  inline void set_filter(const void* value, size_t size);
  inline ::std::string* mutable_filter();
  inline ::std::string* release_filter();
  
  // @@protoc_insertion_point(class_scope:packets.Packet.Destination)
 private:
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_from();
  inline void clear_has_from();
  inline void set_has_to();
  inline void clear_has_to();
  inline void set_has_filter();
  inline void clear_has_filter();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  int type_;
  ::google::protobuf::uint32 from_;
  ::std::string* filter_;
  ::google::protobuf::int32 to_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet_Destination* default_instance_;
};
// -------------------------------------------------------------------

class Packet_Trace : public ::google::protobuf::Message {
 public:
  Packet_Trace();
  virtual ~Packet_Trace();
  
  Packet_Trace(const Packet_Trace& from);
  
  inline Packet_Trace& operator=(const Packet_Trace& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Packet_Trace& default_instance();
  
  void Swap(Packet_Trace* other);
  
  // implements Message ----------------------------------------------
  
  Packet_Trace* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet_Trace& from);
  void MergeFrom(const Packet_Trace& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required bytes info = 1;
  inline bool has_info() const;
  inline void clear_info();
  static const int kInfoFieldNumber = 1;
  inline const ::std::string& info() const;
  inline void set_info(const ::std::string& value);
  inline void set_info(const char* value);
  inline void set_info(const void* value, size_t size);
  inline ::std::string* mutable_info();
  inline ::std::string* release_info();
  
  // @@protoc_insertion_point(class_scope:packets.Packet.Trace)
 private:
  inline void set_has_info();
  inline void clear_has_info();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* info_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet_Trace* default_instance_;
};
// -------------------------------------------------------------------

class Packet : public ::google::protobuf::Message {
 public:
  Packet();
  virtual ~Packet();
  
  Packet(const Packet& from);
  
  inline Packet& operator=(const Packet& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Packet& default_instance();
  
  void Swap(Packet* other);
  
  // implements Message ----------------------------------------------
  
  Packet* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet& from);
  void MergeFrom(const Packet& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  typedef Packet_Destination Destination;
  typedef Packet_Trace Trace;
  
  typedef Packet_PacketType PacketType;
  static const PacketType REQUEST = Packet_PacketType_REQUEST;
  static const PacketType REPLY = Packet_PacketType_REPLY;
  static const PacketType ERR = Packet_PacketType_ERR;
  static const PacketType PING = Packet_PacketType_PING;
  static const PacketType ACK = Packet_PacketType_ACK;
  static const PacketType CONNECT = Packet_PacketType_CONNECT;
  static inline bool PacketType_IsValid(int value) {
    return Packet_PacketType_IsValid(value);
  }
  static const PacketType PacketType_MIN =
    Packet_PacketType_PacketType_MIN;
  static const PacketType PacketType_MAX =
    Packet_PacketType_PacketType_MAX;
  static const int PacketType_ARRAYSIZE =
    Packet_PacketType_PacketType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  PacketType_descriptor() {
    return Packet_PacketType_descriptor();
  }
  static inline const ::std::string& PacketType_Name(PacketType value) {
    return Packet_PacketType_Name(value);
  }
  static inline bool PacketType_Parse(const ::std::string& name,
      PacketType* value) {
    return Packet_PacketType_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required .packets.Packet.PacketType type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline ::packets::Packet_PacketType type() const;
  inline void set_type(::packets::Packet_PacketType value);
  
  // optional .jobs.Job job = 2;
  inline bool has_job() const;
  inline void clear_job();
  static const int kJobFieldNumber = 2;
  inline const ::jobs::Job& job() const;
  inline ::jobs::Job* mutable_job();
  inline ::jobs::Job* release_job();
  
  // optional .packets.Packet.Destination destination = 3;
  inline bool has_destination() const;
  inline void clear_destination();
  static const int kDestinationFieldNumber = 3;
  inline const ::packets::Packet_Destination& destination() const;
  inline ::packets::Packet_Destination* mutable_destination();
  inline ::packets::Packet_Destination* release_destination();
  
  // required bytes guid = 4;
  inline bool has_guid() const;
  inline void clear_guid();
  static const int kGuidFieldNumber = 4;
  inline const ::std::string& guid() const;
  inline void set_guid(const ::std::string& value);
  inline void set_guid(const char* value);
  inline void set_guid(const void* value, size_t size);
  inline ::std::string* mutable_guid();
  inline ::std::string* release_guid();
  
  // optional uint32 timeout = 5;
  inline bool has_timeout() const;
  inline void clear_timeout();
  static const int kTimeoutFieldNumber = 5;
  inline ::google::protobuf::uint32 timeout() const;
  inline void set_timeout(::google::protobuf::uint32 value);
  
  // required bytes from = 6;
  inline bool has_from() const;
  inline void clear_from();
  static const int kFromFieldNumber = 6;
  inline const ::std::string& from() const;
  inline void set_from(const ::std::string& value);
  inline void set_from(const char* value);
  inline void set_from(const void* value, size_t size);
  inline ::std::string* mutable_from();
  inline ::std::string* release_from();
  
  // optional bytes ep = 7;
  inline bool has_ep() const;
  inline void clear_ep();
  static const int kEpFieldNumber = 7;
  inline const ::std::string& ep() const;
  inline void set_ep(const ::std::string& value);
  inline void set_ep(const char* value);
  inline void set_ep(const void* value, size_t size);
  inline ::std::string* mutable_ep();
  inline ::std::string* release_ep();
  
  // repeated .packets.Packet.Trace trace = 8;
  inline int trace_size() const;
  inline void clear_trace();
  static const int kTraceFieldNumber = 8;
  inline const ::packets::Packet_Trace& trace(int index) const;
  inline ::packets::Packet_Trace* mutable_trace(int index);
  inline ::packets::Packet_Trace* add_trace();
  inline const ::google::protobuf::RepeatedPtrField< ::packets::Packet_Trace >&
      trace() const;
  inline ::google::protobuf::RepeatedPtrField< ::packets::Packet_Trace >*
      mutable_trace();
  
  // @@protoc_insertion_point(class_scope:packets.Packet)
 private:
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_job();
  inline void clear_has_job();
  inline void set_has_destination();
  inline void clear_has_destination();
  inline void set_has_guid();
  inline void clear_has_guid();
  inline void set_has_timeout();
  inline void clear_has_timeout();
  inline void set_has_from();
  inline void clear_has_from();
  inline void set_has_ep();
  inline void clear_has_ep();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::jobs::Job* job_;
  ::packets::Packet_Destination* destination_;
  int type_;
  ::google::protobuf::uint32 timeout_;
  ::std::string* guid_;
  ::std::string* from_;
  ::std::string* ep_;
  ::google::protobuf::RepeatedPtrField< ::packets::Packet_Trace > trace_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(8 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet* default_instance_;
};
// ===================================================================


// ===================================================================

// Packet_Destination

// required .packets.Packet.Destination.Type type = 1 [default = ONE];
inline bool Packet_Destination::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet_Destination::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet_Destination::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet_Destination::clear_type() {
  type_ = 1;
  clear_has_type();
}
inline ::packets::Packet_Destination_Type Packet_Destination::type() const {
  return static_cast< ::packets::Packet_Destination_Type >(type_);
}
inline void Packet_Destination::set_type(::packets::Packet_Destination_Type value) {
  GOOGLE_DCHECK(::packets::Packet_Destination_Type_IsValid(value));
  set_has_type();
  type_ = value;
}

// optional uint32 from = 2;
inline bool Packet_Destination::has_from() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet_Destination::set_has_from() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet_Destination::clear_has_from() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet_Destination::clear_from() {
  from_ = 0u;
  clear_has_from();
}
inline ::google::protobuf::uint32 Packet_Destination::from() const {
  return from_;
}
inline void Packet_Destination::set_from(::google::protobuf::uint32 value) {
  set_has_from();
  from_ = value;
}

// optional int32 to = 3;
inline bool Packet_Destination::has_to() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet_Destination::set_has_to() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet_Destination::clear_has_to() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet_Destination::clear_to() {
  to_ = 0;
  clear_has_to();
}
inline ::google::protobuf::int32 Packet_Destination::to() const {
  return to_;
}
inline void Packet_Destination::set_to(::google::protobuf::int32 value) {
  set_has_to();
  to_ = value;
}

// optional bytes filter = 4;
inline bool Packet_Destination::has_filter() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Packet_Destination::set_has_filter() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Packet_Destination::clear_has_filter() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Packet_Destination::clear_filter() {
  if (filter_ != &::google::protobuf::internal::kEmptyString) {
    filter_->clear();
  }
  clear_has_filter();
}
inline const ::std::string& Packet_Destination::filter() const {
  return *filter_;
}
inline void Packet_Destination::set_filter(const ::std::string& value) {
  set_has_filter();
  if (filter_ == &::google::protobuf::internal::kEmptyString) {
    filter_ = new ::std::string;
  }
  filter_->assign(value);
}
inline void Packet_Destination::set_filter(const char* value) {
  set_has_filter();
  if (filter_ == &::google::protobuf::internal::kEmptyString) {
    filter_ = new ::std::string;
  }
  filter_->assign(value);
}
inline void Packet_Destination::set_filter(const void* value, size_t size) {
  set_has_filter();
  if (filter_ == &::google::protobuf::internal::kEmptyString) {
    filter_ = new ::std::string;
  }
  filter_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet_Destination::mutable_filter() {
  set_has_filter();
  if (filter_ == &::google::protobuf::internal::kEmptyString) {
    filter_ = new ::std::string;
  }
  return filter_;
}
inline ::std::string* Packet_Destination::release_filter() {
  clear_has_filter();
  if (filter_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = filter_;
    filter_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// Packet_Trace

// required bytes info = 1;
inline bool Packet_Trace::has_info() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet_Trace::set_has_info() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet_Trace::clear_has_info() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet_Trace::clear_info() {
  if (info_ != &::google::protobuf::internal::kEmptyString) {
    info_->clear();
  }
  clear_has_info();
}
inline const ::std::string& Packet_Trace::info() const {
  return *info_;
}
inline void Packet_Trace::set_info(const ::std::string& value) {
  set_has_info();
  if (info_ == &::google::protobuf::internal::kEmptyString) {
    info_ = new ::std::string;
  }
  info_->assign(value);
}
inline void Packet_Trace::set_info(const char* value) {
  set_has_info();
  if (info_ == &::google::protobuf::internal::kEmptyString) {
    info_ = new ::std::string;
  }
  info_->assign(value);
}
inline void Packet_Trace::set_info(const void* value, size_t size) {
  set_has_info();
  if (info_ == &::google::protobuf::internal::kEmptyString) {
    info_ = new ::std::string;
  }
  info_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet_Trace::mutable_info() {
  set_has_info();
  if (info_ == &::google::protobuf::internal::kEmptyString) {
    info_ = new ::std::string;
  }
  return info_;
}
inline ::std::string* Packet_Trace::release_info() {
  clear_has_info();
  if (info_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = info_;
    info_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// Packet

// required .packets.Packet.PacketType type = 1;
inline bool Packet::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet::clear_type() {
  type_ = 1;
  clear_has_type();
}
inline ::packets::Packet_PacketType Packet::type() const {
  return static_cast< ::packets::Packet_PacketType >(type_);
}
inline void Packet::set_type(::packets::Packet_PacketType value) {
  GOOGLE_DCHECK(::packets::Packet_PacketType_IsValid(value));
  set_has_type();
  type_ = value;
}

// optional .jobs.Job job = 2;
inline bool Packet::has_job() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet::set_has_job() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet::clear_has_job() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet::clear_job() {
  if (job_ != NULL) job_->::jobs::Job::Clear();
  clear_has_job();
}
inline const ::jobs::Job& Packet::job() const {
  return job_ != NULL ? *job_ : *default_instance_->job_;
}
inline ::jobs::Job* Packet::mutable_job() {
  set_has_job();
  if (job_ == NULL) job_ = new ::jobs::Job;
  return job_;
}
inline ::jobs::Job* Packet::release_job() {
  clear_has_job();
  ::jobs::Job* temp = job_;
  job_ = NULL;
  return temp;
}

// optional .packets.Packet.Destination destination = 3;
inline bool Packet::has_destination() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet::set_has_destination() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet::clear_has_destination() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet::clear_destination() {
  if (destination_ != NULL) destination_->::packets::Packet_Destination::Clear();
  clear_has_destination();
}
inline const ::packets::Packet_Destination& Packet::destination() const {
  return destination_ != NULL ? *destination_ : *default_instance_->destination_;
}
inline ::packets::Packet_Destination* Packet::mutable_destination() {
  set_has_destination();
  if (destination_ == NULL) destination_ = new ::packets::Packet_Destination;
  return destination_;
}
inline ::packets::Packet_Destination* Packet::release_destination() {
  clear_has_destination();
  ::packets::Packet_Destination* temp = destination_;
  destination_ = NULL;
  return temp;
}

// required bytes guid = 4;
inline bool Packet::has_guid() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Packet::set_has_guid() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Packet::clear_has_guid() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Packet::clear_guid() {
  if (guid_ != &::google::protobuf::internal::kEmptyString) {
    guid_->clear();
  }
  clear_has_guid();
}
inline const ::std::string& Packet::guid() const {
  return *guid_;
}
inline void Packet::set_guid(const ::std::string& value) {
  set_has_guid();
  if (guid_ == &::google::protobuf::internal::kEmptyString) {
    guid_ = new ::std::string;
  }
  guid_->assign(value);
}
inline void Packet::set_guid(const char* value) {
  set_has_guid();
  if (guid_ == &::google::protobuf::internal::kEmptyString) {
    guid_ = new ::std::string;
  }
  guid_->assign(value);
}
inline void Packet::set_guid(const void* value, size_t size) {
  set_has_guid();
  if (guid_ == &::google::protobuf::internal::kEmptyString) {
    guid_ = new ::std::string;
  }
  guid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet::mutable_guid() {
  set_has_guid();
  if (guid_ == &::google::protobuf::internal::kEmptyString) {
    guid_ = new ::std::string;
  }
  return guid_;
}
inline ::std::string* Packet::release_guid() {
  clear_has_guid();
  if (guid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = guid_;
    guid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional uint32 timeout = 5;
inline bool Packet::has_timeout() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Packet::set_has_timeout() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Packet::clear_has_timeout() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Packet::clear_timeout() {
  timeout_ = 0u;
  clear_has_timeout();
}
inline ::google::protobuf::uint32 Packet::timeout() const {
  return timeout_;
}
inline void Packet::set_timeout(::google::protobuf::uint32 value) {
  set_has_timeout();
  timeout_ = value;
}

// required bytes from = 6;
inline bool Packet::has_from() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void Packet::set_has_from() {
  _has_bits_[0] |= 0x00000020u;
}
inline void Packet::clear_has_from() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void Packet::clear_from() {
  if (from_ != &::google::protobuf::internal::kEmptyString) {
    from_->clear();
  }
  clear_has_from();
}
inline const ::std::string& Packet::from() const {
  return *from_;
}
inline void Packet::set_from(const ::std::string& value) {
  set_has_from();
  if (from_ == &::google::protobuf::internal::kEmptyString) {
    from_ = new ::std::string;
  }
  from_->assign(value);
}
inline void Packet::set_from(const char* value) {
  set_has_from();
  if (from_ == &::google::protobuf::internal::kEmptyString) {
    from_ = new ::std::string;
  }
  from_->assign(value);
}
inline void Packet::set_from(const void* value, size_t size) {
  set_has_from();
  if (from_ == &::google::protobuf::internal::kEmptyString) {
    from_ = new ::std::string;
  }
  from_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet::mutable_from() {
  set_has_from();
  if (from_ == &::google::protobuf::internal::kEmptyString) {
    from_ = new ::std::string;
  }
  return from_;
}
inline ::std::string* Packet::release_from() {
  clear_has_from();
  if (from_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = from_;
    from_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional bytes ep = 7;
inline bool Packet::has_ep() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void Packet::set_has_ep() {
  _has_bits_[0] |= 0x00000040u;
}
inline void Packet::clear_has_ep() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void Packet::clear_ep() {
  if (ep_ != &::google::protobuf::internal::kEmptyString) {
    ep_->clear();
  }
  clear_has_ep();
}
inline const ::std::string& Packet::ep() const {
  return *ep_;
}
inline void Packet::set_ep(const ::std::string& value) {
  set_has_ep();
  if (ep_ == &::google::protobuf::internal::kEmptyString) {
    ep_ = new ::std::string;
  }
  ep_->assign(value);
}
inline void Packet::set_ep(const char* value) {
  set_has_ep();
  if (ep_ == &::google::protobuf::internal::kEmptyString) {
    ep_ = new ::std::string;
  }
  ep_->assign(value);
}
inline void Packet::set_ep(const void* value, size_t size) {
  set_has_ep();
  if (ep_ == &::google::protobuf::internal::kEmptyString) {
    ep_ = new ::std::string;
  }
  ep_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet::mutable_ep() {
  set_has_ep();
  if (ep_ == &::google::protobuf::internal::kEmptyString) {
    ep_ = new ::std::string;
  }
  return ep_;
}
inline ::std::string* Packet::release_ep() {
  clear_has_ep();
  if (ep_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = ep_;
    ep_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated .packets.Packet.Trace trace = 8;
inline int Packet::trace_size() const {
  return trace_.size();
}
inline void Packet::clear_trace() {
  trace_.Clear();
}
inline const ::packets::Packet_Trace& Packet::trace(int index) const {
  return trace_.Get(index);
}
inline ::packets::Packet_Trace* Packet::mutable_trace(int index) {
  return trace_.Mutable(index);
}
inline ::packets::Packet_Trace* Packet::add_trace() {
  return trace_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::packets::Packet_Trace >&
Packet::trace() const {
  return trace_;
}
inline ::google::protobuf::RepeatedPtrField< ::packets::Packet_Trace >*
Packet::mutable_trace() {
  return &trace_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace packets

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::packets::Packet_Destination_Type>() {
  return ::packets::Packet_Destination_Type_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::packets::Packet_PacketType>() {
  return ::packets::Packet_PacketType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_packet_2eproto__INCLUDED

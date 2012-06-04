// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#ifndef PROTOBUF_message_2eproto__INCLUDED
#define PROTOBUF_message_2eproto__INCLUDED

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
// @@protoc_insertion_point(includes)

namespace net {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_message_2eproto();
void protobuf_AssignDesc_message_2eproto();
void protobuf_ShutdownFile_message_2eproto();

class Packet;
class Batch;

enum Packet_PacketType {
  Packet_PacketType_DATA = 0,
  Packet_PacketType_REPLY = 1,
  Packet_PacketType_REQUEST = 2
};
bool Packet_PacketType_IsValid(int value);
const Packet_PacketType Packet_PacketType_PacketType_MIN = Packet_PacketType_DATA;
const Packet_PacketType Packet_PacketType_PacketType_MAX = Packet_PacketType_REQUEST;
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
  
  typedef Packet_PacketType PacketType;
  static const PacketType DATA = Packet_PacketType_DATA;
  static const PacketType REPLY = Packet_PacketType_REPLY;
  static const PacketType REQUEST = Packet_PacketType_REQUEST;
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
  
  // optional int64 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int64 id() const;
  inline void set_id(::google::protobuf::int64 value);
  
  // optional int64 next = 2;
  inline bool has_next() const;
  inline void clear_next();
  static const int kNextFieldNumber = 2;
  inline ::google::protobuf::int64 next() const;
  inline void set_next(::google::protobuf::int64 value);
  
  // required .net.Packet.PacketType type = 3 [default = DATA];
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 3;
  inline ::net::Packet_PacketType type() const;
  inline void set_type(::net::Packet_PacketType value);
  
  // optional bytes data = 4;
  inline bool has_data() const;
  inline void clear_data();
  static const int kDataFieldNumber = 4;
  inline const ::std::string& data() const;
  inline void set_data(const ::std::string& value);
  inline void set_data(const char* value);
  inline void set_data(const void* value, size_t size);
  inline ::std::string* mutable_data();
  inline ::std::string* release_data();
  
  // @@protoc_insertion_point(class_scope:net.Packet)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_next();
  inline void clear_has_next();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_data();
  inline void clear_has_data();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::int64 id_;
  ::google::protobuf::int64 next_;
  ::std::string* data_;
  int type_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_message_2eproto();
  friend void protobuf_AssignDesc_message_2eproto();
  friend void protobuf_ShutdownFile_message_2eproto();
  
  void InitAsDefaultInstance();
  static Packet* default_instance_;
};
// -------------------------------------------------------------------

class Batch : public ::google::protobuf::Message {
 public:
  Batch();
  virtual ~Batch();
  
  Batch(const Batch& from);
  
  inline Batch& operator=(const Batch& from) {
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
  static const Batch& default_instance();
  
  void Swap(Batch* other);
  
  // implements Message ----------------------------------------------
  
  Batch* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Batch& from);
  void MergeFrom(const Batch& from);
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
  
  // repeated .net.Packet packets = 1;
  inline int packets_size() const;
  inline void clear_packets();
  static const int kPacketsFieldNumber = 1;
  inline const ::net::Packet& packets(int index) const;
  inline ::net::Packet* mutable_packets(int index);
  inline ::net::Packet* add_packets();
  inline const ::google::protobuf::RepeatedPtrField< ::net::Packet >&
      packets() const;
  inline ::google::protobuf::RepeatedPtrField< ::net::Packet >*
      mutable_packets();
  
  // @@protoc_insertion_point(class_scope:net.Batch)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::net::Packet > packets_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_message_2eproto();
  friend void protobuf_AssignDesc_message_2eproto();
  friend void protobuf_ShutdownFile_message_2eproto();
  
  void InitAsDefaultInstance();
  static Batch* default_instance_;
};
// ===================================================================


// ===================================================================

// Packet

// optional int64 id = 1;
inline bool Packet::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet::clear_id() {
  id_ = GOOGLE_LONGLONG(0);
  clear_has_id();
}
inline ::google::protobuf::int64 Packet::id() const {
  return id_;
}
inline void Packet::set_id(::google::protobuf::int64 value) {
  set_has_id();
  id_ = value;
}

// optional int64 next = 2;
inline bool Packet::has_next() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet::set_has_next() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet::clear_has_next() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet::clear_next() {
  next_ = GOOGLE_LONGLONG(0);
  clear_has_next();
}
inline ::google::protobuf::int64 Packet::next() const {
  return next_;
}
inline void Packet::set_next(::google::protobuf::int64 value) {
  set_has_next();
  next_ = value;
}

// required .net.Packet.PacketType type = 3 [default = DATA];
inline bool Packet::has_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet::set_has_type() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet::clear_has_type() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::net::Packet_PacketType Packet::type() const {
  return static_cast< ::net::Packet_PacketType >(type_);
}
inline void Packet::set_type(::net::Packet_PacketType value) {
  GOOGLE_DCHECK(::net::Packet_PacketType_IsValid(value));
  set_has_type();
  type_ = value;
}

// optional bytes data = 4;
inline bool Packet::has_data() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Packet::set_has_data() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Packet::clear_has_data() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Packet::clear_data() {
  if (data_ != &::google::protobuf::internal::kEmptyString) {
    data_->clear();
  }
  clear_has_data();
}
inline const ::std::string& Packet::data() const {
  return *data_;
}
inline void Packet::set_data(const ::std::string& value) {
  set_has_data();
  if (data_ == &::google::protobuf::internal::kEmptyString) {
    data_ = new ::std::string;
  }
  data_->assign(value);
}
inline void Packet::set_data(const char* value) {
  set_has_data();
  if (data_ == &::google::protobuf::internal::kEmptyString) {
    data_ = new ::std::string;
  }
  data_->assign(value);
}
inline void Packet::set_data(const void* value, size_t size) {
  set_has_data();
  if (data_ == &::google::protobuf::internal::kEmptyString) {
    data_ = new ::std::string;
  }
  data_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet::mutable_data() {
  set_has_data();
  if (data_ == &::google::protobuf::internal::kEmptyString) {
    data_ = new ::std::string;
  }
  return data_;
}
inline ::std::string* Packet::release_data() {
  clear_has_data();
  if (data_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = data_;
    data_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// Batch

// repeated .net.Packet packets = 1;
inline int Batch::packets_size() const {
  return packets_.size();
}
inline void Batch::clear_packets() {
  packets_.Clear();
}
inline const ::net::Packet& Batch::packets(int index) const {
  return packets_.Get(index);
}
inline ::net::Packet* Batch::mutable_packets(int index) {
  return packets_.Mutable(index);
}
inline ::net::Packet* Batch::add_packets() {
  return packets_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::net::Packet >&
Batch::packets() const {
  return packets_;
}
inline ::google::protobuf::RepeatedPtrField< ::net::Packet >*
Batch::mutable_packets() {
  return &packets_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace net

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::net::Packet_PacketType>() {
  return ::net::Packet_PacketType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_message_2eproto__INCLUDED

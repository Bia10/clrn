// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "packet.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace packets {

namespace {

const ::google::protobuf::Descriptor* Packet_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Packet_reflection_ = NULL;
const ::google::protobuf::Descriptor* Packet_Destination_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Packet_Destination_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* Packet_Destination_Type_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* Packet_PacketType_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_packet_2eproto() {
  protobuf_AddDesc_packet_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "packet.proto");
  GOOGLE_CHECK(file != NULL);
  Packet_descriptor_ = file->message_type(0);
  static const int Packet_offsets_[8] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, job_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, destination_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, guid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, timeout_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, from_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, ep_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, trace_),
  };
  Packet_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Packet_descriptor_,
      Packet::default_instance_,
      Packet_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Packet));
  Packet_Destination_descriptor_ = Packet_descriptor_->nested_type(0);
  static const int Packet_Destination_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet_Destination, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet_Destination, from_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet_Destination, to_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet_Destination, filter_),
  };
  Packet_Destination_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Packet_Destination_descriptor_,
      Packet_Destination::default_instance_,
      Packet_Destination_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet_Destination, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Packet_Destination, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Packet_Destination));
  Packet_Destination_Type_descriptor_ = Packet_Destination_descriptor_->enum_type(0);
  Packet_PacketType_descriptor_ = Packet_descriptor_->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_packet_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Packet_descriptor_, &Packet::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Packet_Destination_descriptor_, &Packet_Destination::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_packet_2eproto() {
  delete Packet::default_instance_;
  delete Packet_reflection_;
  delete Packet_Destination::default_instance_;
  delete Packet_Destination_reflection_;
}

void protobuf_AddDesc_packet_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::jobs::protobuf_AddDesc_job_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\014packet.proto\022\007packets\032\tjob.proto\"\247\003\n\006P"
    "acket\022(\n\004type\030\001 \002(\0162\032.packets.Packet.Pac"
    "ketType\022\026\n\003job\030\002 \001(\0132\t.jobs.Job\0220\n\013desti"
    "nation\030\003 \001(\0132\033.packets.Packet.Destinatio"
    "n\022\014\n\004guid\030\004 \002(\014\022\017\n\007timeout\030\005 \001(\r\022\014\n\004from"
    "\030\006 \002(\014\022\n\n\002ep\030\007 \001(\014\022\r\n\005trace\030\010 \001(\014\032\221\001\n\013De"
    "stination\0223\n\004type\030\001 \002(\0162 .packets.Packet"
    ".Destination.Type:\003ONE\022\014\n\004from\030\002 \001(\r\022\n\n\002"
    "to\030\003 \001(\005\022\016\n\006filter\030\004 \001(\014\"#\n\004Type\022\007\n\003ONE\020"
    "\001\022\t\n\005RANGE\020\002\022\007\n\003ALL\020\003\"M\n\nPacketType\022\013\n\007R"
    "EQUEST\020\001\022\t\n\005REPLY\020\002\022\007\n\003ERR\020\003\022\010\n\004PING\020\004\022\007"
    "\n\003ACK\020\005\022\013\n\007CONNECT\020\006", 460);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "packet.proto", &protobuf_RegisterTypes);
  Packet::default_instance_ = new Packet();
  Packet_Destination::default_instance_ = new Packet_Destination();
  Packet::default_instance_->InitAsDefaultInstance();
  Packet_Destination::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_packet_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_packet_2eproto {
  StaticDescriptorInitializer_packet_2eproto() {
    protobuf_AddDesc_packet_2eproto();
  }
} static_descriptor_initializer_packet_2eproto_;


// ===================================================================

const ::google::protobuf::EnumDescriptor* Packet_PacketType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Packet_PacketType_descriptor_;
}
bool Packet_PacketType_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const Packet_PacketType Packet::REQUEST;
const Packet_PacketType Packet::REPLY;
const Packet_PacketType Packet::ERR;
const Packet_PacketType Packet::PING;
const Packet_PacketType Packet::ACK;
const Packet_PacketType Packet::CONNECT;
const Packet_PacketType Packet::PacketType_MIN;
const Packet_PacketType Packet::PacketType_MAX;
const int Packet::PacketType_ARRAYSIZE;
#endif  // _MSC_VER
const ::google::protobuf::EnumDescriptor* Packet_Destination_Type_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Packet_Destination_Type_descriptor_;
}
bool Packet_Destination_Type_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const Packet_Destination_Type Packet_Destination::ONE;
const Packet_Destination_Type Packet_Destination::RANGE;
const Packet_Destination_Type Packet_Destination::ALL;
const Packet_Destination_Type Packet_Destination::Type_MIN;
const Packet_Destination_Type Packet_Destination::Type_MAX;
const int Packet_Destination::Type_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int Packet_Destination::kTypeFieldNumber;
const int Packet_Destination::kFromFieldNumber;
const int Packet_Destination::kToFieldNumber;
const int Packet_Destination::kFilterFieldNumber;
#endif  // !_MSC_VER

Packet_Destination::Packet_Destination()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Packet_Destination::InitAsDefaultInstance() {
}

Packet_Destination::Packet_Destination(const Packet_Destination& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Packet_Destination::SharedCtor() {
  _cached_size_ = 0;
  type_ = 1;
  from_ = 0u;
  to_ = 0;
  filter_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Packet_Destination::~Packet_Destination() {
  SharedDtor();
}

void Packet_Destination::SharedDtor() {
  if (filter_ != &::google::protobuf::internal::kEmptyString) {
    delete filter_;
  }
  if (this != default_instance_) {
  }
}

void Packet_Destination::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Packet_Destination::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Packet_Destination_descriptor_;
}

const Packet_Destination& Packet_Destination::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_packet_2eproto();  return *default_instance_;
}

Packet_Destination* Packet_Destination::default_instance_ = NULL;

Packet_Destination* Packet_Destination::New() const {
  return new Packet_Destination;
}

void Packet_Destination::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type_ = 1;
    from_ = 0u;
    to_ = 0;
    if (has_filter()) {
      if (filter_ != &::google::protobuf::internal::kEmptyString) {
        filter_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Packet_Destination::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .packets.Packet.Destination.Type type = 1 [default = ONE];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::packets::Packet_Destination_Type_IsValid(value)) {
            set_type(static_cast< ::packets::Packet_Destination_Type >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_from;
        break;
      }
      
      // optional uint32 from = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_from:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &from_)));
          set_has_from();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_to;
        break;
      }
      
      // optional int32 to = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_to:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &to_)));
          set_has_to();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_filter;
        break;
      }
      
      // optional bytes filter = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_filter:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_filter()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Packet_Destination::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .packets.Packet.Destination.Type type = 1 [default = ONE];
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }
  
  // optional uint32 from = 2;
  if (has_from()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->from(), output);
  }
  
  // optional int32 to = 3;
  if (has_to()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->to(), output);
  }
  
  // optional bytes filter = 4;
  if (has_filter()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      4, this->filter(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Packet_Destination::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .packets.Packet.Destination.Type type = 1 [default = ONE];
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }
  
  // optional uint32 from = 2;
  if (has_from()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(2, this->from(), target);
  }
  
  // optional int32 to = 3;
  if (has_to()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->to(), target);
  }
  
  // optional bytes filter = 4;
  if (has_filter()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        4, this->filter(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Packet_Destination::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .packets.Packet.Destination.Type type = 1 [default = ONE];
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }
    
    // optional uint32 from = 2;
    if (has_from()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->from());
    }
    
    // optional int32 to = 3;
    if (has_to()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->to());
    }
    
    // optional bytes filter = 4;
    if (has_filter()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->filter());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Packet_Destination::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Packet_Destination* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Packet_Destination*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Packet_Destination::MergeFrom(const Packet_Destination& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_from()) {
      set_from(from.from());
    }
    if (from.has_to()) {
      set_to(from.to());
    }
    if (from.has_filter()) {
      set_filter(from.filter());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Packet_Destination::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Packet_Destination::CopyFrom(const Packet_Destination& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Packet_Destination::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void Packet_Destination::Swap(Packet_Destination* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(from_, other->from_);
    std::swap(to_, other->to_);
    std::swap(filter_, other->filter_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Packet_Destination::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Packet_Destination_descriptor_;
  metadata.reflection = Packet_Destination_reflection_;
  return metadata;
}


// -------------------------------------------------------------------

#ifndef _MSC_VER
const int Packet::kTypeFieldNumber;
const int Packet::kJobFieldNumber;
const int Packet::kDestinationFieldNumber;
const int Packet::kGuidFieldNumber;
const int Packet::kTimeoutFieldNumber;
const int Packet::kFromFieldNumber;
const int Packet::kEpFieldNumber;
const int Packet::kTraceFieldNumber;
#endif  // !_MSC_VER

Packet::Packet()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Packet::InitAsDefaultInstance() {
  job_ = const_cast< ::jobs::Job*>(&::jobs::Job::default_instance());
  destination_ = const_cast< ::packets::Packet_Destination*>(&::packets::Packet_Destination::default_instance());
}

Packet::Packet(const Packet& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Packet::SharedCtor() {
  _cached_size_ = 0;
  type_ = 1;
  job_ = NULL;
  destination_ = NULL;
  guid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  timeout_ = 0u;
  from_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ep_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  trace_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Packet::~Packet() {
  SharedDtor();
}

void Packet::SharedDtor() {
  if (guid_ != &::google::protobuf::internal::kEmptyString) {
    delete guid_;
  }
  if (from_ != &::google::protobuf::internal::kEmptyString) {
    delete from_;
  }
  if (ep_ != &::google::protobuf::internal::kEmptyString) {
    delete ep_;
  }
  if (trace_ != &::google::protobuf::internal::kEmptyString) {
    delete trace_;
  }
  if (this != default_instance_) {
    delete job_;
    delete destination_;
  }
}

void Packet::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Packet::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Packet_descriptor_;
}

const Packet& Packet::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_packet_2eproto();  return *default_instance_;
}

Packet* Packet::default_instance_ = NULL;

Packet* Packet::New() const {
  return new Packet;
}

void Packet::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type_ = 1;
    if (has_job()) {
      if (job_ != NULL) job_->::jobs::Job::Clear();
    }
    if (has_destination()) {
      if (destination_ != NULL) destination_->::packets::Packet_Destination::Clear();
    }
    if (has_guid()) {
      if (guid_ != &::google::protobuf::internal::kEmptyString) {
        guid_->clear();
      }
    }
    timeout_ = 0u;
    if (has_from()) {
      if (from_ != &::google::protobuf::internal::kEmptyString) {
        from_->clear();
      }
    }
    if (has_ep()) {
      if (ep_ != &::google::protobuf::internal::kEmptyString) {
        ep_->clear();
      }
    }
    if (has_trace()) {
      if (trace_ != &::google::protobuf::internal::kEmptyString) {
        trace_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Packet::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .packets.Packet.PacketType type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::packets::Packet_PacketType_IsValid(value)) {
            set_type(static_cast< ::packets::Packet_PacketType >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_job;
        break;
      }
      
      // optional .jobs.Job job = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_job:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_job()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_destination;
        break;
      }
      
      // optional .packets.Packet.Destination destination = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_destination:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_destination()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_guid;
        break;
      }
      
      // required bytes guid = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_guid:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_guid()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(40)) goto parse_timeout;
        break;
      }
      
      // optional uint32 timeout = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_timeout:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &timeout_)));
          set_has_timeout();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_from;
        break;
      }
      
      // required bytes from = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_from:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_from()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(58)) goto parse_ep;
        break;
      }
      
      // optional bytes ep = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_ep:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_ep()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(66)) goto parse_trace;
        break;
      }
      
      // optional bytes trace = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_trace:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_trace()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Packet::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .packets.Packet.PacketType type = 1;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }
  
  // optional .jobs.Job job = 2;
  if (has_job()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->job(), output);
  }
  
  // optional .packets.Packet.Destination destination = 3;
  if (has_destination()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->destination(), output);
  }
  
  // required bytes guid = 4;
  if (has_guid()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      4, this->guid(), output);
  }
  
  // optional uint32 timeout = 5;
  if (has_timeout()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(5, this->timeout(), output);
  }
  
  // required bytes from = 6;
  if (has_from()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      6, this->from(), output);
  }
  
  // optional bytes ep = 7;
  if (has_ep()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      7, this->ep(), output);
  }
  
  // optional bytes trace = 8;
  if (has_trace()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      8, this->trace(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Packet::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .packets.Packet.PacketType type = 1;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }
  
  // optional .jobs.Job job = 2;
  if (has_job()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->job(), target);
  }
  
  // optional .packets.Packet.Destination destination = 3;
  if (has_destination()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->destination(), target);
  }
  
  // required bytes guid = 4;
  if (has_guid()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        4, this->guid(), target);
  }
  
  // optional uint32 timeout = 5;
  if (has_timeout()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(5, this->timeout(), target);
  }
  
  // required bytes from = 6;
  if (has_from()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        6, this->from(), target);
  }
  
  // optional bytes ep = 7;
  if (has_ep()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        7, this->ep(), target);
  }
  
  // optional bytes trace = 8;
  if (has_trace()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        8, this->trace(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Packet::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .packets.Packet.PacketType type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }
    
    // optional .jobs.Job job = 2;
    if (has_job()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->job());
    }
    
    // optional .packets.Packet.Destination destination = 3;
    if (has_destination()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->destination());
    }
    
    // required bytes guid = 4;
    if (has_guid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->guid());
    }
    
    // optional uint32 timeout = 5;
    if (has_timeout()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->timeout());
    }
    
    // required bytes from = 6;
    if (has_from()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->from());
    }
    
    // optional bytes ep = 7;
    if (has_ep()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->ep());
    }
    
    // optional bytes trace = 8;
    if (has_trace()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->trace());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Packet::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Packet* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Packet*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Packet::MergeFrom(const Packet& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_job()) {
      mutable_job()->::jobs::Job::MergeFrom(from.job());
    }
    if (from.has_destination()) {
      mutable_destination()->::packets::Packet_Destination::MergeFrom(from.destination());
    }
    if (from.has_guid()) {
      set_guid(from.guid());
    }
    if (from.has_timeout()) {
      set_timeout(from.timeout());
    }
    if (from.has_from()) {
      set_from(from.from());
    }
    if (from.has_ep()) {
      set_ep(from.ep());
    }
    if (from.has_trace()) {
      set_trace(from.trace());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Packet::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Packet::CopyFrom(const Packet& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Packet::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000029) != 0x00000029) return false;
  
  if (has_destination()) {
    if (!this->destination().IsInitialized()) return false;
  }
  return true;
}

void Packet::Swap(Packet* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(job_, other->job_);
    std::swap(destination_, other->destination_);
    std::swap(guid_, other->guid_);
    std::swap(timeout_, other->timeout_);
    std::swap(from_, other->from_);
    std::swap(ep_, other->ep_);
    std::swap(trace_, other->trace_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Packet::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Packet_descriptor_;
  metadata.reflection = Packet_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace packets

// @@protoc_insertion_point(global_scope)

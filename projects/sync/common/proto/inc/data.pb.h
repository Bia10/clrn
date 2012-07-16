// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: data.proto

#ifndef PROTOBUF_data_2eproto__INCLUDED
#define PROTOBUF_data_2eproto__INCLUDED

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

namespace data {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_data_2eproto();
void protobuf_AssignDesc_data_2eproto();
void protobuf_ShutdownFile_data_2eproto();

class Table;
class Table_Row;

enum Table_Action {
  Table_Action_Insert = 0,
  Table_Action_Delete = 1
};
bool Table_Action_IsValid(int value);
const Table_Action Table_Action_Action_MIN = Table_Action_Insert;
const Table_Action Table_Action_Action_MAX = Table_Action_Delete;
const int Table_Action_Action_ARRAYSIZE = Table_Action_Action_MAX + 1;

const ::google::protobuf::EnumDescriptor* Table_Action_descriptor();
inline const ::std::string& Table_Action_Name(Table_Action value) {
  return ::google::protobuf::internal::NameOfEnum(
    Table_Action_descriptor(), value);
}
inline bool Table_Action_Parse(
    const ::std::string& name, Table_Action* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Table_Action>(
    Table_Action_descriptor(), name, value);
}
enum Table_Id {
  Table_Id_None = 0,
  Table_Id_Modules = 1,
  Table_Id_Settings = 2,
  Table_Id_Hosts = 3,
  Table_Id_HostMap = 4,
  Table_Id_HostMapEvent = 5,
  Table_Id_HostStatusEvent = 6
};
bool Table_Id_IsValid(int value);
const Table_Id Table_Id_Id_MIN = Table_Id_None;
const Table_Id Table_Id_Id_MAX = Table_Id_HostStatusEvent;
const int Table_Id_Id_ARRAYSIZE = Table_Id_Id_MAX + 1;

const ::google::protobuf::EnumDescriptor* Table_Id_descriptor();
inline const ::std::string& Table_Id_Name(Table_Id value) {
  return ::google::protobuf::internal::NameOfEnum(
    Table_Id_descriptor(), value);
}
inline bool Table_Id_Parse(
    const ::std::string& name, Table_Id* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Table_Id>(
    Table_Id_descriptor(), name, value);
}
// ===================================================================

class Table_Row : public ::google::protobuf::Message {
 public:
  Table_Row();
  virtual ~Table_Row();
  
  Table_Row(const Table_Row& from);
  
  inline Table_Row& operator=(const Table_Row& from) {
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
  static const Table_Row& default_instance();
  
  void Swap(Table_Row* other);
  
  // implements Message ----------------------------------------------
  
  Table_Row* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Table_Row& from);
  void MergeFrom(const Table_Row& from);
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
  
  // repeated bytes data = 1;
  inline int data_size() const;
  inline void clear_data();
  static const int kDataFieldNumber = 1;
  inline const ::std::string& data(int index) const;
  inline ::std::string* mutable_data(int index);
  inline void set_data(int index, const ::std::string& value);
  inline void set_data(int index, const char* value);
  inline void set_data(int index, const void* value, size_t size);
  inline ::std::string* add_data();
  inline void add_data(const ::std::string& value);
  inline void add_data(const char* value);
  inline void add_data(const void* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& data() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_data();
  
  // @@protoc_insertion_point(class_scope:data.Table.Row)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::std::string> data_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_data_2eproto();
  friend void protobuf_AssignDesc_data_2eproto();
  friend void protobuf_ShutdownFile_data_2eproto();
  
  void InitAsDefaultInstance();
  static Table_Row* default_instance_;
};
// -------------------------------------------------------------------

class Table : public ::google::protobuf::Message {
 public:
  Table();
  virtual ~Table();
  
  Table(const Table& from);
  
  inline Table& operator=(const Table& from) {
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
  static const Table& default_instance();
  
  void Swap(Table* other);
  
  // implements Message ----------------------------------------------
  
  Table* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Table& from);
  void MergeFrom(const Table& from);
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
  
  typedef Table_Row Row;
  
  typedef Table_Action Action;
  static const Action Insert = Table_Action_Insert;
  static const Action Delete = Table_Action_Delete;
  static inline bool Action_IsValid(int value) {
    return Table_Action_IsValid(value);
  }
  static const Action Action_MIN =
    Table_Action_Action_MIN;
  static const Action Action_MAX =
    Table_Action_Action_MAX;
  static const int Action_ARRAYSIZE =
    Table_Action_Action_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Action_descriptor() {
    return Table_Action_descriptor();
  }
  static inline const ::std::string& Action_Name(Action value) {
    return Table_Action_Name(value);
  }
  static inline bool Action_Parse(const ::std::string& name,
      Action* value) {
    return Table_Action_Parse(name, value);
  }
  
  typedef Table_Id Id;
  static const Id None = Table_Id_None;
  static const Id Modules = Table_Id_Modules;
  static const Id Settings = Table_Id_Settings;
  static const Id Hosts = Table_Id_Hosts;
  static const Id HostMap = Table_Id_HostMap;
  static const Id HostMapEvent = Table_Id_HostMapEvent;
  static const Id HostStatusEvent = Table_Id_HostStatusEvent;
  static inline bool Id_IsValid(int value) {
    return Table_Id_IsValid(value);
  }
  static const Id Id_MIN =
    Table_Id_Id_MIN;
  static const Id Id_MAX =
    Table_Id_Id_MAX;
  static const int Id_ARRAYSIZE =
    Table_Id_Id_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Id_descriptor() {
    return Table_Id_descriptor();
  }
  static inline const ::std::string& Id_Name(Id value) {
    return Table_Id_Name(value);
  }
  static inline bool Id_Parse(const ::std::string& name,
      Id* value) {
    return Table_Id_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // repeated .data.Table.Row rows = 1;
  inline int rows_size() const;
  inline void clear_rows();
  static const int kRowsFieldNumber = 1;
  inline const ::data::Table_Row& rows(int index) const;
  inline ::data::Table_Row* mutable_rows(int index);
  inline ::data::Table_Row* add_rows();
  inline const ::google::protobuf::RepeatedPtrField< ::data::Table_Row >&
      rows() const;
  inline ::google::protobuf::RepeatedPtrField< ::data::Table_Row >*
      mutable_rows();
  
  // optional .data.Table.Id id = 2;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 2;
  inline ::data::Table_Id id() const;
  inline void set_id(::data::Table_Id value);
  
  // optional .data.Table.Action action = 3 [default = Insert];
  inline bool has_action() const;
  inline void clear_action();
  static const int kActionFieldNumber = 3;
  inline ::data::Table_Action action() const;
  inline void set_action(::data::Table_Action value);
  
  // @@protoc_insertion_point(class_scope:data.Table)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_action();
  inline void clear_has_action();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::data::Table_Row > rows_;
  int id_;
  int action_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  friend void  protobuf_AddDesc_data_2eproto();
  friend void protobuf_AssignDesc_data_2eproto();
  friend void protobuf_ShutdownFile_data_2eproto();
  
  void InitAsDefaultInstance();
  static Table* default_instance_;
};
// ===================================================================


// ===================================================================

// Table_Row

// repeated bytes data = 1;
inline int Table_Row::data_size() const {
  return data_.size();
}
inline void Table_Row::clear_data() {
  data_.Clear();
}
inline const ::std::string& Table_Row::data(int index) const {
  return data_.Get(index);
}
inline ::std::string* Table_Row::mutable_data(int index) {
  return data_.Mutable(index);
}
inline void Table_Row::set_data(int index, const ::std::string& value) {
  data_.Mutable(index)->assign(value);
}
inline void Table_Row::set_data(int index, const char* value) {
  data_.Mutable(index)->assign(value);
}
inline void Table_Row::set_data(int index, const void* value, size_t size) {
  data_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Table_Row::add_data() {
  return data_.Add();
}
inline void Table_Row::add_data(const ::std::string& value) {
  data_.Add()->assign(value);
}
inline void Table_Row::add_data(const char* value) {
  data_.Add()->assign(value);
}
inline void Table_Row::add_data(const void* value, size_t size) {
  data_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Table_Row::data() const {
  return data_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Table_Row::mutable_data() {
  return &data_;
}

// -------------------------------------------------------------------

// Table

// repeated .data.Table.Row rows = 1;
inline int Table::rows_size() const {
  return rows_.size();
}
inline void Table::clear_rows() {
  rows_.Clear();
}
inline const ::data::Table_Row& Table::rows(int index) const {
  return rows_.Get(index);
}
inline ::data::Table_Row* Table::mutable_rows(int index) {
  return rows_.Mutable(index);
}
inline ::data::Table_Row* Table::add_rows() {
  return rows_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::data::Table_Row >&
Table::rows() const {
  return rows_;
}
inline ::google::protobuf::RepeatedPtrField< ::data::Table_Row >*
Table::mutable_rows() {
  return &rows_;
}

// optional .data.Table.Id id = 2;
inline bool Table::has_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Table::set_has_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Table::clear_has_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Table::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::data::Table_Id Table::id() const {
  return static_cast< ::data::Table_Id >(id_);
}
inline void Table::set_id(::data::Table_Id value) {
  GOOGLE_DCHECK(::data::Table_Id_IsValid(value));
  set_has_id();
  id_ = value;
}

// optional .data.Table.Action action = 3 [default = Insert];
inline bool Table::has_action() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Table::set_has_action() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Table::clear_has_action() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Table::clear_action() {
  action_ = 0;
  clear_has_action();
}
inline ::data::Table_Action Table::action() const {
  return static_cast< ::data::Table_Action >(action_);
}
inline void Table::set_action(::data::Table_Action value) {
  GOOGLE_DCHECK(::data::Table_Action_IsValid(value));
  set_has_action();
  action_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace data

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::data::Table_Action>() {
  return ::data::Table_Action_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::data::Table_Id>() {
  return ::data::Table_Id_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_data_2eproto__INCLUDED
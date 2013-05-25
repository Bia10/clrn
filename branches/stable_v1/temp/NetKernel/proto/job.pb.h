// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: job.proto

#ifndef PROTOBUF_job_2eproto__INCLUDED
#define PROTOBUF_job_2eproto__INCLUDED

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
#include "data.pb.h"
// @@protoc_insertion_point(includes)

namespace jobs {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_job_2eproto();
void protobuf_AssignDesc_job_2eproto();
void protobuf_ShutdownFile_job_2eproto();

class Job;

enum Job_JobCategory {
  Job_JobCategory_INFO = 0,
  Job_JobCategory_SETTING = 1,
  Job_JobCategory_DATABASE = 2
};
bool Job_JobCategory_IsValid(int value);
const Job_JobCategory Job_JobCategory_JobCategory_MIN = Job_JobCategory_INFO;
const Job_JobCategory Job_JobCategory_JobCategory_MAX = Job_JobCategory_DATABASE;
const int Job_JobCategory_JobCategory_ARRAYSIZE = Job_JobCategory_JobCategory_MAX + 1;

const ::google::protobuf::EnumDescriptor* Job_JobCategory_descriptor();
inline const ::std::string& Job_JobCategory_Name(Job_JobCategory value) {
  return ::google::protobuf::internal::NameOfEnum(
    Job_JobCategory_descriptor(), value);
}
inline bool Job_JobCategory_Parse(
    const ::std::string& name, Job_JobCategory* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Job_JobCategory>(
    Job_JobCategory_descriptor(), name, value);
}
enum Job_JobType {
  Job_JobType_GET_MODULES = 0,
  Job_JobType_GET_SETTINGS = 1,
  Job_JobType_SET_SETTINGS = 2,
  Job_JobType_SCRIPT = 3
};
bool Job_JobType_IsValid(int value);
const Job_JobType Job_JobType_JobType_MIN = Job_JobType_GET_MODULES;
const Job_JobType Job_JobType_JobType_MAX = Job_JobType_SCRIPT;
const int Job_JobType_JobType_ARRAYSIZE = Job_JobType_JobType_MAX + 1;

const ::google::protobuf::EnumDescriptor* Job_JobType_descriptor();
inline const ::std::string& Job_JobType_Name(Job_JobType value) {
  return ::google::protobuf::internal::NameOfEnum(
    Job_JobType_descriptor(), value);
}
inline bool Job_JobType_Parse(
    const ::std::string& name, Job_JobType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Job_JobType>(
    Job_JobType_descriptor(), name, value);
}
// ===================================================================

class Job : public ::google::protobuf::Message {
 public:
  Job();
  virtual ~Job();
  
  Job(const Job& from);
  
  inline Job& operator=(const Job& from) {
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
  static const Job& default_instance();
  
  void Swap(Job* other);
  
  // implements Message ----------------------------------------------
  
  Job* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Job& from);
  void MergeFrom(const Job& from);
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
  
  typedef Job_JobCategory JobCategory;
  static const JobCategory INFO = Job_JobCategory_INFO;
  static const JobCategory SETTING = Job_JobCategory_SETTING;
  static const JobCategory DATABASE = Job_JobCategory_DATABASE;
  static inline bool JobCategory_IsValid(int value) {
    return Job_JobCategory_IsValid(value);
  }
  static const JobCategory JobCategory_MIN =
    Job_JobCategory_JobCategory_MIN;
  static const JobCategory JobCategory_MAX =
    Job_JobCategory_JobCategory_MAX;
  static const int JobCategory_ARRAYSIZE =
    Job_JobCategory_JobCategory_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  JobCategory_descriptor() {
    return Job_JobCategory_descriptor();
  }
  static inline const ::std::string& JobCategory_Name(JobCategory value) {
    return Job_JobCategory_Name(value);
  }
  static inline bool JobCategory_Parse(const ::std::string& name,
      JobCategory* value) {
    return Job_JobCategory_Parse(name, value);
  }
  
  typedef Job_JobType JobType;
  static const JobType GET_MODULES = Job_JobType_GET_MODULES;
  static const JobType GET_SETTINGS = Job_JobType_GET_SETTINGS;
  static const JobType SET_SETTINGS = Job_JobType_SET_SETTINGS;
  static const JobType SCRIPT = Job_JobType_SCRIPT;
  static inline bool JobType_IsValid(int value) {
    return Job_JobType_IsValid(value);
  }
  static const JobType JobType_MIN =
    Job_JobType_JobType_MIN;
  static const JobType JobType_MAX =
    Job_JobType_JobType_MAX;
  static const int JobType_ARRAYSIZE =
    Job_JobType_JobType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  JobType_descriptor() {
    return Job_JobType_descriptor();
  }
  static inline const ::std::string& JobType_Name(JobType value) {
    return Job_JobType_Name(value);
  }
  static inline bool JobType_Parse(const ::std::string& name,
      JobType* value) {
    return Job_JobType_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required int64 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int64 id() const;
  inline void set_id(::google::protobuf::int64 value);
  
  // required .jobs.Job.JobCategory category = 2;
  inline bool has_category() const;
  inline void clear_category();
  static const int kCategoryFieldNumber = 2;
  inline ::jobs::Job_JobCategory category() const;
  inline void set_category(::jobs::Job_JobCategory value);
  
  // required .jobs.Job.JobType type = 3;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 3;
  inline ::jobs::Job_JobType type() const;
  inline void set_type(::jobs::Job_JobType value);
  
  // repeated .data.Table params = 4;
  inline int params_size() const;
  inline void clear_params();
  static const int kParamsFieldNumber = 4;
  inline const ::data::Table& params(int index) const;
  inline ::data::Table* mutable_params(int index);
  inline ::data::Table* add_params();
  inline const ::google::protobuf::RepeatedPtrField< ::data::Table >&
      params() const;
  inline ::google::protobuf::RepeatedPtrField< ::data::Table >*
      mutable_params();
  
  // repeated .data.Table result = 5;
  inline int result_size() const;
  inline void clear_result();
  static const int kResultFieldNumber = 5;
  inline const ::data::Table& result(int index) const;
  inline ::data::Table* mutable_result(int index);
  inline ::data::Table* add_result();
  inline const ::google::protobuf::RepeatedPtrField< ::data::Table >&
      result() const;
  inline ::google::protobuf::RepeatedPtrField< ::data::Table >*
      mutable_result();
  
  // @@protoc_insertion_point(class_scope:jobs.Job)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_category();
  inline void clear_has_category();
  inline void set_has_type();
  inline void clear_has_type();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::int64 id_;
  int category_;
  int type_;
  ::google::protobuf::RepeatedPtrField< ::data::Table > params_;
  ::google::protobuf::RepeatedPtrField< ::data::Table > result_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  friend void  protobuf_AddDesc_job_2eproto();
  friend void protobuf_AssignDesc_job_2eproto();
  friend void protobuf_ShutdownFile_job_2eproto();
  
  void InitAsDefaultInstance();
  static Job* default_instance_;
};
// ===================================================================


// ===================================================================

// Job

// required int64 id = 1;
inline bool Job::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Job::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Job::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Job::clear_id() {
  id_ = GOOGLE_LONGLONG(0);
  clear_has_id();
}
inline ::google::protobuf::int64 Job::id() const {
  return id_;
}
inline void Job::set_id(::google::protobuf::int64 value) {
  set_has_id();
  id_ = value;
}

// required .jobs.Job.JobCategory category = 2;
inline bool Job::has_category() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Job::set_has_category() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Job::clear_has_category() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Job::clear_category() {
  category_ = 0;
  clear_has_category();
}
inline ::jobs::Job_JobCategory Job::category() const {
  return static_cast< ::jobs::Job_JobCategory >(category_);
}
inline void Job::set_category(::jobs::Job_JobCategory value) {
  GOOGLE_DCHECK(::jobs::Job_JobCategory_IsValid(value));
  set_has_category();
  category_ = value;
}

// required .jobs.Job.JobType type = 3;
inline bool Job::has_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Job::set_has_type() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Job::clear_has_type() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Job::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::jobs::Job_JobType Job::type() const {
  return static_cast< ::jobs::Job_JobType >(type_);
}
inline void Job::set_type(::jobs::Job_JobType value) {
  GOOGLE_DCHECK(::jobs::Job_JobType_IsValid(value));
  set_has_type();
  type_ = value;
}

// repeated .data.Table params = 4;
inline int Job::params_size() const {
  return params_.size();
}
inline void Job::clear_params() {
  params_.Clear();
}
inline const ::data::Table& Job::params(int index) const {
  return params_.Get(index);
}
inline ::data::Table* Job::mutable_params(int index) {
  return params_.Mutable(index);
}
inline ::data::Table* Job::add_params() {
  return params_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::data::Table >&
Job::params() const {
  return params_;
}
inline ::google::protobuf::RepeatedPtrField< ::data::Table >*
Job::mutable_params() {
  return &params_;
}

// repeated .data.Table result = 5;
inline int Job::result_size() const {
  return result_.size();
}
inline void Job::clear_result() {
  result_.Clear();
}
inline const ::data::Table& Job::result(int index) const {
  return result_.Get(index);
}
inline ::data::Table* Job::mutable_result(int index) {
  return result_.Mutable(index);
}
inline ::data::Table* Job::add_result() {
  return result_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::data::Table >&
Job::result() const {
  return result_;
}
inline ::google::protobuf::RepeatedPtrField< ::data::Table >*
Job::mutable_result() {
  return &result_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace jobs

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::jobs::Job_JobCategory>() {
  return ::jobs::Job_JobCategory_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::jobs::Job_JobType>() {
  return ::jobs::Job_JobType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_job_2eproto__INCLUDED

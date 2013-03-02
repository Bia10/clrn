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
// @@protoc_insertion_point(includes)

namespace net {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_packet_2eproto();
void protobuf_AssignDesc_packet_2eproto();
void protobuf_ShutdownFile_packet_2eproto();

class Packet;
class Packet_Player;
class Packet_Table;
class Packet_Action;
class Packet_Phase;
class Reply;

// ===================================================================

class Packet_Player : public ::google::protobuf::Message {
 public:
  Packet_Player();
  virtual ~Packet_Player();
  
  Packet_Player(const Packet_Player& from);
  
  inline Packet_Player& operator=(const Packet_Player& from) {
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
  static const Packet_Player& default_instance();
  
  void Swap(Packet_Player* other);
  
  // implements Message ----------------------------------------------
  
  Packet_Player* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet_Player& from);
  void MergeFrom(const Packet_Player& from);
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
  
  // required string Name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  
  // required uint32 Stack = 2;
  inline bool has_stack() const;
  inline void clear_stack();
  static const int kStackFieldNumber = 2;
  inline ::google::protobuf::uint32 stack() const;
  inline void set_stack(::google::protobuf::uint32 value);
  
  // required uint32 Bet = 3;
  inline bool has_bet() const;
  inline void clear_bet();
  static const int kBetFieldNumber = 3;
  inline ::google::protobuf::uint32 bet() const;
  inline void set_bet(::google::protobuf::uint32 value);
  
  // repeated uint32 Cards = 4;
  inline int cards_size() const;
  inline void clear_cards();
  static const int kCardsFieldNumber = 4;
  inline ::google::protobuf::uint32 cards(int index) const;
  inline void set_cards(int index, ::google::protobuf::uint32 value);
  inline void add_cards(::google::protobuf::uint32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
      cards() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
      mutable_cards();
  
  // @@protoc_insertion_point(class_scope:net.Packet.Player)
 private:
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_stack();
  inline void clear_has_stack();
  inline void set_has_bet();
  inline void clear_has_bet();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* name_;
  ::google::protobuf::uint32 stack_;
  ::google::protobuf::uint32 bet_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint32 > cards_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet_Player* default_instance_;
};
// -------------------------------------------------------------------

class Packet_Table : public ::google::protobuf::Message {
 public:
  Packet_Table();
  virtual ~Packet_Table();
  
  Packet_Table(const Packet_Table& from);
  
  inline Packet_Table& operator=(const Packet_Table& from) {
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
  static const Packet_Table& default_instance();
  
  void Swap(Packet_Table* other);
  
  // implements Message ----------------------------------------------
  
  Packet_Table* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet_Table& from);
  void MergeFrom(const Packet_Table& from);
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
  
  // repeated .net.Packet.Player Players = 1;
  inline int players_size() const;
  inline void clear_players();
  static const int kPlayersFieldNumber = 1;
  inline const ::net::Packet_Player& players(int index) const;
  inline ::net::Packet_Player* mutable_players(int index);
  inline ::net::Packet_Player* add_players();
  inline const ::google::protobuf::RepeatedPtrField< ::net::Packet_Player >&
      players() const;
  inline ::google::protobuf::RepeatedPtrField< ::net::Packet_Player >*
      mutable_players();
  
  // required uint32 Button = 2;
  inline bool has_button() const;
  inline void clear_button();
  static const int kButtonFieldNumber = 2;
  inline ::google::protobuf::uint32 button() const;
  inline void set_button(::google::protobuf::uint32 value);
  
  // repeated uint32 Cards = 3;
  inline int cards_size() const;
  inline void clear_cards();
  static const int kCardsFieldNumber = 3;
  inline ::google::protobuf::uint32 cards(int index) const;
  inline void set_cards(int index, ::google::protobuf::uint32 value);
  inline void add_cards(::google::protobuf::uint32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
      cards() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
      mutable_cards();
  
  // @@protoc_insertion_point(class_scope:net.Packet.Table)
 private:
  inline void set_has_button();
  inline void clear_has_button();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::net::Packet_Player > players_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint32 > cards_;
  ::google::protobuf::uint32 button_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet_Table* default_instance_;
};
// -------------------------------------------------------------------

class Packet_Action : public ::google::protobuf::Message {
 public:
  Packet_Action();
  virtual ~Packet_Action();
  
  Packet_Action(const Packet_Action& from);
  
  inline Packet_Action& operator=(const Packet_Action& from) {
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
  static const Packet_Action& default_instance();
  
  void Swap(Packet_Action* other);
  
  // implements Message ----------------------------------------------
  
  Packet_Action* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet_Action& from);
  void MergeFrom(const Packet_Action& from);
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
  
  // required uint32 Player = 1;
  inline bool has_player() const;
  inline void clear_player();
  static const int kPlayerFieldNumber = 1;
  inline ::google::protobuf::uint32 player() const;
  inline void set_player(::google::protobuf::uint32 value);
  
  // required uint32 Id = 2;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 2;
  inline ::google::protobuf::uint32 id() const;
  inline void set_id(::google::protobuf::uint32 value);
  
  // optional uint32 Amount = 3;
  inline bool has_amount() const;
  inline void clear_amount();
  static const int kAmountFieldNumber = 3;
  inline ::google::protobuf::uint32 amount() const;
  inline void set_amount(::google::protobuf::uint32 value);
  
  // @@protoc_insertion_point(class_scope:net.Packet.Action)
 private:
  inline void set_has_player();
  inline void clear_has_player();
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_amount();
  inline void clear_has_amount();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::uint32 player_;
  ::google::protobuf::uint32 id_;
  ::google::protobuf::uint32 amount_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet_Action* default_instance_;
};
// -------------------------------------------------------------------

class Packet_Phase : public ::google::protobuf::Message {
 public:
  Packet_Phase();
  virtual ~Packet_Phase();
  
  Packet_Phase(const Packet_Phase& from);
  
  inline Packet_Phase& operator=(const Packet_Phase& from) {
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
  static const Packet_Phase& default_instance();
  
  void Swap(Packet_Phase* other);
  
  // implements Message ----------------------------------------------
  
  Packet_Phase* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet_Phase& from);
  void MergeFrom(const Packet_Phase& from);
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
  
  // repeated .net.Packet.Action Actions = 1;
  inline int actions_size() const;
  inline void clear_actions();
  static const int kActionsFieldNumber = 1;
  inline const ::net::Packet_Action& actions(int index) const;
  inline ::net::Packet_Action* mutable_actions(int index);
  inline ::net::Packet_Action* add_actions();
  inline const ::google::protobuf::RepeatedPtrField< ::net::Packet_Action >&
      actions() const;
  inline ::google::protobuf::RepeatedPtrField< ::net::Packet_Action >*
      mutable_actions();
  
  // @@protoc_insertion_point(class_scope:net.Packet.Phase)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::net::Packet_Action > actions_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet_Phase* default_instance_;
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
  
  typedef Packet_Player Player;
  typedef Packet_Table Table;
  typedef Packet_Action Action;
  typedef Packet_Phase Phase;
  
  // accessors -------------------------------------------------------
  
  // required .net.Packet.Table Info = 1;
  inline bool has_info() const;
  inline void clear_info();
  static const int kInfoFieldNumber = 1;
  inline const ::net::Packet_Table& info() const;
  inline ::net::Packet_Table* mutable_info();
  inline ::net::Packet_Table* release_info();
  
  // repeated .net.Packet.Phase Phases = 2;
  inline int phases_size() const;
  inline void clear_phases();
  static const int kPhasesFieldNumber = 2;
  inline const ::net::Packet_Phase& phases(int index) const;
  inline ::net::Packet_Phase* mutable_phases(int index);
  inline ::net::Packet_Phase* add_phases();
  inline const ::google::protobuf::RepeatedPtrField< ::net::Packet_Phase >&
      phases() const;
  inline ::google::protobuf::RepeatedPtrField< ::net::Packet_Phase >*
      mutable_phases();
  
  // @@protoc_insertion_point(class_scope:net.Packet)
 private:
  inline void set_has_info();
  inline void clear_has_info();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::net::Packet_Table* info_;
  ::google::protobuf::RepeatedPtrField< ::net::Packet_Phase > phases_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Packet* default_instance_;
};
// -------------------------------------------------------------------

class Reply : public ::google::protobuf::Message {
 public:
  Reply();
  virtual ~Reply();
  
  Reply(const Reply& from);
  
  inline Reply& operator=(const Reply& from) {
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
  static const Reply& default_instance();
  
  void Swap(Reply* other);
  
  // implements Message ----------------------------------------------
  
  Reply* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Reply& from);
  void MergeFrom(const Reply& from);
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
  
  // optional string Error = 1;
  inline bool has_error() const;
  inline void clear_error();
  static const int kErrorFieldNumber = 1;
  inline const ::std::string& error() const;
  inline void set_error(const ::std::string& value);
  inline void set_error(const char* value);
  inline void set_error(const char* value, size_t size);
  inline ::std::string* mutable_error();
  inline ::std::string* release_error();
  
  // required uint32 Action = 2;
  inline bool has_action() const;
  inline void clear_action();
  static const int kActionFieldNumber = 2;
  inline ::google::protobuf::uint32 action() const;
  inline void set_action(::google::protobuf::uint32 value);
  
  // required uint32 Amount = 3;
  inline bool has_amount() const;
  inline void clear_amount();
  static const int kAmountFieldNumber = 3;
  inline ::google::protobuf::uint32 amount() const;
  inline void set_amount(::google::protobuf::uint32 value);
  
  // @@protoc_insertion_point(class_scope:net.Reply)
 private:
  inline void set_has_error();
  inline void clear_has_error();
  inline void set_has_action();
  inline void clear_has_action();
  inline void set_has_amount();
  inline void clear_has_amount();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* error_;
  ::google::protobuf::uint32 action_;
  ::google::protobuf::uint32 amount_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  friend void  protobuf_AddDesc_packet_2eproto();
  friend void protobuf_AssignDesc_packet_2eproto();
  friend void protobuf_ShutdownFile_packet_2eproto();
  
  void InitAsDefaultInstance();
  static Reply* default_instance_;
};
// ===================================================================


// ===================================================================

// Packet_Player

// required string Name = 1;
inline bool Packet_Player::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet_Player::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet_Player::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet_Player::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& Packet_Player::name() const {
  return *name_;
}
inline void Packet_Player::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Packet_Player::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Packet_Player::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Packet_Player::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* Packet_Player::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required uint32 Stack = 2;
inline bool Packet_Player::has_stack() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet_Player::set_has_stack() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet_Player::clear_has_stack() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet_Player::clear_stack() {
  stack_ = 0u;
  clear_has_stack();
}
inline ::google::protobuf::uint32 Packet_Player::stack() const {
  return stack_;
}
inline void Packet_Player::set_stack(::google::protobuf::uint32 value) {
  set_has_stack();
  stack_ = value;
}

// required uint32 Bet = 3;
inline bool Packet_Player::has_bet() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet_Player::set_has_bet() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet_Player::clear_has_bet() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet_Player::clear_bet() {
  bet_ = 0u;
  clear_has_bet();
}
inline ::google::protobuf::uint32 Packet_Player::bet() const {
  return bet_;
}
inline void Packet_Player::set_bet(::google::protobuf::uint32 value) {
  set_has_bet();
  bet_ = value;
}

// repeated uint32 Cards = 4;
inline int Packet_Player::cards_size() const {
  return cards_.size();
}
inline void Packet_Player::clear_cards() {
  cards_.Clear();
}
inline ::google::protobuf::uint32 Packet_Player::cards(int index) const {
  return cards_.Get(index);
}
inline void Packet_Player::set_cards(int index, ::google::protobuf::uint32 value) {
  cards_.Set(index, value);
}
inline void Packet_Player::add_cards(::google::protobuf::uint32 value) {
  cards_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
Packet_Player::cards() const {
  return cards_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
Packet_Player::mutable_cards() {
  return &cards_;
}

// -------------------------------------------------------------------

// Packet_Table

// repeated .net.Packet.Player Players = 1;
inline int Packet_Table::players_size() const {
  return players_.size();
}
inline void Packet_Table::clear_players() {
  players_.Clear();
}
inline const ::net::Packet_Player& Packet_Table::players(int index) const {
  return players_.Get(index);
}
inline ::net::Packet_Player* Packet_Table::mutable_players(int index) {
  return players_.Mutable(index);
}
inline ::net::Packet_Player* Packet_Table::add_players() {
  return players_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::net::Packet_Player >&
Packet_Table::players() const {
  return players_;
}
inline ::google::protobuf::RepeatedPtrField< ::net::Packet_Player >*
Packet_Table::mutable_players() {
  return &players_;
}

// required uint32 Button = 2;
inline bool Packet_Table::has_button() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet_Table::set_has_button() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet_Table::clear_has_button() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet_Table::clear_button() {
  button_ = 0u;
  clear_has_button();
}
inline ::google::protobuf::uint32 Packet_Table::button() const {
  return button_;
}
inline void Packet_Table::set_button(::google::protobuf::uint32 value) {
  set_has_button();
  button_ = value;
}

// repeated uint32 Cards = 3;
inline int Packet_Table::cards_size() const {
  return cards_.size();
}
inline void Packet_Table::clear_cards() {
  cards_.Clear();
}
inline ::google::protobuf::uint32 Packet_Table::cards(int index) const {
  return cards_.Get(index);
}
inline void Packet_Table::set_cards(int index, ::google::protobuf::uint32 value) {
  cards_.Set(index, value);
}
inline void Packet_Table::add_cards(::google::protobuf::uint32 value) {
  cards_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
Packet_Table::cards() const {
  return cards_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
Packet_Table::mutable_cards() {
  return &cards_;
}

// -------------------------------------------------------------------

// Packet_Action

// required uint32 Player = 1;
inline bool Packet_Action::has_player() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet_Action::set_has_player() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet_Action::clear_has_player() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet_Action::clear_player() {
  player_ = 0u;
  clear_has_player();
}
inline ::google::protobuf::uint32 Packet_Action::player() const {
  return player_;
}
inline void Packet_Action::set_player(::google::protobuf::uint32 value) {
  set_has_player();
  player_ = value;
}

// required uint32 Id = 2;
inline bool Packet_Action::has_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet_Action::set_has_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet_Action::clear_has_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet_Action::clear_id() {
  id_ = 0u;
  clear_has_id();
}
inline ::google::protobuf::uint32 Packet_Action::id() const {
  return id_;
}
inline void Packet_Action::set_id(::google::protobuf::uint32 value) {
  set_has_id();
  id_ = value;
}

// optional uint32 Amount = 3;
inline bool Packet_Action::has_amount() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet_Action::set_has_amount() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet_Action::clear_has_amount() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet_Action::clear_amount() {
  amount_ = 0u;
  clear_has_amount();
}
inline ::google::protobuf::uint32 Packet_Action::amount() const {
  return amount_;
}
inline void Packet_Action::set_amount(::google::protobuf::uint32 value) {
  set_has_amount();
  amount_ = value;
}

// -------------------------------------------------------------------

// Packet_Phase

// repeated .net.Packet.Action Actions = 1;
inline int Packet_Phase::actions_size() const {
  return actions_.size();
}
inline void Packet_Phase::clear_actions() {
  actions_.Clear();
}
inline const ::net::Packet_Action& Packet_Phase::actions(int index) const {
  return actions_.Get(index);
}
inline ::net::Packet_Action* Packet_Phase::mutable_actions(int index) {
  return actions_.Mutable(index);
}
inline ::net::Packet_Action* Packet_Phase::add_actions() {
  return actions_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::net::Packet_Action >&
Packet_Phase::actions() const {
  return actions_;
}
inline ::google::protobuf::RepeatedPtrField< ::net::Packet_Action >*
Packet_Phase::mutable_actions() {
  return &actions_;
}

// -------------------------------------------------------------------

// Packet

// required .net.Packet.Table Info = 1;
inline bool Packet::has_info() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet::set_has_info() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet::clear_has_info() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet::clear_info() {
  if (info_ != NULL) info_->::net::Packet_Table::Clear();
  clear_has_info();
}
inline const ::net::Packet_Table& Packet::info() const {
  return info_ != NULL ? *info_ : *default_instance_->info_;
}
inline ::net::Packet_Table* Packet::mutable_info() {
  set_has_info();
  if (info_ == NULL) info_ = new ::net::Packet_Table;
  return info_;
}
inline ::net::Packet_Table* Packet::release_info() {
  clear_has_info();
  ::net::Packet_Table* temp = info_;
  info_ = NULL;
  return temp;
}

// repeated .net.Packet.Phase Phases = 2;
inline int Packet::phases_size() const {
  return phases_.size();
}
inline void Packet::clear_phases() {
  phases_.Clear();
}
inline const ::net::Packet_Phase& Packet::phases(int index) const {
  return phases_.Get(index);
}
inline ::net::Packet_Phase* Packet::mutable_phases(int index) {
  return phases_.Mutable(index);
}
inline ::net::Packet_Phase* Packet::add_phases() {
  return phases_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::net::Packet_Phase >&
Packet::phases() const {
  return phases_;
}
inline ::google::protobuf::RepeatedPtrField< ::net::Packet_Phase >*
Packet::mutable_phases() {
  return &phases_;
}

// -------------------------------------------------------------------

// Reply

// optional string Error = 1;
inline bool Reply::has_error() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Reply::set_has_error() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Reply::clear_has_error() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Reply::clear_error() {
  if (error_ != &::google::protobuf::internal::kEmptyString) {
    error_->clear();
  }
  clear_has_error();
}
inline const ::std::string& Reply::error() const {
  return *error_;
}
inline void Reply::set_error(const ::std::string& value) {
  set_has_error();
  if (error_ == &::google::protobuf::internal::kEmptyString) {
    error_ = new ::std::string;
  }
  error_->assign(value);
}
inline void Reply::set_error(const char* value) {
  set_has_error();
  if (error_ == &::google::protobuf::internal::kEmptyString) {
    error_ = new ::std::string;
  }
  error_->assign(value);
}
inline void Reply::set_error(const char* value, size_t size) {
  set_has_error();
  if (error_ == &::google::protobuf::internal::kEmptyString) {
    error_ = new ::std::string;
  }
  error_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Reply::mutable_error() {
  set_has_error();
  if (error_ == &::google::protobuf::internal::kEmptyString) {
    error_ = new ::std::string;
  }
  return error_;
}
inline ::std::string* Reply::release_error() {
  clear_has_error();
  if (error_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = error_;
    error_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required uint32 Action = 2;
inline bool Reply::has_action() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Reply::set_has_action() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Reply::clear_has_action() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Reply::clear_action() {
  action_ = 0u;
  clear_has_action();
}
inline ::google::protobuf::uint32 Reply::action() const {
  return action_;
}
inline void Reply::set_action(::google::protobuf::uint32 value) {
  set_has_action();
  action_ = value;
}

// required uint32 Amount = 3;
inline bool Reply::has_amount() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Reply::set_has_amount() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Reply::clear_has_amount() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Reply::clear_amount() {
  amount_ = 0u;
  clear_has_amount();
}
inline ::google::protobuf::uint32 Reply::amount() const {
  return amount_;
}
inline void Reply::set_amount(::google::protobuf::uint32 value) {
  set_has_amount();
  amount_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace net

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_packet_2eproto__INCLUDED

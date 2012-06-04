#ifndef GUID_h__
#define GUID_h__

//! Guid helper
//!
//! \class CGUID
//!
class CGUID
{
public:
	CGUID(void);
	~CGUID(void);

	//! Generate guid
	static const std::string Generate();
};
#endif // GUID_h__

#ifndef COPY_DISABLER_H_
#define COPY_DISABLER_H_

#if 0
#define DISABLE_COPY(Type)\
	Type(const Type&) = delete;\
	Type& operator=(const Type&) = delete

#define DISABLE_MOVE(Type)\
	Type(Type&&) = delete;\
	Type& operator=(Type&&) = delete

#define DISABLE_COPY_MOVE(Type)\
	DISABLE_COPY(Type);\
	DISABLE_MOVE(Type)
#endif
// A macro to disallow the evil copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
	TypeName(const TypeName&);                    \
	void operator=(const TypeName&)

#endif // COPY_DISABLER_H_

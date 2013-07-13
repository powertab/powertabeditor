/////////////////////////////////////////////////////////////////////////////
// Name:            powertabstream.h
// Purpose:         Constants and enumerations used by the PowerTabInputStream and PowerTabOutputStream classes
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 19, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef POWERTABSTREAM_H
#define POWERTABSTREAM_H

// Note: Starting with MFC 4.0, the file format written/read has been
//  extended to eliminate the previous 32k limit.  Files previously written
//  can still be read by old versions (even 16-bit versions).  In addition,
//  new files, unless they are large enough to take advantage of 32-bit tags,
//  can be read by old versions.

// Pointer mapping constants
const uint16_t        NULL_TAG                = 0;                    ///< Special tag indicating NULL ptrs
const uint16_t        NEW_CLASS_TAG           = 0xffff;               ///< Special tag indicating new CRuntimeClass
const uint16_t        CLASS_TAG               = 0x8000;               ///< 0x8000 indicates class tag (OR'd)
const uint32_t      BIG_CLASS_TAG           = 0x80000000;           ///< 0x80000000 indicates big class tag (OR'd)
const uint16_t        BIG_OBJECT_TAG          = 0x7fff;               ///< 0x7fff indicates uint32_t object tag
const uint32_t      MAX_MAP_COUNT           = 0x3ffffffe;           ///< 0x3ffffffe last valid mapCount

// Note: tag value 0x8000 could be used for something in the future, since
//  it is currently an invalid tag (0x8000 means zero wClassTag, but zero
//  index is always reserved for a NULL pointer, and a NULL runtime class
//  does not make any sense).

// This is how the tags have been allocated currently:
//
//  0x0000              represents NULL pointer
//  0x0001 - 0x7FFE     "small" object tags
//  0x7FFF              header for "big" object/class tag
//  0x8000              reserved for future use
//  0x8001 - 0xFFFE     "small" class tag
//  0xFFFF              header for class definition
//
// The special value of 0x7FFF indicates that a uint32_t tag follows.  This
// two part "big" tag is used for 32-bit tag values 0x7FFF and above.
// The tag value of 0x7FFF was unused in MFC versions prior to MFC 4.0.

// Added
// MFC String constants
const uint8_t        BYTE_PLACEHOLDER            = 0xff;             ///< Placeholder used when string length > 254
const uint16_t        WORD_PLACEHOLDER            = 0xffff;           ///< Placeholder used when string length > 65533
const uint16_t        UNICODE_MARKER              = 0xfffe;           ///< Marker used to indicate a Unicode string
const uint32_t      DOUBLEWORD_PLACEHOLDER      = 0xffffffff;       ///< Placeholder used when string length > 4294967295

const size_t        MAX_CLASSNAME_LENGTH        = 64;               ///< Maximum length for a class name

enum PowerTabStreamError
{   
    POWERTABSTREAM_NO_ERROR                     = 0x1000,           ///< stream is in good state
    POWERTABSTREAM_INVALID_MARKER,                                  ///< Marker is invalid at start of file
    POWERTABSTREAM_INVALID_FILE_VERSION,
    POWERTABSTREAM_INVALID_FILE_TYPE,
    POWERTABSTREAM_BAD_INDEX,
    POWERTABSTREAM_BAD_CLASS,
    POWERTABSTREAM_BAD_SCHEMA
};

// Position Flags
#define POSITION_AT			0	// Exact match of position
#define POSITION_AT_BEFORE	1	// Position at or before
#define POSITION_AT_AFTER	2	// Position at or after
#define POSITION_BEFORE		3	// Last position before
#define POSITION_AFTER		4	// First position after

// ----------------------------------------------------------------------------
// _WX_DEFINE_POWERTABARRAY: array used to simulate Power Tab MFC lists
// ----------------------------------------------------------------------------

#define  _WX_DEFINE_POWERTABARRAY(T, name, base, classexp)            \
wxCOMPILE_TIME_ASSERT2(sizeof(T) <= sizeof(base::base_type),          \
                       TypeTooBigToBeStoredIn##base,                  \
                       name);                                         \
typedef int (CMPFUNC_CONV *CMPFUNC##T)(T *pItem1, T *pItem2);         \
classexp name : public base                                           \
{                                                                     \
public:                                                               \
  name() { }                                                          \
  ~name() { DeleteContents(); }                                       \
                                                                      \
  name& operator=(const name& src)                                    \
    { base* temp = (base*) this;                                      \
      (*temp) = ((const base&)src);                                   \
      return *this; }                                                 \
                                                                      \
  T& operator[](size_t uiIndex) const                                 \
    { return (T&)(base::Item(uiIndex)); }                             \
  T& Item(size_t uiIndex) const                                       \
    { return (T&)(base::Item(uiIndex)); }                             \
  T& Last() const                                                     \
    { return (T&)(base::Item(Count() - 1)); }                         \
                                                                      \
  int Index(T Item, bool bFromEnd = FALSE) const                      \
    { return base::Index(Item, bFromEnd); }                           \
                                                                      \
  void Add(T Item, size_t nInsert = 1)                                \
    { base::Add(Item, nInsert); }                                     \
  void Insert(T Item, size_t uiIndex, size_t nInsert = 1)             \
    { base::Insert(Item, uiIndex, nInsert) ; }                        \
                                                                      \
  bool Equals(const name& compare) const                             \
  {                                                                   \
    size_t i = 0;                                                     \
    uint32_t thisCount = base::GetCount();                            \
    uint32_t thatCount = compare.GetCount();                          \
    if (thisCount != thatCount)                                       \
        return (false);                                               \
    for (; i < thisCount; i++)                                        \
    {                                                                 \
        if (*(*this)[i] != *compare[i])                               \
            return (false);                                           \
    }                                                                 \
    return (true);                                                    \
  }                                                                   \
                                                                      \
  void Copy(const name& src)                                          \
  {                                                                   \
    DeleteContents();                                                 \
    size_t i = 0;                                                     \
    uint32_t count = src.GetCount();                                  \
    base::Alloc(count);                                               \
    for (; i < count; i++)                                            \
        base::Add(src[i]->CloneObject());                             \
  }                                                                   \
                                                                      \
  void RemoveAt(size_t uiIndex, size_t nRemove = 1)                   \
    { base::RemoveAt(uiIndex, nRemove); }                             \
  void Remove(T Item)                                                 \
    { int iIndex = Index(Item);                                       \
      PTB_CHECK_THAT2_MSG( iIndex != wxNOT_FOUND, return,                    \
         _WX_ERROR_REMOVE);                                           \
      base::RemoveAt((size_t)iIndex); }                               \
                                                                      \
  void Sort(CMPFUNC##T fCmp) { base::Sort((CMPFUNC)fCmp); }           \
                                                                      \
  bool Serialize(PowerTabOutputStream& stream)                        \
  {                                                                   \
    uint32_t count = base::GetCount();                                \
    stream.WriteCount(count);                                         \
    PTB_CHECK_THAT(stream.CheckState(), false);                              \
    size_t i = 0;                                                     \
    for (; i < count; i++)                                            \
    {                                                                 \
        stream.WriteObject((*this)[i]);                               \
        PTB_CHECK_THAT(stream.CheckState(), false);                          \
    }                                                                 \
    return (true);                                                    \
  }                                                                   \
                                                                      \
  bool Deserialize(PowerTabInputStream& stream, uint16_t version)       \
  {                                                                   \
    uint32_t newCount = stream.ReadCount();                           \
    PTB_CHECK_THAT(stream.CheckState(), false);                              \
    if (newCount > 0)                                                 \
        base::Alloc(newCount);                                        \
    while (newCount--)                                                \
    {                                                                 \
        PowerTabObject* newData = stream.ReadObject(version);         \
        PTB_CHECK_THAT(stream.CheckState(), false);                          \
        base::Add(newData);                                           \
    }                                                                 \
    return (true);                                                    \
  }                                                                   \
  void DeleteContents()                                               \
  {                                                                   \
    size_t i = 0;                                                     \
    uint32_t count = base::GetCount();                                \
    for (; i < count; i++)                                            \
        delete (*this)[i];                                            \
    base::Clear();                                                    \
  }                                                                   \
}
////////////////////////////////////////////////////////////////////////////

#define WX_DEFINE_POWERTABARRAY(T, name)                              \
    WX_DEFINE_USER_EXPORTED_POWERTABARRAY(T, name, wxBaseArrayPtrVoid, wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_EXPORTED_POWERTABARRAY(T, name)                     \
    WX_DEFINE_USER_EXPORTED_POWERTABARRAY(T, name, wxBaseArrayPtrVoid, WXDLLEXPORT)

#define WX_DEFINE_USER_EXPORTED_POWERTABARRAY(T, name, base, expmode) \
    typedef T _wxArray##name;                                     \
    _WX_DEFINE_POWERTABARRAY(_wxArray##name, name, base, class expmode)
        
#endif // POWERTABSTREAM_H

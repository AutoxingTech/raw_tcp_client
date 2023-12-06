/*
 * Copyright (C) 2009, Willow Garage, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#ifndef AXCPP_SERIALIZATION_H
#define AXCPP_SERIALIZATION_H

#include <vector>
#include <map>
#include <cstring>
#include <type_traits>
#include <ostream>

#include <boost/call_traits.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>

#include "time.h"

#define ROS_FORCE_INLINE inline

namespace ros
{
namespace message_traits
{

/**
 * \brief Base type for compile-type true/false tests.  Compatible with Boost.MPL.  classes inheriting from this type
 * are \b true values.
 */
struct TrueType
{
    static const bool value = true;
    typedef TrueType type;
};

/**
 * \brief Base type for compile-type true/false tests.  Compatible with Boost.MPL.  classes inheriting from this type
 * are \b false values.
 */
struct FalseType
{
    static const bool value = false;
    typedef FalseType type;
};

/**
 * \brief A fixed-size datatype is one whose size is constant, i.e. it has no variable-length arrays or strings
 */
template <typename M>
struct IsFixedSize : public FalseType
{
};

#define ROSLIB_CREATE_SIMPLE_TRAITS(Type)                                                                              \
    template <>                                                                                                        \
    struct IsFixedSize<Type> : public TrueType                                                                         \
    {                                                                                                                  \
    };

ROSLIB_CREATE_SIMPLE_TRAITS(uint8_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int8_t)
ROSLIB_CREATE_SIMPLE_TRAITS(uint16_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int16_t)
ROSLIB_CREATE_SIMPLE_TRAITS(uint32_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int32_t)
ROSLIB_CREATE_SIMPLE_TRAITS(uint64_t)
ROSLIB_CREATE_SIMPLE_TRAITS(int64_t)
ROSLIB_CREATE_SIMPLE_TRAITS(float)
ROSLIB_CREATE_SIMPLE_TRAITS(double)
ROSLIB_CREATE_SIMPLE_TRAITS(Time)
ROSLIB_CREATE_SIMPLE_TRAITS(Duration)

// because std::vector<bool> is not a true vector, bool is not a simple type
template <>
struct IsFixedSize<bool> : public TrueType
{
};

} // namespace message_traits
} // namespace ros

/**
 * \brief Declare your serializer to use an allInOne member instead of requiring 3 different serialization
 * functions.
 *
 * The allinone method has the form:
\verbatim
template<typename Stream, typename T>
inline static void allInOne(Stream& stream, T t)
{
  stream.next(t.a);
  stream.next(t.b);
  ...
}
\endverbatim
 *
 * The only guarantee given is that Stream::next(T) is defined.
 */
#define ROS_DECLARE_ALLINONE_SERIALIZER                                                                                \
    template <typename Stream, typename T>                                                                             \
    inline static void write(Stream& stream, const T& t)                                                               \
    {                                                                                                                  \
        allInOne<Stream, const T&>(stream, t);                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    template <typename Stream, typename T>                                                                             \
    inline static void read(Stream& stream, T& t)                                                                      \
    {                                                                                                                  \
        allInOne<Stream, T&>(stream, t);                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <typename T>                                                                                              \
    inline static uint32_t serializedLength(const T& t)                                                                \
    {                                                                                                                  \
        LStream stream;                                                                                                \
        allInOne<LStream, const T&>(stream, t);                                                                        \
        return stream.getLength();                                                                                     \
    }

namespace ros
{
namespace serialization
{
namespace mt = message_traits;
namespace mpl = boost::mpl;

class StreamOverrunException : public ros::Exception
{
public:
    StreamOverrunException(const std::string& what) : Exception(what) {}
};

inline void throwStreamOverrun()
{
    throw StreamOverrunException("xxx");
}

/**
 * \brief Templated serialization class.  Default implementation provides backwards compatibility with
 * old message types.
 *
 * Specializing the Serializer class is the only thing you need to do to get the ROS serialization system
 * to work with a type.
 */
template <typename T>
struct Serializer
{
    /**
     * \brief Write an object to the stream.  Normally the stream passed in here will be a ros::serialization::OStream
     */
    template <typename Stream>
    inline static void write(Stream& stream, typename boost::call_traits<T>::param_type t)
    {
        t.serialize(stream.getData(), 0);
    }

    /**
     * \brief Read an object from the stream.  Normally the stream passed in here will be a ros::serialization::IStream
     */
    template <typename Stream>
    inline static void read(Stream& stream, typename boost::call_traits<T>::reference t)
    {
        t.deserialize(stream.getData());
    }

    /**
     * \brief Determine the serialized length of an object.
     */
    inline static uint32_t serializedLength(typename boost::call_traits<T>::param_type t)
    {
        return t.serializationLength();
    }
};

/**
 * \brief Serialize an object.  Stream here should normally be a ros::serialization::OStream
 */
template <typename T, typename Stream>
inline void serialize(Stream& stream, const T& t)
{
    Serializer<T>::write(stream, t);
}

/**
 * \brief Deserialize an object.  Stream here should normally be a ros::serialization::IStream
 */
template <typename T, typename Stream>
inline void deserialize(Stream& stream, T& t)
{
    Serializer<T>::read(stream, t);
}

/**
 * \brief Determine the serialized length of an object
 */
template <typename T>
inline uint32_t serializationLength(const T& t)
{
    return Serializer<T>::serializedLength(t);
}

#define ROS_CREATE_SIMPLE_SERIALIZER(Type)                                                                             \
    template <>                                                                                                        \
    struct Serializer<Type>                                                                                            \
    {                                                                                                                  \
        template <typename Stream>                                                                                     \
        inline static void write(Stream& stream, const Type v)                                                         \
        {                                                                                                              \
            memcpy(stream.advance(sizeof(v)), &v, sizeof(v));                                                          \
        }                                                                                                              \
                                                                                                                       \
        template <typename Stream>                                                                                     \
        inline static void read(Stream& stream, Type& v)                                                               \
        {                                                                                                              \
            memcpy(&v, stream.advance(sizeof(v)), sizeof(v));                                                          \
        }                                                                                                              \
                                                                                                                       \
        inline static uint32_t serializedLength(const Type&)                                                           \
        {                                                                                                              \
            return sizeof(Type);                                                                                       \
        }                                                                                                              \
    };

ROS_CREATE_SIMPLE_SERIALIZER(uint8_t)
ROS_CREATE_SIMPLE_SERIALIZER(int8_t)
ROS_CREATE_SIMPLE_SERIALIZER(uint16_t)
ROS_CREATE_SIMPLE_SERIALIZER(int16_t)
ROS_CREATE_SIMPLE_SERIALIZER(uint32_t)
ROS_CREATE_SIMPLE_SERIALIZER(int32_t)
ROS_CREATE_SIMPLE_SERIALIZER(uint64_t)
ROS_CREATE_SIMPLE_SERIALIZER(int64_t)
ROS_CREATE_SIMPLE_SERIALIZER(float)
ROS_CREATE_SIMPLE_SERIALIZER(double)

/**
 * \brief Serializer specialized for bool (serialized as uint8)
 */
template <>
struct Serializer<bool>
{
    template <typename Stream>
    inline static void write(Stream& stream, const bool v)
    {
        uint8_t b = static_cast<uint8_t>(v);
        memcpy(stream.advance(1), &b, 1);
    }

    template <typename Stream>
    inline static void read(Stream& stream, bool& v)
    {
        uint8_t b;
        memcpy(&b, stream.advance(1), 1);
        v = static_cast<bool>(b);
    }

    inline static uint32_t serializedLength(bool) { return 1; }
};

/**
 * \brief  Serializer specialized for std::string
 */
template <class ContainerAllocator>
struct Serializer<std::basic_string<char, std::char_traits<char>, ContainerAllocator>>
{
    typedef std::basic_string<char, std::char_traits<char>, ContainerAllocator> StringType;

    template <typename Stream>
    inline static void write(Stream& stream, const StringType& str)
    {
        size_t len = str.size();
        stream.next(static_cast<uint32_t>(len));

        if (len > 0)
        {
            memcpy(stream.advance(static_cast<uint32_t>(len)), str.data(), len);
        }
    }

    template <typename Stream>
    inline static void read(Stream& stream, StringType& str)
    {
        uint32_t len;
        stream.next(len);
        if (len > 0)
        {
            str = StringType(reinterpret_cast<char*>(stream.advance(len)), len);
        }
        else
        {
            str.clear();
        }
    }

    inline static uint32_t serializedLength(const StringType& str) { return 4 + static_cast<uint32_t>(str.size()); }
};

/**
 * \brief Serializer specialized for ros::Time
 */
template <>
struct Serializer<ros::Time>
{
    template <typename Stream>
    inline static void write(Stream& stream, const ros::Time& v)
    {
        stream.next(v.sec);
        stream.next(v.nsec);
    }

    template <typename Stream>
    inline static void read(Stream& stream, ros::Time& v)
    {
        stream.next(v.sec);
        stream.next(v.nsec);
    }

    inline static uint32_t serializedLength(const ros::Time&) { return 8; }
};

/**
 * \brief Serializer specialized for ros::Duration
 */
template <>
struct Serializer<ros::Duration>
{
    template <typename Stream>
    inline static void write(Stream& stream, const ros::Duration& v)
    {
        stream.next(v.sec);
        stream.next(v.nsec);
    }

    template <typename Stream>
    inline static void read(Stream& stream, ros::Duration& v)
    {
        stream.next(v.sec);
        stream.next(v.nsec);
    }

    inline static uint32_t serializedLength(const ros::Duration&) { return 8; }
};

/**
 * \brief Vector serializer.  Default implementation does nothing
 */
template <typename T, class ContainerAllocator, class Enabled = void>
struct VectorSerializer
{
};

/**
 * \brief Vector serializer, specialized for non-fixed-size, non-simple types
 */
template <typename T, class ContainerAllocator>
struct VectorSerializer<T, ContainerAllocator, typename std::enable_if<!mt::IsFixedSize<T>::value>::type>
{
    typedef std::vector<T, typename ContainerAllocator::template rebind<T>::other> VecType;
    typedef typename VecType::iterator IteratorType;
    typedef typename VecType::const_iterator ConstIteratorType;

    template <typename Stream>
    inline static void write(Stream& stream, const VecType& v)
    {
        stream.next(static_cast<uint32_t>(v.size()));
        ConstIteratorType it = v.begin();
        ConstIteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    template <typename Stream>
    inline static void read(Stream& stream, VecType& v)
    {
        uint32_t len;
        stream.next(len);
        v.resize(len);
        IteratorType it = v.begin();
        IteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    inline static uint32_t serializedLength(const VecType& v)
    {
        uint32_t size = 4;
        ConstIteratorType it = v.begin();
        ConstIteratorType end = v.end();
        for (; it != end; ++it)
        {
            size += serializationLength(*it);
        }

        return size;
    }
};

/**
 * \brief Vector serializer, specialized for fixed-size simple types
 */
template <typename T, class ContainerAllocator>
struct VectorSerializer<T, ContainerAllocator, typename std::enable_if<std::is_pod<T>::value>::type>
{
    typedef std::vector<T, typename ContainerAllocator::template rebind<T>::other> VecType;
    typedef typename VecType::iterator IteratorType;
    typedef typename VecType::const_iterator ConstIteratorType;

    template <typename Stream>
    inline static void write(Stream& stream, const VecType& v)
    {
        uint32_t len = static_cast<uint32_t>(v.size());
        stream.next(len);
        if (!v.empty())
        {
            const uint32_t data_len = len * static_cast<uint32_t>(sizeof(T));
            memcpy(stream.advance(data_len), &v.front(), data_len);
        }
    }

    template <typename Stream>
    inline static void read(Stream& stream, VecType& v)
    {
        uint32_t len;
        stream.next(len);
        v.resize(len);

        if (len > 0)
        {
            const uint32_t data_len = static_cast<uint32_t>(sizeof(T)) * len;
            memcpy(static_cast<void*>(&v.front()), stream.advance(data_len), data_len);
        }
    }

    inline static uint32_t serializedLength(const VecType& v)
    {
        return 4 + v.size() * static_cast<uint32_t>(sizeof(T));
    }
};

/**
 * \brief Vector serializer, specialized for fixed-size non-simple types
 */
template <typename T, class ContainerAllocator>
struct VectorSerializer<T, ContainerAllocator,
                        typename std::enable_if<mpl::and_<mt::IsFixedSize<T>, mpl::not_<std::is_pod<T>>>::value>::type>
{
    typedef std::vector<T, typename ContainerAllocator::template rebind<T>::other> VecType;
    typedef typename VecType::iterator IteratorType;
    typedef typename VecType::const_iterator ConstIteratorType;

    template <typename Stream>
    inline static void write(Stream& stream, const VecType& v)
    {
        stream.next(static_cast<uint32_t>(v.size()));
        ConstIteratorType it = v.begin();
        ConstIteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    template <typename Stream>
    inline static void read(Stream& stream, VecType& v)
    {
        uint32_t len;
        stream.next(len);
        v.resize(len);
        IteratorType it = v.begin();
        IteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    inline static uint32_t serializedLength(const VecType& v)
    {
        uint32_t size = 4;
        if (!v.empty())
        {
            uint32_t len_each = serializationLength(v.front());
            size += len_each * static_cast<uint32_t>(v.size());
        }

        return size;
    }
};

/**
 * \brief serialize version for std::vector
 */
template <typename T, class ContainerAllocator, typename Stream>
inline void serialize(Stream& stream, const std::vector<T, ContainerAllocator>& t)
{
    VectorSerializer<T, ContainerAllocator>::write(stream, t);
}

/**
 * \brief deserialize version for std::vector
 */
template <typename T, class ContainerAllocator, typename Stream>
inline void deserialize(Stream& stream, std::vector<T, ContainerAllocator>& t)
{
    VectorSerializer<T, ContainerAllocator>::read(stream, t);
}

/**
 * \brief serializationLength version for std::vector
 */
template <typename T, class ContainerAllocator>
inline uint32_t serializationLength(const std::vector<T, ContainerAllocator>& t)
{
    return VectorSerializer<T, ContainerAllocator>::serializedLength(t);
}

/**
 * \brief Array serializer, default implementation does nothing
 */
template <typename T, size_t N, class Enabled = void>
struct ArraySerializer
{
};

/**
 * \brief Array serializer, specialized for non-fixed-size, non-simple types
 */
template <typename T, size_t N>
struct ArraySerializer<T, N, typename std::enable_if<!mt::IsFixedSize<T>::value>::type>
{
    typedef std::array<T, N> ArrayType;
    typedef typename ArrayType::iterator IteratorType;
    typedef typename ArrayType::const_iterator ConstIteratorType;

    template <typename Stream>
    inline static void write(Stream& stream, const ArrayType& v)
    {
        ConstIteratorType it = v.begin();
        ConstIteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    template <typename Stream>
    inline static void read(Stream& stream, ArrayType& v)
    {
        IteratorType it = v.begin();
        IteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    inline static uint32_t serializedLength(const ArrayType& v)
    {
        uint32_t size = 0;
        ConstIteratorType it = v.begin();
        ConstIteratorType end = v.end();
        for (; it != end; ++it)
        {
            size += serializationLength(*it);
        }

        return size;
    }
};

/**
 * \brief Array serializer, specialized for fixed-size, simple types
 */
template <typename T, size_t N>
struct ArraySerializer<T, N, typename std::enable_if<std::is_pod<T>::value>::type>
{
    typedef std::array<T, N> ArrayType;
    typedef typename ArrayType::iterator IteratorType;
    typedef typename ArrayType::const_iterator ConstIteratorType;

    template <typename Stream>
    inline static void write(Stream& stream, const ArrayType& v)
    {
        const uint32_t data_len = N * sizeof(T);
        memcpy(stream.advance(data_len), &v.front(), data_len);
    }

    template <typename Stream>
    inline static void read(Stream& stream, ArrayType& v)
    {
        const uint32_t data_len = N * sizeof(T);
        memcpy(&v.front(), stream.advance(data_len), data_len);
    }

    inline static uint32_t serializedLength(const ArrayType&) { return N * sizeof(T); }
};

/**
 * \brief Array serializer, specialized for fixed-size, non-simple types
 */
template <typename T, size_t N>
struct ArraySerializer<T, N,
                       typename std::enable_if<mpl::and_<mt::IsFixedSize<T>, mpl::not_<std::is_pod<T>>>::value>::type>
{
    typedef std::array<T, N> ArrayType;
    typedef typename ArrayType::iterator IteratorType;
    typedef typename ArrayType::const_iterator ConstIteratorType;

    template <typename Stream>
    inline static void write(Stream& stream, const ArrayType& v)
    {
        ConstIteratorType it = v.begin();
        ConstIteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    template <typename Stream>
    inline static void read(Stream& stream, ArrayType& v)
    {
        IteratorType it = v.begin();
        IteratorType end = v.end();
        for (; it != end; ++it)
        {
            stream.next(*it);
        }
    }

    inline static uint32_t serializedLength(const ArrayType& v) { return serializationLength(v.front()) * N; }
};

/**
 * \brief serialize version for std::array
 */
template <typename T, size_t N, typename Stream>
inline void serialize(Stream& stream, const std::array<T, N>& t)
{
    ArraySerializer<T, N>::write(stream, t);
}

/**
 * \brief deserialize version for std::array
 */
template <typename T, size_t N, typename Stream>
inline void deserialize(Stream& stream, std::array<T, N>& t)
{
    ArraySerializer<T, N>::read(stream, t);
}

/**
 * \brief serializationLength version for std::array
 */
template <typename T, size_t N>
inline uint32_t serializationLength(const std::array<T, N>& t)
{
    return ArraySerializer<T, N>::serializedLength(t);
}

/**
 * \brief Enum
 */
namespace stream_types
{
enum StreamType
{
    Input,
    Output,
    Length
};
} // namespace stream_types
typedef stream_types::StreamType StreamType;

/**
 * \brief Stream base-class, provides common functionality for IStream and OStream
 */
struct Stream
{
    /*
     * \brief Returns a pointer to the current position of the stream
     */
    inline uint8_t* getData() { return data_; }
    /**
     * \brief Advances the stream, checking bounds, and returns a pointer to the position before it
     * was advanced.
     * \throws StreamOverrunException if len would take this stream past the end of its buffer
     */
    ROS_FORCE_INLINE uint8_t* advance(uint32_t len)
    {
        uint8_t* old_data = data_;
        data_ += len;
        if (data_ > end_)
        {
            // Throwing directly here causes a significant speed hit due to the extra code generated
            // for the throw statement
            throwStreamOverrun();
        }
        return old_data;
    }

    /**
     * \brief Returns the amount of space left in the stream
     */
    inline uint32_t getLength() { return static_cast<uint32_t>(end_ - data_); }

protected:
    Stream(uint8_t* _data, uint32_t _count) : data_(_data), end_(_data + _count) {}

private:
    uint8_t* data_;
    uint8_t* end_;
};

/**
 * \brief Input stream
 */
struct IStream : public Stream
{
    static const StreamType stream_type = stream_types::Input;

    IStream(uint8_t* data, uint32_t count) : Stream(data, count) {}

    /**
     * \brief Deserialize an item from this input stream
     */
    template <typename T>
    ROS_FORCE_INLINE void next(T& t)
    {
        deserialize(*this, t);
    }

    template <typename T>
    ROS_FORCE_INLINE IStream& operator>>(T& t)
    {
        deserialize(*this, t);
        return *this;
    }
};

/**
 * \brief Output stream
 */
struct OStream : public Stream
{
    static const StreamType stream_type = stream_types::Output;

    OStream(uint8_t* data, uint32_t count) : Stream(data, count) {}

    /**
     * \brief Serialize an item to this output stream
     */
    template <typename T>
    ROS_FORCE_INLINE void next(const T& t)
    {
        serialize(*this, t);
    }

    template <typename T>
    ROS_FORCE_INLINE OStream& operator<<(const T& t)
    {
        serialize(*this, t);
        return *this;
    }
};

/**
 * \brief Length stream
 *
 * LStream is not what you would normally think of as a stream, but it is used in order to support
 * allinone serializers.
 */
struct LStream
{
    static const StreamType stream_type = stream_types::Length;

    LStream() : count_(0) {}

    /**
     * \brief Add the length of an item to this length stream
     */
    template <typename T>
    ROS_FORCE_INLINE void next(const T& t)
    {
        count_ += serializationLength(t);
    }

    /**
     * \brief increment the length by len
     */
    ROS_FORCE_INLINE uint32_t advance(uint32_t len)
    {
        uint32_t old = count_;
        count_ += len;
        return old;
    }

    /**
     * \brief Get the total length of this tream
     */
    inline uint32_t getLength() { return count_; }

private:
    uint32_t count_;
};

} // namespace serialization

} // namespace ros

#endif // ROSCPP_SERIALIZATION_H

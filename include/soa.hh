/*
 * Author: Benedict R. Gaster
 * Desc:  Simple SOA interface
 *
 * The idea is to provide an interface for storing and retriving
 * aggrguate datatypes in SOA format.  
 *
 * The current interface is pretty straightfoward and has some
 * overhead with respect to having to specify template
 * arguments. (Note that this does not imply any runtime overhead,
 * which in that case the abstraction introduces none.) Clearly it
 * would be possible to implement a more advanced interface but this
 * satifies my current needs.
 */
#pragma once

#include <memory>
#include <tuple>

namespace detail {

template<class... T>
struct load_compound;

template<class last>
struct load_compound<last>
{
    static std::tuple<last> load(void * address, size_t byte_stride)
    {
	return std::tuple<last>(*reinterpret_cast<last*>(address));
    }
};

template<class head, class... tail>
struct load_compound<head, tail...>
{    
    // stride defines the space between elements in the compound strut as stored in SOA... 
    static std::tuple<head, tail...> load(void * address, size_t byte_stride)
    {
	auto ca = reinterpret_cast<char *>(address);
	auto tmp = std::tuple<head>(*reinterpret_cast<head*>(address));
	
	return std::tuple_cat(
	    tmp,
	    load_compound<tail...>::load(static_cast<void*>(ca + byte_stride), byte_stride));
    }
};

template<typename T, typename Enable = void> struct count_types;

template<typename T>
struct count_types<
    T, 
    typename std::enable_if<std::is_scalar<typename std::remove_reference<T>::type>::value>::type> 
{
    static constexpr int count = 1;
};

template<> struct count_types<std::tuple<>> {
    static constexpr int count = 0;
};

template<class... T>
struct store_soa;

template<class last>
struct store_soa<last>
{
    static void store(void * address, size_t byte_stride, last value)
    {
	*reinterpret_cast<last*>(address) = value;
    }
};

template<class head, class... tail>
struct store_soa<head, tail...>
{    
    // stride defines the space between elements in the compound strut as stored in SOA... 
    static void store(
	void * address, 
	size_t byte_stride, 
	head value,
	tail... ts)
    {
	auto ca = reinterpret_cast<char *>(address);

	*reinterpret_cast<head*>(address) = value;

	store_soa<tail...>::store(static_cast<void*>(ca + byte_stride), byte_stride, ts...);
    }
};



}; // namespace detail

template<class compound, class ptr, class... Ts>
compound load_compound(ptr address, size_t byte_stride)
{
    return compound(detail::load_compound<Ts...>::load(
	    reinterpret_cast<void *>(address), 
	    byte_stride));   
}

template<class compound, class ptr>
compound load_soa(ptr address, size_t byte_stride)
{
    return compound(*reinterpret_cast<compound *>(
	    reinterpret_cast<char*>(address) + byte_stride));
}

template<class compound, class... Ts>
void store_soa(compound address, size_t byte_stride, Ts... ts)
{
    detail::store_soa<Ts...>::store(static_cast<void*>(address), byte_stride, ts...);
}




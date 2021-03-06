#pragma once

//return the size in bytes of the largest type in the pack
template<typename F, typename... Ts> struct SizeofLargestType { static const size_t size = (sizeof(F) > SizeofLargestType <Ts...>::size ? sizeof(F) : SizeofLargestType <Ts...>::size); };
template<typename F> struct SizeofLargestType <F> { static const size_t size = sizeof(F); };

//A simple anonymous (does not have type information) union that can store a range of values defined by Ty
template <typename... Ty>
using Union = uint8_t[SizeofLargestType<Ty...>::size];
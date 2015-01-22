A Structure Of Arrays (SOA) Interface for C++
===

It is common and more straightforward to program in the idiom Arrays
Of Structures (AOS), however, it is often more friendly to the
underlying memory system to program with the idion Structure Of Arrays
(SOA). This is particually true in the presence of vectorization,
e.g. AVX/Neon or OpenCL on GPUs, such as AMD and NVIDIA.

This code is intended as a place to play around with ideas. Currently
I've implemented only a very simply interface, which meets the
needs of my current development. I expect that it is quite possible to
do something less complex but it is still slightly unclear how to
acheive this and still remain flexible.

Building
========

The interface itself is defined as a single header file and so can
just be included direclty. The implementation requires at least C++11.

To build the examples (just one for now), then cmake 2.8 is required
and at least clang/llvm 3.4. (Note the header does not reqire clang,
just the example.) To build:

change into the root dirctory and create the directory build. Then
cd into build and run the command:

    cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang

then 

    make

the example can be run with the command:

    ./examples/example1

and should produce the output:

    Read in AOS form...
    1 2 3
    4 5 6
    7 8 9
    Read in SOA form...
    1 2 3
    4 5 6
    7 8 9
    Store in SOA form from SOA seqneuces...
    Read in SOA form...
    1 2 3
    4 5 6
    7 8 9

Interface
=========

The interface is defined in:

    include/soa.hpp

and requires support for C++11, including varardic templates. I have
tested it only with clang/llvm 3.3.

Each datatype that whats to support storage SOA form, it is recomended
that each interface support the following types and methods, however,
it is not required.

The following type can be defined as part of a datatypes public
interface, that is a pointer type representing the elements is SOA
form. In the case that the elements are of the same type, then use that
type, otherwise use unsigned char *:

    soa_ptr_type      
 
The following public method can be provided to allow users to
allocates space for storing, n, values of the type being defined in
SOA form:

    static soa_ptr_type_ allocate_soa(size_t n); 
 
Finally the following public method can be provided to allow users to
determine the stride from one array of elements to the next within the
SOA layout:

    static constexpr size_t stride_soa(); 

A limitation of this approach is it assumes that each array is padded
to equal length. I don't expect this to be a major concern as the
general use case for SOA representation is to paralleize using
AVX/Neon and so on and these expect elements of a certain size.
 

The library itself provides the following four functions. Each of
them assume the SOA layout, otherwise you can just use the datatype as
is, combined with standard memory allocation.

The first function enables access to SOA data via standard compound
(i.e. AOS) form.

    template<class compound, class ptr, class... Ts>
    compound load_compound(ptr address, size_t byte_stride);
 
The next function enables loading sequences of a single element in SOA
form. An example of this is if we were storing sequence of homogenous
points, then a sequence of SOA loads, mapping to AVX/Neon, would load
4 x values, 4 y values, and so on.

    template<class compound, class ptr>
    compound load_soa(ptr address, size_t byte_stride);
 
The next function stores a compound type into SOA form. This is particular useful
when converting from sequences of AOS into sequences of SOA.

    template<class compound, class... Ts>
    void store_soa_compound(compound address, size_t byte_stride, Ts... ts);

The final function stores sequences of a single element in SOA
form. The is the dual to the load_soa defined above.

    template<class compound, class... Ts>
    void store_soa(compound address, size_t byte_stride, Ts... ts);

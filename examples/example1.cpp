#include <soa.hh>

#include <iostream>
#include <memory>
#include <initializer_list>
#include <type_traits>

using namespace std;

typedef float float4 __attribute__((ext_vector_type(4)));

class vertex {
public:
    float x_;
    float y_;
    float z_;
    float _dummy_; 
    // we need this for when we want to map to SOA and AVX/NEON
    // we don't actually want to have them defined here as float4 as we want them in SOA form
    // this means we keep them seperate for now, techncially it would still work but it woudl 
    // strange

    // now implement the interface that all SOA storable compound values must provide
    typedef float * soa_ptr_type_;
    typedef std::remove_pointer<soa_ptr_type_>::type soa_base_type_;

    static soa_ptr_type_ allocate_soa(size_t n)
    {
	return reinterpret_cast<soa_ptr_type_>(new float[n * 4]);
    }    

    static vertex * allocate_aos(size_t n)
    {
	return new vertex[n];
    }

    static constexpr size_t stride_soa()
    {
	return 4 * sizeof(float);
    }

    // this is the only SOA method that is not static and is only
    // actually used when loading SOA to compound!
    vertex(std::tuple<float,float,float> tuple)
	: x_(std::get<0>(tuple)), 
	  y_(std::get<1>(tuple)), 
	  z_(std::get<2>(tuple))
    {
    }
    // finish interface that all SOA compound values must provide

    vertex()
    {
    }

    vertex(float x, float y, float z)
	: x_(x), y_(y), z_(z)
    {
    }
    
    
    vertex& operator= (const vertex& rhs) 
    {
	x_ = rhs.x_;
	y_ = rhs.y_;
	z_ = rhs.z_;
	return *this;
    }
};

int main(void)
{
    vertex::soa_ptr_type_ vsoa = vertex::allocate_soa(3);
    
    float v = 0.f;    
    // write some in SOA form
    for (int i = 0; i < 3; i++, v += 3) {
	store_soa<vertex::soa_ptr_type_, float, float, float>(vsoa + i, vertex::stride_soa(), v+1.f, v+2.f, v+3.f);
    }
   
    // read into compound type, i.e. AOS style
    cout << "Read in AOS form..." << endl;
    for (int i = 0; i < 3; i++) {
       vertex v = load_compound<vertex, vertex::soa_ptr_type_, float, float, float>(vsoa + i, vertex::stride_soa());    

       cout <<  v.x_ << " " <<  v.y_ << " " <<  v.z_ << endl;
    }

    // read in SOA form
    float4 x = load_soa<float4, vertex::soa_ptr_type_>(vsoa, vertex::stride_soa() * 0);
    float4 y = load_soa<float4, vertex::soa_ptr_type_>(vsoa, vertex::stride_soa() * 1);
    float4 z = load_soa<float4, vertex::soa_ptr_type_>(vsoa, vertex::stride_soa() * 2);

    cout << "Read in SOA form..." << std::endl;    
    cout <<  x.x << " " <<  y.x << " " <<  z.x << endl;
    cout <<  x.y << " " <<  y.y << " " <<  z.y << endl;
    cout <<  x.z << " " <<  y.z << " " <<  z.z << endl;
    
    return 0;
}

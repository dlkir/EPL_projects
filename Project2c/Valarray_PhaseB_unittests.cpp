/*
 * valarray_PhaseB_unittests.cpp
 * EPL - Spring 2015
 */

#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>

#include "InstanceCounter.h"
#include "Valarray.h"

#include "gtest/gtest.h"

using std::cout;
using std::endl;
using std::string;
using std::complex;

int InstanceCounter::counter = 0;

using namespace epl;

template <typename X, typename Y>
bool match(X x, Y y) {
    double d = x - y;
    if (d < 0) { d = -d; }
    return d < 1.0e-9; // not really machine epsilon, but close enough
}
/*********************************************************************/
// Phase B Tests
/*********************************************************************/

#if defined(PHASE_B1_0) | defined(PHASE_B)
TEST(PhaseB1, Sqrt) {
    valarray<int> v1(10);
    valarray<int> v4 = 4 + v1;

    valarray<float> v5 = v4.sqrt();
    for (uint64_t i = 0; i<10; i++) {
        EXPECT_EQ(2.0, v5[i]);
    }
}
#endif

#if defined(PHASE_B1_1) | defined(PHASE_B)
TEST(PhaseB1, Apply) {
    valarray<int> v1(10);
    valarray<int> v4 = 4 + v1;

    valarray<int> v7 = v4.apply(std::negate<int>());

    EXPECT_EQ(10, v7.size());

    for (uint64_t i = 0; i<10; i++) {
        EXPECT_EQ(-4, v7[i]);
    }
}
#endif

#if defined(PHASE_B1_2) | defined(PHASE_B)
TEST(PhaseB1, Accumulate) {
    valarray<int> v1{1, 2, 3, 4, 5};
    int sum = v1.accumulate(std::plus<int>());
    int product = v1.accumulate(std::multiplies<int>());
    EXPECT_EQ(15, sum);
    EXPECT_EQ(120, product);
}
#endif

#if defined(PHASE_B1_3) | defined(PHASE_B)
TEST(PhaseB1, Lazy) {
    // lazy evaluation test
    valarray <double> v1, v2, v3, v4;
    for (int i = 0; i<10; ++i) {
        v1.push_back(1.0);
        v2.push_back(1.0);
        v3.push_back(1.0);
        v4.push_back(1.0);
    }
    int cnt = InstanceCounter::counter;
    v1 + v2 - (v3 * v4);
    EXPECT_EQ(cnt, InstanceCounter::counter);

    valarray<double> ans(10);
    ans = v1 + v2 - (v3*v4);
    EXPECT_TRUE(match(ans[3], (v1[3] + v2[3] - (v3[3] * v4[3]))));
}
#endif

// Some random Foo
// Class Instrumentation
class Foo {
public:
    Foo(void){}
    ~Foo(void){}
    template<typename T>
    int operator+(T x){
        return 42;
    }
};

class Bar {
public:
    Bar(void){}
    ~Bar(void){}
};


#if defined(PHASE_C1_0) | defined(PHASE_C)
TEST(PhaseC1, Foo) {
    valarray<Foo> x(10);
    
    Foo f=Foo{};
    Bar b=Bar{};
    
    valarray <double> v1;
    for (int i = 0; i<10; ++i) {
        v1.push_back(1.0);
    }
      cout<<f+b<<endl;
    
    
//    cout<<x[0]<<endl;//can't compile
//    auto y=x+x;//can't compile
    
//    valarray<uint64_t> v2(10);//can compile
//    auto x=v1+v2;//can't compile because can't find matching "int my_rank"
}
#endif

#if defined(PHASE_C1_1) | defined(PHASE_C)
TEST(PhaseC1, const_iterator) {
    valarray<int> x(10);
    valarray<double> y(10);
    for (auto const & p : -y) {
        cout << p; // p should be a double
    }

//    auto p = (x + y).begin();
//    auto q = (x + y).end();
//    while (p != q) {
//        cout << *p;
//        ++p; }
}
#endif
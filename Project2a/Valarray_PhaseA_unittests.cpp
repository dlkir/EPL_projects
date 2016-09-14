/*
 * Valarray_PhaseA_unittests.cpp
 * EPL - Spring 2015
 */

#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>

#include "Valarray.h"
#include "gtest/gtest.h"

using std::cout;
using std::endl;
using std::string;
using std::complex;

using namespace epl;

template <typename X, typename Y>
bool match(X x, Y y) {
    double d = x - y;
    if (d < 0) { d = -d; }
    return d < 1.0e-9; // not really machine epsilon, but close enough
}

/*********************************************************************/
// Phase A Tests
/*********************************************************************/

#if defined(PHASE_A0_1) | defined(PHASE_A)
TEST(PhaseA, BracketOp) {
    valarray<complex<double>> x(10);
    for (int i = 0; i < 10; ++i)
        x[i] = complex<double>(i, i);

    auto y = x + x;//passes
    for (int i = 0; i<10; ++i) {
        complex<double> ans(i, i);
        ans += ans;
        EXPECT_TRUE(match(y[i].real(), ans.real()));
    }
    
    x = y;//passes
    int i = 0;
    for (auto yi : x) {
        complex<double> ans(i, i);
        ++i;
        ans += ans;
        EXPECT_TRUE(match(yi.imag(), ans.real()));
    }
    std::cout<<"x="<<x<<std::endl;
    std::cout<<"y="<<y<<std::endl;
    x = y;
    std::cout<<"x="<<x<<std::endl;
    std::cout<<"y="<<y<<std::endl;
    for(auto const & p : x+y){
        cout<<p;
    }
    
    
    std::cout<<x.accumulate(std::minus<complex<double>>(),0)<<std::endl;
    x=-1;
    std::cout<<x<<std::endl;
    
    double z=1.0;
    std::cout<<(x+z).size()<<std::endl;
    std::cout<<"flag3"<<std::endl;
    std::cout<<(x+z).sqrt()<<std::endl;
    
    auto p=x+y;//should work
    std::cout<<p.size()<<std::endl;
    //p=y+z;//reports error
    //p.push_back;//reports error
//    std::cout<<x.sum()<<std::endl;//should work
}
#endif

#if defined(PHASE_A0_0) | defined(PHASE_A)
TEST(PhaseA, Operators) {
    valarray<int> x(20);
    valarray<int> y(10);
    std::cout<<(x+y).size()<<std::endl;
    std::cout<<(x+y).sqrt()<<std::endl;
    x = -((2 * (1 + x - y + 1)) / 1);

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(-4, x[i]);
    }
    std::cout<<x.size()<<std::endl;
}
#endif

#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, Complex) {
    valarray<complex<float>> x(10);
    valarray<double> y = { 1.5, 2.5, 3.5 };

    for (int i = 0; i < 10; i++)
        x[i] = complex<float>(0.0, 0.0);

    auto z = y + x;
    auto r = y + y + z;
    EXPECT_EQ(r[0], complex<double>(4.5, 0));
    EXPECT_EQ(r[1], complex<double>(7.5, 0));
    EXPECT_EQ(r[2], complex<double>(10.5, 0));
}
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto out = RUN_ALL_TESTS();

#ifdef _MSC_VER
    system("pause");
#endif

    return out;
}

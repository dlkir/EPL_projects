// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h
#include "Vector.h"
#include <complex>
#include <cmath>
#include <iostream>

using namespace std::rel_ops;

//using std::vector; // during development and testing
using epl::vector; // after submission

namespace epl {
template<typename T, typename R> class valarray;
template<typename T> class scalar;

/***************************************************************/
//choose the "max" type...
/***************************************************************/
template <typename T>
struct rank;

template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };
template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };

template <int R>
struct stype;

template <> struct stype<1> { using type = int; };
template <> struct stype<2> { using type = float; };
template <> struct stype<3> { using type = double; };

template <typename T> struct is_complex : public std::false_type {};
template <typename T> struct is_complex<std::complex<T>> : public std::true_type {};

template <bool p, typename T> struct ctype;
template <typename T> struct ctype<true, T> { using type = std::complex<T>; };
template <typename T> struct ctype<false, T> { using type = T; };

//template<bool iscomp, typename T>
//using compType=typename ctype<iscomp,T>::type;


//for types
template<typename T>
class array_traits{
public:
        static constexpr int my_rank = rank<T>::value;
        static constexpr bool my_iscomp = is_complex<T>::value;
};
//for valarrays
template<typename T, typename R>
class array_traits<valarray<T,R>>{
public:
        static constexpr int my_rank = array_traits<T>::my_rank;
        static constexpr bool my_iscomp = array_traits<T>::my_iscomp;
};


template <typename T1, typename T2>
struct choose_type {
        static constexpr int t1_rank = array_traits<T1>::my_rank;
        static constexpr int t2_rank = array_traits<T2>::my_rank;
        static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;

        using my_stype = typename stype<max_rank>::type;

        static constexpr bool t1_comp = array_traits<T1>::my_iscomp;
        static constexpr bool t2_comp = array_traits<T2>::my_iscomp;
        static constexpr  bool my_comp = t1_comp || t2_comp;

        using type = typename ctype<my_comp, my_stype>::type;
};


/***************************************************************/
//data type conversion
/***************************************************************/
template <typename To, typename From, bool is_to_complex, bool is_from_complex>
struct convert;

template<typename To, typename From>
struct convert<To, From, true, true> {
    From val;
    convert(From _v) { val = _v; }
    To getValue() {
        return To{static_cast<typename stype<array_traits<To>::my_rank>::type>(val.real()), static_cast<typename stype<array_traits<To>::my_rank>::type>(val.imag())};
    }
};

template<typename To, typename From>
struct convert<To, From, true, false> {
    From val;
    convert(From _v) { val = _v; }
    To getValue() {
        return To{static_cast<typename stype<array_traits<To>::my_rank>::type>(val)};
    }
};

template<typename To, typename From>
struct convert<To, From, false, false> {
    From val;
    convert(From _v) { val = _v; }
    To getValue() {
        return static_cast<To>(val);
    }
};

template<typename To, typename From, typename R=convert<To, From, array_traits<To>::my_iscomp, array_traits<From>::my_iscomp>>
struct promote : public R {
    promote(From _v) : R(_v) {}
};

//iterator
template<typename Proxy, typename T>
class MyIterator {
public:
    using iterator = MyIterator<Proxy, T>;

    Proxy* proxy;
    uint64_t index;

    MyIterator(Proxy* _p, int _i) : proxy(_p), index(_i) {}

    T operator*() {
        return proxy->operator[](index);
    }

    // Pre-increment operator.
    MyIterator& operator++() {
        ++index;
        return *this;
    }

    // Post increment operator.
    MyIterator operator++(int) {
        iterator t{*this};
        ++index;
        return t;
    }

    // + operator.
    MyIterator operator+(int64_t n) {
        return iterator{proxy, index + n};
    }

    // - operator.
    MyIterator operator-(int64_t n) {
        return iterator{proxy, index - n};
    }

    // += operator.
    MyIterator& operator+=(int64_t n) {
        index += n;
        return *this;
    }

    // -= operator.
    MyIterator& operator-=(int64_t n) {
        index -= n;
        return *this;
    }

    // [] operator.
    T operator[](int64_t n) {
        return proxy->operator[](index + n);
    }

    // == operator
    bool operator == (const MyIterator& rhs) const {
        return this->index == rhs.index ? true : false;
    }

};



/***************************************************************/

//binary operation on two valarrays
template <typename BinaryOperation, typename T1, typename R1, typename T2, typename R2>
struct ParentProxy {
    using RetType = typename choose_type<T1, T2>::type;

    // Members.
    BinaryOperation op;
    const valarray<T1,R1>& left;
    const valarray<T2,R2>& right;
    // Constructor.
    ParentProxy(BinaryOperation _op, const valarray<T1,R1>& _l, const valarray<T2,R2>& _r) : op(_op), left(_l), right(_r) {}

    RetType operator[](uint64_t index) const {
        auto lValue = promote<RetType,T1>(this->left[index]);
        auto rValue = promote<RetType,T2>(this->right[index]);

        return this->op(lValue.getValue(), rValue.getValue());
    }

    // Chooose min size.
    uint64_t size() const {
        return std::min((uint64_t)left.size(), (uint64_t)right.size());
    }
};

template <typename BinaryOperation, typename T1, typename R1, typename T2>
struct ParentProxy<BinaryOperation,T1, R1,T2,scalar<T2>>
{
    using RetType = typename choose_type<T1, T2>::type;

    BinaryOperation op;
    const valarray<T1,R1>& left;
    scalar<T2> right;
    bool correct;

    ParentProxy(BinaryOperation _op, const valarray<T1,R1>& _l, const scalar<T2>& _r, bool _co) : op(_op), left(_l), right(_r), correct(_co) {}

    RetType operator[](uint64_t index) const {
        auto lValue = promote<RetType,T1>(this->left[index]);
        auto rValue = promote<RetType,T2>(this->right[index]);
	if(correct)
 	  return this->op(lValue.getValue(), rValue.getValue());
	else
	  return this->op(rValue.getValue(), lValue.getValue()); 
    }

    uint64_t size() const {
        return left.size();
    }

};



template <typename OP, typename T1, typename R1, typename T2=T1, typename R2=scalar<T2>, typename R=ParentProxy<OP, T1, R1, T2, R2>>
class Proxy : public R {

public:    
    using RetType = typename choose_type<T1, T2>::type;

    // Constructors.
    Proxy(OP _op, const valarray<T1,R1>& _l, const valarray<T2,R2>& _r) : R(_op, _l, _r) {}
    Proxy(OP _op, const valarray<T1,R1>& _l, const scalar<T2> & _r, bool correct) : R(_op, _l, _r, correct) {}


    using iterator = MyIterator<Proxy, RetType>;
    using const_iterator = MyIterator<Proxy, const RetType>;

    iterator begin() {
        return iterator(this, 0);
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    iterator end() {
        return iterator(this, this->size());
    }

    const_iterator end() const {
        return const_iterator(this, this->size());
    }
};


template <typename L, typename R, typename UnaryOperation>
struct UnaryOperationProxy {
    using RetType = typename UnaryOperation::result_type;

    // Members.
    const valarray<L, R>& val;
    UnaryOperation op;

    // Constructor.
    UnaryOperationProxy(const valarray<L, R>& _v, UnaryOperation _op) : val(_v), op(_op) {}

    // Element at operator.
    RetType operator[](uint64_t index) const {
        return op(this->val[index]);
    }

    // Return size.
    uint64_t size() const { return (uint64_t)val.size(); }

    //  Iterator related functions.
    using iterator = MyIterator<UnaryOperationProxy, L>;
    using const_iterator = MyIterator<UnaryOperationProxy, const L>;

    iterator begin() {
        return iterator(this, 0);
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    iterator end() {
        return iterator(this, this->size());
    }

    const_iterator end() const {
        return const_iterator(this, this->size());
    }
};




template<typename T>
class scalar{

private:
	const T& s;

public:
	scalar(const T& v):s(v){}

	T operator[](uint64_t k) const{
		return s;
	}

	uint64_t size() const{
		return -1;
	}

};


template <typename Arg, typename Result>
struct square_root : public std::unary_function<Arg, Result>  {
    Result operator() (const Arg& arg) const {
        return std::sqrt(arg);
    }
};



/***************************************************************/
//valarray class
/***************************************************************/
template <typename T, typename R=vector<T>>
class valarray : public R{

public:
	//constructors
	valarray(void):R(){}
	explicit valarray(uint64_t n):R(n){}
	valarray(const std::initializer_list<T>& that):R(that){}

	template <typename OP, typename T1, typename R1, typename T2, typename R2>
	valarray(OP op, const valarray<T1, R1>& x, const valarray<T2, R2>& y):R(op, x, y){}

	template <typename OP, typename T1, typename R1, typename T2>
	valarray(OP op, const valarray<T1, R1>& x, scalar<T2> y, bool order=true) : R(op, x, y, order) {}

	// This is for UnaryOperation Proxy.
	template <typename L1, typename R1, typename UnaryOperation>
	valarray(const valarray<L1, R1>& rhs, UnaryOperation op) : R(rhs, op) {}


    template <typename T1, typename R1>
    valarray(const valarray<T1, R1>& rhs) {
        for (int i=0; i < rhs.size(); ++i) {
            this->push_back((T)rhs[i]);
        }
    }

    
    // Assignment operator.
    valarray<T, R>& operator=(const valarray<T, R>& that) {
        if (this != &that) {
            for (int i=0; i < that.size()&& i<this->size(); ++i) {
                (*this)[i] = that[i];
            }
        }
        return *this;
    }

    // Assignment operator for valarray of different type.
    template <typename L1, typename R1>
    valarray<T, R>& operator=(const valarray<L1, R1>& that) {
	if(this->size()==0){
	    for (int i=0; i < that.size(); ++i)
		this->push_back((T)that[i]);
	}
	else{
	    for (int i=0; i < that.size()&& i<this->size(); ++i){(*this)[i] = (T)that[i];} //is type conversion OK?
        int m = std::max(this->size(),that.size())-std::min(this->size(),that.size()); for (int i=0; i< m;++i) {this->pop_back();}
	}
        return *this;
    }

    // Assignment a constant value to all the members of the valarray.
    valarray<T, R>& operator=(const T& val) {
        for (int i=0; i < this->size(); ++i) {
            this->operator[](i) = val;
        }
	return *this;
    }

    template<class OP>
    T accumulate(OP op, T init) {
        for (int i=0; i < this->size();  ++i) {
            init = op(init, (*this)[i]);
        }
        return init;
    }

    
    
    T sum() {
        T result{};
        return accumulate(std::plus<T>(), result);
    }

    template<typename uOP>
    valarray<typename uOP::result_type, UnaryOperationProxy<T, R, uOP>>
    apply(uOP op) const{
        return valarray<typename uOP::result_type, UnaryOperationProxy<T, R, uOP>>(*this, op);
    }
    //template <typename T1=typename choose_type<T, compType<array_traits<T>::my_iscomp, double> >::type>
    template <typename T1=typename ctype<array_traits<T>::my_iscomp, double>::type>
    valarray<T1, UnaryOperationProxy<T, R,  square_root<T, T1>>>
    sqrt() {
        return apply(square_root<T, T1>());
    }
    
    
    
};//class valarray
/***************************************************************/
//printing stuffs
/***************************************************************/

//print valarray.
template<typename T,typename R>
std::ostream& operator<<(std::ostream& out, const valarray<T,R>& val) {
	for (int i=0; i < val.size(); ++i) {
        	out << val[i]<<", ";
	}
	return out;
}
/*
//print Proxy
template<typename OP, typename T1, typename T2>
std::ostream& operator<<(std::ostream& out, const Proxy<OP,T1,T2>& val) {
        for (int i=0; i < val.size(); ++i) {
                out << val[i]<<", ";
        }
        return out;
}

template<typename OP, typename T1, typename T2, bool bb>
std::ostream& operator<<(std::ostream& out, const Proxy<OP,T1,T2,bb>& val) {
        for (int i=0; i < val.size(); ++i) {
                out << val[i]<<", ";
        }
        return out;
}
*/

/***************************************************************/
//operators
/***************************************************************/
//unary -
template <typename T1, typename R1>
valarray<T1, UnaryOperationProxy<T1, R1, std::negate<T1>> >
operator-(const valarray<T1, R1>& x) {
    return x.apply(std::negate<T1>());
}



//+
template <typename T1, typename R1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::plus<RetType>, T1, R1, T2, R2> >
operator+(const valarray<T1, R1>& x, const valarray<T2, R2>& y) {
    return valarray<RetType, Proxy<std::plus<RetType>, T1, R1, T2, R2>>(std::plus<RetType>(), x, y);
}

template <typename T1, typename R1, typename T2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::plus<RetType>, T1, R1, T2>>
operator+(const valarray<T1, R1>& x, const T2& y) {
    return valarray<RetType, Proxy<std::plus<RetType>, T1, R1, T2>>(std::plus<RetType>(), x, scalar<T2>(y), true);
}

template <typename T1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::plus<RetType>, T2, R2, T1> >
operator+(const T1& y, const valarray<T2, R2>& x) {
    return valarray<RetType, Proxy<std::plus<RetType>, T2, R2, T1>>(std::plus<RetType>(), x, scalar<T1>(y), false);
}

//-
template <typename T1, typename R1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::minus<RetType>, T1, R1, T2, R2> >
operator-(const valarray<T1, R1>& x, const valarray<T2, R2>& y) {
    return valarray<RetType, Proxy<std::minus<RetType>, T1, R1, T2, R2>>(std::minus<RetType>(), x, y);
}

template <typename T1, typename R1, typename T2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::minus<RetType>, T1, R1, T2>>
operator-(const valarray<T1, R1>& x, const T2& y) {
    return valarray<RetType, Proxy<std::minus<RetType>, T1, R1, T2>>(std::minus<RetType>(), x, scalar<T2>(y), true);
}

template <typename T1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::minus<RetType>, T2, R2, T1> >
operator-(const T1& y, const valarray<T2, R2>& x) {
    return valarray<RetType, Proxy<std::minus<RetType>, T2, R2, T1>>(std::minus<RetType>(), x, scalar<T1>(y), false);
}

//*
template <typename T1, typename R1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::multiplies<RetType>, T1, R1, T2, R2> >
operator*(const valarray<T1, R1>& x, const valarray<T2, R2>& y) {
    return valarray<RetType, Proxy<std::multiplies<RetType>, T1, R1, T2, R2>>(std::multiplies<RetType>(), x, y);
}

template <typename T1, typename R1, typename T2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::multiplies<RetType>, T1, R1, T2>>
operator*(const valarray<T1, R1>& x, const T2& y) {
    return valarray<RetType, Proxy<std::multiplies<RetType>, T1, R1, T2>>(std::multiplies<RetType>(), x, scalar<T2>(y), true);
}

template <typename T1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::multiplies<RetType>, T2, R2, T1> >
operator*(const T1& y, const valarray<T2, R2>& x) {
    return valarray<RetType, Proxy<std::multiplies<RetType>, T2, R2, T1>>(std::multiplies<RetType>(), x, scalar<T1>(y), false);
}

// /
template <typename T1, typename R1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::divides<RetType>, T1, R1, T2, R2> >
operator/(const valarray<T1, R1>& x, const valarray<T2, R2>& y) {
    return valarray<RetType, Proxy<std::divides<RetType>, T1, R1, T2, R2>>(std::divides<RetType>(), x, y);
}

template <typename T1, typename R1, typename T2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::divides<RetType>, T1, R1, T2>>
operator/(const valarray<T1, R1>& x, const T2& y) {
    return valarray<RetType, Proxy<std::divides<RetType>, T1, R1, T2>>(std::divides<RetType>(), x, scalar<T2>(y), true);
}

template <typename T1, typename T2, typename R2, typename RetType=typename choose_type<T1, T2>::type>
valarray<RetType, Proxy<std::divides<RetType>, T2, R2, T1> >
operator/(const T1& y, const valarray<T2, R2>& x) {
    return valarray<RetType, Proxy<std::divides<RetType>, T2, R2, T1>>(std::divides<RetType>(), x, scalar<T1>(y), false);
}

}//namespace epl
#endif /* _Valarray_h */


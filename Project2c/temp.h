template<typename R, typename T> struct binop_OK<T,vec_wrap<R>> : public is_numeric_type<T>{};
template<typename T1, typename T2> constexpr bool BinopOK(void) { return binop_OK<T1,T2>::value; }
    
template<typename T1, typename T2, template<typename> class Op> struct result_type;
template<typename T, typename R, template<typename> class Op>
    struct result_type<T,vec_wrap<R>,Op> : binop_choose_type<Scalarwrap<T>,R,Op> {};
template<typename T, typename R, template<typename> class Op>
    struct result_type<vec_wrap<R>,T,Op> : binop_choose_type<R,Scalarwrap<T>,Op> {};
template<typename R1, typename R2, template<typename> class Op>
    struct result_type<vec_wrap<R1>,vec_wrap<R2>,Op> : binop_choose_type<R1,R2,Op> {};
template <typename T1, typename T2, template<typename> class Op>
using ResultType=typename result_type<T1,T2,Op>::type
//put the above in valarray

template<typename T1,typename T2>
EnableIf<valarray::BinopOK<T1,T2>(),valarray::ResultType<T1, T2, std::plus>>
operator+(const T1& x, const T2& y) {
    return valarray::ResultType<T1,T2,std::plus>{x,y};
}
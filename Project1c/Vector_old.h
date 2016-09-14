#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>


//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;
//using std::cout;
//using std::endl;
namespace epl{

class invalid_iterator {
	public:
	enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
	SeverityLevel level;	

	invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
	virtual const char* what() const {
    switch(level){
      case WARNING:   return "Warning"; // not used in Spring 2015
      case MILD:      return "Mild";
      case MODERATE:  return "Moderate";
      case SEVERE:    return "Severe";
      default:        return "ERROR"; // should not be used
    }
	}
};

template <typename T>
class vector;
    
/***************************   iterator   ***************************/
template <typename T>
class iterator 
{
    
private:
    uint64_t _pos;
    vector<T> *_p_vector;

public:
    typedef T value_type;
    typedef typename std::random_access_iterator_tag iterator_category;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
    
    iterator<T>(void){}
    
    iterator<T>(vector<T>* p_vector, uint64_t pos)
    : _pos(pos)
    , _p_vector (p_vector)
    {printf("lalala iterator\n");}
    
//the 6~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    bool operator != (const iterator<T>& other) const {
        return _pos != other._pos;
    }
    bool operator == (const iterator<T>& other) const {
        std::cout<<"_pos="<<_pos<<"other._pos"<<other._pos<<std::endl;
        return _pos == other._pos;
    }
    bool operator < (const iterator<T>& other) const{
        return _pos < other._pos;
    }
    
    bool operator > (const iterator<T>& other) const {
        return _pos > other._pos;
    }
    bool operator <= (const iterator<T>& other) const {
        return _pos <= other._pos;
    }
    bool operator >= (const iterator<T>& other) const {
        return _pos >= other._pos;
    }
//the 6 done~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    iterator<T>& operator + (const uint64_t num) {
        _pos = _pos + num;
        return *this;
    }
    
    iterator<T>& operator += (const uint64_t num) {
        _pos = _pos + num;
        return *this;
    }
    
    iterator<T>& operator - (const uint64_t num) {
        _pos = _pos - num;
        return *this;
    }
    
    iterator<T>& operator -= (const uint64_t num) {
        _pos = _pos - num;
        return *this;
    }
    
    uint64_t operator - (const iterator<T>& other) {
        return _pos - other._pos;
    }
//the 5 done~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    T& operator*(void) const{
        return _p_vector->get(_pos);
    }
    
    T& operator[](uint64_t num) const{
        return _p_vector->get(num);
    }

    iterator<T>& operator++(void){
    ++_pos;
    return *this;
    }
    
    iterator<T> operator++(int){
        iterator t(*this);
        this->operator++();
        return t;
    }
    
    iterator<T>& operator--(void){
        --_pos;
        return *this;
    }
    
    iterator<T> operator--(int){
        iterator t(*this);
        this->operator--();
        return t;
    }
    

};

/***************************   const_iterator   ***************************/
template <typename T>
class const_iterator
{
//        friend class vector;
private:
        uint64_t _pos;
        const vector<T> *_p_vector;

public:
    typedef T value_type;
    typedef typename std::random_access_iterator_tag iterator_category;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
    
    const_iterator<T>(void){}

    const_iterator<T>(const vector<T>* p_vector, uint64_t pos)
    : _pos(pos)
    , _p_vector (p_vector)
    {printf("lalala const_iterator\n");}

    //the 6~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    bool operator != (const const_iterator<T>& other) const{
        return _pos != other._pos;
    }
    bool operator == (const const_iterator<T>& other) const{
        std::cout<<"_pos="<<_pos<<"other._pos"<<other._pos<<std::endl;
        return _pos == other._pos;
    }
    bool operator < (const const_iterator<T>& other) const{
        return _pos < other._pos;
    }
    
    bool operator > (const const_iterator<T>& other) const{
        return _pos > other._pos;
    }
    bool operator <= (const const_iterator<T>& other) const{
        return _pos <= other._pos;
    }
    bool operator >= (const const_iterator<T>& other) const{
        return _pos >= other._pos;
    }
    //the 6 done~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    const_iterator<T>& operator + (const uint64_t num) {
        _pos = _pos + num;
        return *this;
    }
    
    const_iterator<T>& operator - (const uint64_t num) {
        _pos = _pos - num;
        return *this;
    }
    
    uint64_t operator - (const const_iterator<T>& other) const{
        return _pos - other._pos;
    }

    const T& operator*(void) const {
            return _p_vector->get(_pos);
    }

    const_iterator<T>& operator++(void){
            ++_pos;
            return *this;
    }

    const_iterator<T> operator++(int){
        const_iterator t(*this);
        this->operator++();
        return t;
        }
    
    const_iterator<T>& operator--(void){
        --_pos;
        return *this;
    }
    
    const_iterator<T> operator--(int){
        const_iterator t(*this);
        this->operator--();
        return t;
    }
    
};

template <typename T>
class vector{
private:
	uint64_t capacity; // of array
	uint64_t lenH; //# at head of the vector, ending at storage+capacity-1
	uint64_t lenT; //# at tail of the vector, starting from storage
	T* storage;
    uint64_t moderate;
	uint64_t mild;
	uint64_t severe;
public:
	vector<T>(void) {
        storage = (T*)operator new(8*sizeof(T));
        //storage = operator new T[8];
        capacity = 8;
        lenH = 0;
        lenT = 0;
        mild = 0;
        moderate = 0;
        severe = 0;
    };
    
	explicit vector<T> (uint64_t n) {
		if(n>0){
			//storage = new T[n];
			storage = (T*)operator new(n*sizeof(T));
			capacity = n;
			lenH = 0;
			lenT = n;
			for (uint64_t k = 0; k < n; k += 1) {
				new (storage+k) T{};
			}
		}
		else{
			storage = (T*)operator new(8*sizeof(T));
			//storage = operator new T[8];
			capacity = 8;
			lenH = 0;
			lenT = 0;
		}
        mild = 0;
		moderate = 0;
		severe = 0;
	};
    
    template <typename I>
    void vec(I b, I e, std::input_iterator_tag){
        if(b==e){
            storage = (T*)operator new(8*sizeof(T));
			//storage = operator new T[8];
			capacity = 8;
			lenH = 0;
			lenT = 0;
        }else{
            storage = (T*)operator new(64*sizeof(T));
			//storage = operator new T[8];
			capacity = 64;
			lenH = 0;
			lenT = 0;
            for(int i=0;(b+i)!=e;i++){
                new (storage+i) T(*(b+i));
                lenT++;
            }
        }
    }
    
    template <typename I>
    void vec(I b, I e, std::random_access_iterator_tag t) {
        if(b==e){
            storage = (T*)operator new(8*sizeof(T));
			//storage = operator new T[8];
			capacity = 8;
			lenH = 0;
			lenT = 0;
        }else{
            capacity = e-b;
            storage = (T*)operator new(capacity*sizeof(T));
			lenH = 0;
			lenT = 0;
            for(int i=0;(b+i)!=e;i++){
                new (storage+i) T(*(b+i));
                lenT++;
            }
        }
    }
    
    template <typename I>
    vector<T>(I b, I e) {
        typename std::iterator_traits<I>::iterator_category x{};
        vec(b, e, x);
    }
    
	//copy constructor
	vector<T> (const vector<T>& that) {copy(that);moderate = 1;}
    
	//move constructor
	vector<T> (vector<T>&& that) {
		this->storage = that.storage;
		that.storage=nullptr;
		lenH=that.lenH;that.lenH=0;
		lenT=that.lenT;that.lenT=0;
		capacity=that.capacity;that.capacity=0;
        severe=1;
	}
	//copy assignment
	vector<T>& operator= (const vector<T>& that) {
		if (this != &that) {
			destroy();
			copy(that);
            moderate=1;
		}
		return *this;
	}
	//move assignment
	vector<T>& operator= (vector<T>&& that) {
		if (this != &that){
			destroy();
			this->storage = that.storage;
			that.storage=nullptr;
			lenH=that.lenH;that.lenH=0;
			lenT=that.lenT;that.lenT=0;
			capacity=that.capacity;that.capacity=0;
            severe = 1;
		}
		return *this;
	}
    
	~vector<T> (void) {
		destroy();
	}
    
	uint64_t size(void) const {
		return lenH+lenT;
	}
    
	T& operator[](uint64_t k){
		if (k >= this->size()) { throw std::out_of_range{"index out of range"}; }
		if (k < lenH){
			return *(storage+capacity-lenH+k);
		}
		else {
			return *(storage+k-lenH);
		}
		
	}
	
	const T& operator[](uint64_t k) const {
		if (k >= this->size()) { throw std::out_of_range{"index out of range"}; }
		if (k < lenH){
			const T* tmp = (storage+capacity-lenH+k);
			return *tmp;
		}
		else {
			const T* tmp =  (storage+k-lenH);
			return *tmp;
		}
	}
	///////////////////////////////////////////////////
	void push_back(const T& x){
		if (this->size()==capacity){
			T* storage_new=(T*)operator new(2*capacity*sizeof(T));
			new (storage_new+lenT) T(x);
			for (uint64_t k = 0; k < lenT; k += 1) {
				new (storage_new+k) T(std::move(*(storage+k)));
			}
			for (uint64_t k = 0; k < lenH; k += 1) {
				new (storage_new+capacity*2-1-k) T(std::move(*(storage+capacity-1-k)));
			}
			capacity = capacity*2;
			this->storage=storage_new;
		}else{
			new (storage+lenT) T(x);//append the new value to tail
		}
		lenT=lenT+1;
	}
    
	void push_back(T&& x){
		if (this->size()==capacity){
			T* storage_new=(T*)operator new(2*capacity*sizeof(T));
			new (&storage_new[lenT]) T((T&&) x);//append the new value to tail
			for (uint64_t k = 0; k < lenT; k += 1) {
				new (&storage_new[k]) T((T&&) storage[k]);
			}
			for (uint64_t k = 0; k < lenH; k += 1) {
				new (&storage_new[capacity*2-1-k]) T((T&&) storage[capacity-1-k]);
			}
			capacity = capacity*2;
			this->storage=storage_new;
		}else{
			new (&storage[lenT]) T((T&&) x);//append the new value to tail
		}
		lenT=lenT+1;
	}
	///////////////////////////////////////////////////
	void push_front(const T& x){
		if (this->size()==capacity){
			T* storage_new=(T*)operator new(2*capacity*sizeof(T));
			new (storage_new+2*capacity-1-lenH) T(x);//append the new value to head
			for (uint64_t k = 0; k < lenT; k += 1) {
				new (storage_new+k) T(std::move(*(storage+k)));
			}
			for (uint64_t k = 0; k < lenH; k += 1) {
				new (storage_new+capacity*2-1-k) T(std::move(*(storage+capacity-1-k)));
			}
			capacity = capacity*2;
			this->storage=storage_new;
		}else{
			new (storage+capacity-1-lenH) T(x);//append the new value to head
		}
		lenH=lenH+1;
	}
	
	void push_front(T&& x){
		if (this->size()==capacity){
			T* storage_new=(T*)operator new(2*capacity*sizeof(T));
			new (storage_new+2*capacity-1-lenH) T(std::move(x));
			for (uint64_t k = 0; k < lenT; k += 1) {
				new (storage_new+k) T(std::move(*(storage+k)));
			}
			for (uint64_t k = 0; k < lenH; k += 1) {
				new (storage_new+capacity*2-1-k) T(std::move(*(storage+capacity-1-k)));
			}
			capacity = capacity*2;
			this->storage=storage_new;
		}else{
			new (storage+capacity-1-lenH) T(std::move(x));
		}
		lenH=lenH+1;
	}
	///////////////////////////////////////////////////
	void pop_back(void){
		if (this->size()==0) { throw std::out_of_range{"nothing to remove"}; }
		if (lenT==0){
			uint64_t lenH1=lenH/2;
			uint64_t lenT1=lenH-lenH1;
			for (uint64_t k = 0; k < lenT1; k += 1){
				new (storage+k) T(*(storage+capacity-lenT1+k));
				storage[capacity-lenT1+k].~T();
			}
			for (uint64_t k = 0; k < lenH1; k += 1){
				new (storage+capacity-1-k) T(*(storage+capacity-1-k-lenT1));
				storage[capacity-1-k-lenT1].~T();
			}
			lenT=lenT1;
			lenH=lenH1;
		}
		storage[lenT-1].~T();
		lenT=lenT-1;
        mild = 1;
	}
    
	void pop_front(void){
		if (this->size()==0) { throw std::out_of_range{"nothing to remove"}; }
		if (lenT==capacity){
			storage[0].~T();
			lenH=lenT-1;
			lenT=0;
			return;
		}
		if (lenH==0){
			uint64_t lenT1=lenT/2;
			uint64_t lenH1=lenT-lenT1;
			for (uint64_t k = 0; k < lenH1; k += 1){
				new (storage+capacity-lenH1+k) T(*(storage+k));
				storage[k].~T();
			}
			for (uint64_t k = 0; k < lenT1; k += 1){
				new (storage+k) T(*(storage+k+lenH1));
				storage[k+lenH1].~T();
			}
			lenT=lenT1;
			lenH=lenH1;
		}
		storage[capacity-lenH].~T();
		lenH=lenH-1;
	}
    
    vector<T> (const std::initializer_list<T>& that) {
        storage = (T*)operator new(8*sizeof(T));
        capacity = 8;
        lenH = 0;
        lenT = 0;
        for (typename std::initializer_list<T>::iterator i=that.begin();i!=that.end();i++){
            push_back(*i);
        }
    }
    
    T& get(uint64_t pos){
        invalid_iterator invalid;
        
        if(pos == this->size() && this->size() != 0)
            return (*this)[0];
        else if(pos>=lenT && (pos<capacity-lenH || lenH ==0)){
            if(mild == 1 && this->size() != 0)
                invalid.level = invalid_iterator::SeverityLevel::MILD;
            else
                invalid.level = invalid_iterator::SeverityLevel::SEVERE;
            throw invalid;
        }
        else if(mild == 1){
            invalid.level = invalid_iterator::SeverityLevel::MILD;
            throw invalid;
            mild = 0;
        }
        else if(moderate == 1){
            invalid.level = invalid_iterator::SeverityLevel::MODERATE;
            throw invalid;
            moderate = 0;
        }
        else if(severe == 1){
            invalid.level = invalid_iterator::SeverityLevel::SEVERE;
            throw invalid;
            severe = 0;
        }
        
        else
            return (*this)[pos];
    }
    
    const T& get(uint64_t pos) const{
        invalid_iterator invalid;
        
        if(pos == this->size() && this->size() != 0)
            return (*this)[0];
        else
            return (*this)[pos];
    }
    
	iterator<T> begin() {
		return iterator<T>(this, 0);
	}

	const const_iterator<T> begin() const {
        return const_iterator<T>(this, 0);
    }

	iterator<T> end() {
		return iterator<T>(this, this->size());
	}

	const const_iterator<T> end() const {
        return const_iterator<T>(this, this->size());
    }
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
	void copy(const vector& that) {
		capacity = that.capacity;
		this->storage=(T*)operator new(capacity*sizeof(T));
		lenH=that.lenH;
		lenT=that.lenT;
		for (uint64_t k = 0; k < lenT; k += 1) {
			new (storage+k) T(*(that.storage+k));
		}
		for (uint64_t k = 0; k < lenH; k += 1) {
			new (storage+capacity-1-k) T(*(that.storage+capacity-1-k));
		}
	}
	void destroy(void) {
		for (uint64_t k = 0; k < lenT; k += 1) {
			storage[k].~T();
		}
		for (uint64_t k = 0; k < lenH; k += 1) {
			storage[capacity-1-k].~T();
		}
		::operator delete(storage);
	}
};

} //namespace epl

#endif

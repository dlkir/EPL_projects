// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

namespace epl{

template <typename T>
class vector {
private:	
	uint64_t capacity; // of array
	uint64_t lenH; //# at head of the vector, ending at storage+capacity-1
	uint64_t lenT; //# at tail of the vector, starting from storage
	T *storage;
public:
	//vector<T>(void) : vector<T>(0){};

	vector<T>(void) {
			storage = (T*)operator new(8*sizeof(T));
			//storage = operator new T[8];			
			capacity = 8;
			lenH = 0;
			lenT = 0;
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
	};

	vector<T> (const vector<T>& that) {copy(that);} 

	vector<T>& operator= (const vector<T>& that) {
		if (this != &that) {
			destroy();
			copy(that);
		}
		return *this;
	}

	~vector<T> (void) {destroy();}

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
	
	void push_back(const T& x){//???????????
		if (this->size()==capacity){
			T* storage_new=(T*)operator new(2*capacity*sizeof(T));
			for (uint64_t k = 0; k < lenT; k += 1) {
				new (storage_new+k) T(*(storage+k));
			}
			for (uint64_t k = 0; k < lenH; k += 1) {
				new (storage_new+capacity*2-1-k) T(*(storage+capacity-1-k));
			}			
			capacity = capacity*2;
			this->storage=storage_new;
		}
		new (storage+lenT) T(x);//append the new value to tail
		lenH=lenH;//??????????
		lenT=lenT+1;
	}

	void push_front(const T& x){//???????????
		if (this->size()==capacity){
			T* storage_new=(T*)operator new(2*capacity*sizeof(T));
			for (uint64_t k = 0; k < lenT; k += 1) {
				new (storage_new+k) T(*(storage+k));
			}
			for (uint64_t k = 0; k < lenH; k += 1) {
				new (storage_new+capacity*2-1-k) T(*(storage+capacity-1-k));
			}
			capacity = capacity*2;
			this->storage=storage_new;
		}
		new (storage+capacity-1-lenH) T(x);//append the new value to head
		lenH=lenH+1;//??????????
		lenT=lenT;
	}

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

#endif /* _Vector_h */

// Exercise with operator overloading

// g++ -std=c++11 objects.cpp -o objects

#include <iostream>

// arithmetic type example (concrete)
class complex {
    double re, im;
public:
// constructors
complex(double r, double i) :re{r}, im{i} {}
complex(double r) :re{r}, im{0} {}
complex() :re{0}, im{0} {}

double real() const {return re;}
void real(double d) {re=d;}
double imag() const {return im;}
void imag(double i) {im=i;} 

complex& operator+=(complex z) {
    re+=z.re;
    im+=z.im;
    return *this;
}
complex& operator-=(complex z) {
    re-=z.re;
    im-=z.im;
    return *this;
}
complex& operator*=(complex z) {
    // (a+ib)*(c+id)
    double r = re*z.re - im*z.im;
    double i = re*z.im + im*z.re;
    re = r;
    im = i;
    return *this;
}
complex& operator/=(complex z) {
    double mag = z.re*z.re + z.im*z.im;
    double r = (re*z.re + im*z.im) / mag;
    double i = (im*z.re - re*z.im) / mag;
    re = r;
    im = i;
    return *this;
}
};

// use fact that arg passed by value is copied so modify argument without affecting caller's copy
complex operator+(complex a, complex b) {return a+=b;}
complex operator-(complex a, complex b) {return a-=b;}
complex operator-(complex a) {return {-a.real(), -a.imag()};} // negation
complex operator*(complex a, complex b) {return a*=b;}
complex operator/(complex a, complex b) {return a/=b;}
bool operator==(complex a, complex b) {return a.real()==b.real() && a.imag()==b.imag();}
bool operator!=(complex a, complex b) {return !(a==b);}

// container type example (concrete)
// C++ does not offer garage collection. We need deconstructors.
class Vector {
public:
    // "Resource Acquisition is Initialization"
    Vector();
    // new operator allocates memory on the "free store" (heap or dynamic memory)
    Vector(int s) :elem{new double[s]}, sz{s} { 
        for (int ii = 0; ii < s; ++ii) {
            elem[ii] = 0;
        }
    }
    Vector(std::initializer_list<double> lst)
        // std uses unsigned int for size and subscripts so static_cast is needed
        :elem{new double[lst.size()]}, sz{static_cast<int>(lst.size())} {
            // copy lst into elem
            std::copy(lst.begin(), lst.end(), elem);
        }

    // delete operator frees the memory
    // delete for objects vs. delete[] or arrays
    ~Vector() {delete[] elem;}

    double& operator[](int i);
    int size() const;
private:
    // elem points to an array of sz doubles
    double* elem;
    int sz;
};

// container type example (abstract)
class Container {
public:
    // no constructor, but virtual deconstructor allows redefinitions
    // virtual keyword means may be redefined in a class derived from this one
    // the =0 means this is pure virtual function, meaning the derived class must define it
    // abstract classes have all virtual functions
    virtual double& operator[](int) = 0; // index operator
    virtual int size() const = 0; // const member function
    virtual ~Container() {}
};

// Objects are constructed base-first and destroyed derived-first

class VectorContainer : public Container {
public:
    VectorContainer(int s) : v(s){}
    ~VectorContainer() {}
    // use explicit override keyword to catch mistakes
    double& operator[](int idx) override {return v[idx];}
    int size() const override {return v.size();}
private:
    Vector v;
};

int main() {

    //complex a = complex(1.0,2.0);
    //complex b = {1.0,2.0};
    //const complex cz {1,3};
    //b = cz; // okay, assigning to non-const
    //cz = b  // error
    //double x = b.real();
    // std::cout << x << std::endl;
    // complex a(2);
    // complex b(1/a);
    // std::cout << b.real() << std::endl;

    // global; destroyed end of program
    Vector gv(10);
    // Vector on free store; never implicitly destroyed
    Vector* gp = new Vector(100);

    return 0;
}
#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include <iostream>
#include <vector>
#include <memory>

class TestClass {
public:
    TestClass(int a, int b) {
        std::cout << "Konstruktor " << a << " " << b << std::endl; 
    }

    float testMethod() {
        std::cout << "Test method" << std::endl;
        return 3.141529;
    }

    int complicatedFunction(const std::vector<int>& array) {
        
        int sum = 0;
        for (auto a : array) {
            std::cout << a << " ";
            sum += a;
        }

        std::cout << std::endl;

        return sum;
    }
};

int Geant4_init();
int Geant4_GDML();
int Geant4_run();

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(drugi_mod) {
    emscripten::class_<TestClass>("TestClass")
        .constructor<int, int>()
        .smart_ptr<std::shared_ptr<TestClass>>("TestClass")
        .function("testMethod", &TestClass::testMethod)
        .function("complicatedFunction", &TestClass::complicatedFunction);

    emscripten::register_vector<int>("vector_int");
}
#endif
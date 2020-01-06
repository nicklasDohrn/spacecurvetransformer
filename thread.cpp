// thread example
#include <iostream>       // std::cout
#include <thread>         // std::thread
 
int arr [4];
static const int num_threads = 4;
void foo(int pos) 
{
  arr[pos] = pos;
}

int main() 
{
    std::thread t[num_threads];
       //Launch a group of threads
    for (int i = 0; i < num_threads; ++i) {
        t[i] = std::thread(foo, i);
    } 

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
        t[i].join();         
    }
    std::cout << arr[0] << " " << arr[1] << std::endl;
 
    return 0;
}
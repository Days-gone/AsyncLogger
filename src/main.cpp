#include <iostream>
#include "SafeQueue.hpp"

using namespace std;

int main() {
    SafeQueue<int> q(10);
    q.enqueue(1);
    int val = q.dequeue();
    cout << "Dequeued value: " << val << endl;
    return 0;
}
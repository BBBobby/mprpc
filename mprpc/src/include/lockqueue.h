#ifndef LOCKQUEUE_H
#define LOCKQUEUE_H
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
using namespace std;

template<typename T>
class LockQueue{
public:
    void Push(const T& msg){
        {       
            lock_guard<mutex> lock(mtx);
            que.push(msg);
        }
        cv.notify_one();
    }
    T Pop(){
        unique_lock<mutex> lock(mtx);
        while(que.empty()){
            cv.wait(lock);
        }
        T msg = que.front();
        que.pop();
        return msg;
    }
private:
    queue<T> que;
    mutex mtx;
    condition_variable cv;
};

#endif
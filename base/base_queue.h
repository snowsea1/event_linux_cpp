#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "base_common.h"

template <typename T, typename Container = std::queue<T>>
class ThreadQueue
{
public:
    using container_type           = Container;
    using value_type               = typename Container::value_type;
    using reference                = typename Container::reference;
    using const_reference          = typename Container::const_reference;
    using size_type                = typename Container::size_type;
    using mutex_type               = std::mutex;
    using condition_variable_type  = std::condition_variable;
private:
    Container                queue_;
    mutable mutex_type       mutex_;
    condition_variable_type  cond_;
    
public:
    ThreadQueue() = default;
    ThreadQueue( const ThreadQueue & ) = delete;
    ThreadQueue &operator=( const ThreadQueue & ) = delete;
    void pop( reference elem, long long msleep )
    {
#if 0
        std::unique_lock<mutex_type> lock( mutex_ );
        // cond_.wait( lock, [this]() {  return !queue_.empty();  } );
#endif

        std::unique_lock<mutex_type> lock( mutex_ );
        if (queue_.empty()) {
            cond_.wait_for( lock,  std::chrono::milliseconds(msleep)) == std::cv_status::timeout;
        } else {
            elem = std::move(queue_.front());
            queue_.pop();
        }

    }
    bool try_pop( reference elem )
    {
        std::unique_lock<mutex_type> lock( mutex_ );
        if( queue_.empty() ) {
            return false;
        }
        elem = std::move( queue_.front() );
        queue_.pop();
        return true;
    }
    
    bool empty() const
    {
        std::lock_guard<mutex_type> lock( mutex_ );
        return queue_.empty();
    }
    
    size_type size() const
    {
        std::lock_guard<mutex_type> lock( mutex_ );	
        return queue_.size();
    }
    
    void push( const value_type &elem )
    {
        {
            std::lock_guard<mutex_type> lock( mutex_ );
            queue_.push( elem );
        }
        cond_.notify_one();
    }
    void push( value_type &&elem )
    {
        {
            std::lock_guard<mutex_type> lock( mutex_ );
            queue_.push( std::move( elem ) );
        }
        cond_.notify_one();
    }  
};
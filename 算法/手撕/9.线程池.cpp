#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <atomic>
#include <chrono>

using callback = std::function<void(void*)>;

// 任务类
class Task {
public:
    callback fun;
    void* arg;

    Task() : fun(nullptr), arg(nullptr) {}
    Task(callback f, void* a) : fun(f), arg(a) {}
};

// 任务队列类
class TaskQueue {
private:
    std::queue<Task> m_queue;
    std::mutex m_mutex;

public:
    // 添加任务
    void add_task(Task& task) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(task);
    }

    void add_task(callback f, void* arg) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.emplace(f, arg);
    }

    // 取出任务
    Task get_task() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_queue.empty()) {
            Task task = m_queue.front();
            m_queue.pop();
            return task;
        }
        return Task();
    }

    // 获取任务队列大小
    size_t task_size() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};

// 线程池类
class ThreadPool {
private:
    std::mutex m_mutex_pool;
    std::condition_variable m_not_empty;
    
    std::vector<std::thread> m_threads;
    std::thread m_manager_thread;
    std::unique_ptr<TaskQueue> m_task_queue;

    int m_min_num;  //最小线程数
    int m_max_num;  //最大线程数
    std::atomic<int> m_busy_num;  //工作的线程数
    std::atomic<int> m_live_num;  //存活的线程数一
    std::atomic<int> m_destory_num; //待销毁的线程数
    std::atomic<bool> m_shutdown; //是否销毁

    // 管理线程的任务函数
    void manager() {
        while (!m_shutdown) {
            std::this_thread::sleep_for(std::chrono::seconds(5));

            int queue_size = m_task_queue->task_size();
            int live_size = m_live_num.load();
            int busy_size = m_busy_num.load();

            // 创建线程，最多创建两个
            if (queue_size > live_size && live_size < m_max_num) {
                std::lock_guard<std::mutex> lock(m_mutex_pool);
                int count = 0;
                for (int i = 0; i < m_max_num && count < 2 && m_live_num.load() < m_max_num; ++i) {
                    if (i >= m_threads.size() || !m_threads[i].joinable()) {
                        m_threads.emplace_back(&ThreadPool::worker, this);
                        ++count;
                        ++m_live_num;
                    }
                }
            }

            // 销毁多余的线程
            if (2 * busy_size < live_size && live_size > m_min_num) {
                std::lock_guard<std::mutex> lock(m_mutex_pool);
                m_destory_num = 2;
                m_not_empty.notify_all();
            }
        }
    }

    // 工作线程的任务函数
    void worker() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_mutex_pool);
                m_not_empty.wait(lock, [this] { return m_task_queue->task_size() > 0 || m_shutdown || m_destory_num > 0; });

                if (m_destory_num > 0) {
                    --m_destory_num;
                    if (m_live_num.load() > m_min_num) {
                        --m_live_num;
                        return;
                    }
                }

                if (m_shutdown) {
                    return;
                }

                task = m_task_queue->get_task();
                if (task.fun) {
                    ++m_busy_num;
                }
            }

            if (task.fun) {
                std::cout << "thread " << std::this_thread::get_id() << " start working.....\n";
                task.fun(task.arg);
                delete task.arg;
                std::cout << "thread " << std::this_thread::get_id() << " end work.....\n";
                --m_busy_num;
            }
        }
    }

public:
    // 构造函数
    ThreadPool(int min, int max) : m_min_num(min), m_max_num(max), m_busy_num(0), m_live_num(min), m_destory_num(0), m_shutdown(false) {
        m_task_queue = std::make_unique<TaskQueue>();

        // 创建工作线程
        for (int i = 0; i < m_min_num; ++i) {
            m_threads.emplace_back(&ThreadPool::worker, this);
        }

        // 创建管理者线程
        m_manager_thread = std::thread(&ThreadPool::manager, this);
    }

    // 析构函数
    ~ThreadPool() {
        m_shutdown = true;
        m_not_empty.notify_all();

        if (m_manager_thread.joinable()) {
            m_manager_thread.join();
        }

        for (auto& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // 添加任务
    void add_task(Task task) {
        if (m_shutdown) {
            std::cout << "the thread pool will destory!\n";
            return;
        }
        m_task_queue->add_task(task);
        m_not_empty.notify_one();
    }

    // 获取忙碌线程数量
    int get_busy_num() {
        return m_busy_num.load();
    }

    // 获取存活线程数量
    int get_live_num() {
        return m_live_num.load();
    }
};

// 示例任务函数
void test_task(void* arg) {
    int* num = static_cast<int*>(arg);
    std::cout << "Task with number " << *num << " is running." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
    ThreadPool pool(2, 5);

    // 添加任务
    for (int i = 0; i < 10; ++i) {
        int* num = new int(i);
        Task task(test_task, num);
        pool.add_task(task);
    }

    std::this_thread::sleep_for(std::chrono::seconds(10)); // 等待任务完成

    return 0;
}

#ifndef WORKER_H
#define WORKER_H

#include <mutex>
#include <thread>
#include <string>

class Fenetre;
class Worker
{
public:
    Worker();
    void do_work(Fenetre *caller);
    void stop_work();
    void get_data(double *fraction_done);
    bool has_stopped() const;

private:
    mutable std::mutex m_Mutex;
    bool m_has_stopped;
    long long source_size;
    float m_fraction_done;
    long long f_size_usb;
    long long out();
};

#endif
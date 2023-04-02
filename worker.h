#ifndef worker_H
#define worker_H

#include <QObject>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <QImage>

struct info_render
{
    info_render() = default;
    info_render(info_render const&) = default;

    info_render(std::pair<double, double> m_start, std::pair<int, int> m_size, double m_scale, int m_block_size)
        : m_start(m_start)
        , m_size(m_size)
        , m_scale(m_scale)
        , m_block_size(m_block_size)
    {}

    std::pair<double, double> m_start;
    std::pair<int, int> m_size;
    double m_scale;
    int m_block_size;
};


class info_worker
{
public:
    info_worker() {}

    info_worker(info_worker const&) = delete;
    info_worker(const info_render & info)
        : m_image(info.m_size.first, info.m_size.second, QImage::Format_RGB888)
    {}

    QImage &get_image() noexcept
    {
        return m_image;
    }

    bool is_all_ended() noexcept;

    void end() noexcept
    {
        ++thread_usage;
    }

private:
    QImage m_image;
    std::atomic<int> thread_usage{0};
};

class worker : public QObject
{
    Q_OBJECT

public:
    worker() noexcept;
    ~worker();

    std::mutex m_mt;

    QImage m_image;
    info_render m_info;
    std::shared_ptr<info_worker> processing_info{nullptr};
    std::atomic<int> processing_version{0};
    std::condition_variable cv;
    std::vector<std::thread> workers;

    void thread_worker(int) noexcept;
    void thread_worker_impl(int, info_render const&, auto&, const int, uchar*) noexcept;
    void request(info_render const&) noexcept;
    QImage get_image() noexcept;
signals:
    void image_is_done();
};

#endif // worker_H

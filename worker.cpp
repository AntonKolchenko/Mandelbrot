#include "worker.h"
#include <complex>
#include <cmath>
#include <QThread>

static const int thread_count = (QThread::idealThreadCount() == 1) ? 1 : QThread::idealThreadCount() - 1;
static const int MAX_STEP = 2000;

double pixel_color(double x, double y) {
    std::complex<double> c(x, y), z = 0;
    for (int i = 0; i < MAX_STEP; i++) {
        if ((z.real() * z.real() + z.imag()*z.imag()) >= 4.) {
            return (i % 51) / 50.;
        }
        z = z * z + c;
    }
    return 0;
}


bool info_worker::is_all_ended() noexcept {
    return thread_count == thread_usage;
}

worker::worker() noexcept
{
    for (int i = 0; i < thread_count; ++i) {
        workers.emplace_back([i, this]{
            thread_worker(i);
        });
    }
}

worker::~worker()
{
    processing_version = 0;
    cv.notify_all();
    for (auto& thread : workers) {
        thread.join();
    }
}

void worker::thread_worker(int id) noexcept
{
    std::shared_ptr<info_worker> temp_request;
    int temp_version = 0;
    uchar* start_ptr;
    info_render inf;
    int bytes_per_line;

    auto is_cancellation = [&, id, this] {
        return temp_version != processing_version;
    };

    while(true) {
        {
            std::unique_lock lock(m_mt);
            cv.wait(lock, is_cancellation);
            temp_version = processing_version;
            if (temp_version == 0) {
                return;
            }
            temp_request = processing_info;
            inf = m_info;

            start_ptr = temp_request->get_image().bits();
            bytes_per_line = temp_request->get_image().bytesPerLine();
        }

        thread_worker_impl(id, inf, is_cancellation, bytes_per_line, start_ptr);

        if (!is_cancellation()) {
            temp_request->end();

            if (temp_request->is_all_ended()) {
                {
                    std::lock_guard lock(m_mt);
                    m_image.swap(temp_request->get_image());
                }
                emit image_is_done();
                if (inf.m_block_size > 1) {
                    inf.m_block_size /= 2;
                    request(inf);
                }
            }
        }
    }
}


double get_color(double x) {
    double res = -4. * x * x + 1;
    return (res >= 0) ? res : 0;
}

void worker::thread_worker_impl(int id, info_render const& inf, auto& is_cancellation, const int bpl, uchar* start_ptr) noexcept
{
    const int block_size = inf.m_size.second / thread_count;
    for (int j = id * block_size; j < (id + 1) * block_size; ++j) {
        uchar* p = start_ptr + (id * bpl * block_size) + ((j - id * block_size) * bpl);
        if (is_cancellation()) return;
        for (int i = 0; i < inf.m_size.first; ++i) {
            if (is_cancellation()) return;
            if (i % inf.m_block_size == 0) {
                if ((j - id * block_size) % inf.m_block_size == 0) {
                    double color = pixel_color(inf.m_start.first + (i * inf.m_scale), inf.m_start.second + (j * inf.m_scale));
                    *p++ = static_cast<uchar>(color * get_color(color - 1) * 0xff);
                    *p++ = static_cast<uchar>(color * get_color(color - 0.5) * 0xff);
                    *p++ = static_cast<uchar>(color * get_color(color) * 0xff);
                } else {
                    *p++ = p[-bpl];
                    *p++ = p[-bpl];
                    *p++ = p[-bpl];
                }
            } else {
                *p++ = p[-3];
                *p++ = p[-3];
                *p++ = p[-3];
            }
        }
    }
}

void worker::request(const info_render & inf) noexcept
{
    {
        std::lock_guard lock(m_mt);
        processing_info = std::make_shared<info_worker>(inf);
        m_info = inf;
    }
    ++processing_version;
    cv.notify_all();
}

QImage worker::get_image() noexcept
{
    std::lock_guard lock(m_mt);
    return m_image;
}


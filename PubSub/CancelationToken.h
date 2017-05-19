#pragma once

#include <mutex>

class CancelationToken
{
public:
    CancelationToken();

    void Cancel();
    void Reset();
    bool IsCanceled();

private:
    bool m_cancel;
    std::mutex m_mutex;
};
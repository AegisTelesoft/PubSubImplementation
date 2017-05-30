#include "CancelationToken.h"

CancelationToken::CancelationToken() : m_cancel(false)
{

}

/**************************************************************************************************/
void CancelationToken::Cancel()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cancel = true;
    lock.unlock();
}

/**************************************************************************************************/
void CancelationToken::Reset()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cancel = false;
    lock.unlock();
}

/**************************************************************************************************/
bool CancelationToken::IsCanceled()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_cancel)
        return true;
    else
        return false;
}
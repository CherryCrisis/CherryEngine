#pragma once

#include <deque>
#include <set>
#include <typeindex>

#include "callback.hpp"
#include "function.hpp"


template<class... Args>
class Event
{
private:
    struct ARequest
    {
        enum class ERequest
        {
            ADD,
            REMOVE,
            RESET,
        };

        ERequest m_requestType = ERequest::ADD;
        std::shared_ptr<CCCallback::ACallback<Args...>> m_callback;
    };

    std::deque<ARequest> m_queries;
    std::set<std::shared_ptr<CCCallback::ACallback<Args...>>> m_callbacks;

    void AddToCallbacks(std::shared_ptr<CCCallback::ACallback<Args...>> callback)
    {
        m_callbacks.insert(callback);
    }

    void DeleteCallbacks(std::shared_ptr<CCCallback::ACallback<Args...>> callback)
    {
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
        {
            if ((*it)->IsEqual(callback.get()))
            {
                m_callbacks.erase(it);
                return;
            }
        }
    }

public:
    Event() = default;
    virtual ~Event() = default;

    void Bind(std::shared_ptr<CCCallback::ACallback<Args...>> callback)
    {
        m_queries.push_back(ARequest{ .m_requestType = ARequest::ERequest::ADD, .m_callback = callback });
    }

    template<typename T>
    void Bind(void (T::* func)(Args... type), T* member)
    {
        auto callback = CCCallback::BindCallback(func, member);
        Bind(callback);
    }

    void Bind(void (*func)(Args... type))
    {
        auto callback = CCCallback::BindCallback(func);
        Bind(callback);
    }

    template<typename T>
    void Unbind(void (T::* func)(Args... type), T* member)
    {
        for (auto& callback : m_callbacks)
        {
            if (auto memberCallback = std::dynamic_pointer_cast<CCCallback::MemberCallback<T, Args...>>(callback))
            {
                if (memberCallback->m_func == func && memberCallback->m_member == member)
                {
                    m_queries.push_back(ARequest{ .m_requestType = ARequest::ERequest::REMOVE, .m_callback = callback });

                    return;
                }
            }
        }

        for (auto it = m_queries.begin(); it != m_queries.end(); ++it)
        {
            if (auto memberCallback = std::dynamic_pointer_cast<CCCallback::MemberCallback<T, Args...>>(it->m_callback))
            {
                if (memberCallback->m_func == func && memberCallback->m_member == member)
                {
                    m_queries.erase(it);
                    return;
                }
            }
        }
    }

    void Unbind(void (*func)(Args... type))
    {
        for (auto& callback : m_callbacks)
        {
            if (auto nonMemberCallback = std::dynamic_pointer_cast<CCCallback::NonMemberCallback<Args...>>(callback))
            {
                if (nonMemberCallback->m_func == func)
                {
                    m_queries.push_back(ARequest{ .m_requestType = ARequest::ERequest::REMOVE, .m_callback = callback });
                    return;
                }
            }
        }

        for (auto it = m_queries.begin(); it != m_queries.end(); ++it)
        {
            if (auto nonMemberCallback = std::dynamic_pointer_cast<CCCallback::NonMemberCallback<Args...>>(it->m_callback))
            {
                if (nonMemberCallback->m_func == func)
                {
                    m_queries.erase(it);
                    return;
                }
            }
        }
    }

    void Invoke(Args&&... args)
    {
        while (!m_queries.empty())
        {
            auto& request = m_queries.front();

            switch (request.m_requestType)
            {
            case ARequest::ERequest::REMOVE:
                DeleteCallbacks(request.m_callback);
                break;
            case ARequest::ERequest::RESET:
                m_callbacks.clear();
                break;
            default:
                AddToCallbacks(request.m_callback);
                break;
            }

            m_queries.pop_front();
        }

        for (auto callbackIt = m_callbacks.begin(); callbackIt != m_callbacks.end(); callbackIt++)
        {
            std::shared_ptr< CCCallback::ACallback<Args...>> callback = *callbackIt;
            callback->Invoke(std::forward<Args>(args)...);
        }
    }

    void Reset()
    {
        m_queries.push_back(ARequest{ .m_requestType = ARequest::ERequest::RESET, .m_callback = nullptr });
    }
};
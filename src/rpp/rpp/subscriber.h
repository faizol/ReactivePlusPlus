// MIT License
// 
// Copyright (c) 2021 Aleksey Loginov
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "subscription.h"

#include <rpp/observer.h>

#include <variant>

namespace rpp
{
template<typename Type>
class subscriber final
{
    static_assert(std::is_same_v<std::decay_t<Type>, Type>,
        "Type should be decayed to match with decayed observable types");

    template<typename TType>
    using enable_if_same_type_t = std::enable_if_t<std::is_same_v<std::decay_t<TType>, Type>>;
public:
    template<typename TType, typename = enable_if_same_type_t<TType>>
    subscriber(const observer<TType>& observer)
        : m_observer{observer} { }

    template<typename TType, typename = enable_if_same_type_t<TType>>
    subscriber(observer<TType>&& observer)
        : m_observer{std::move(observer)} { }

    template<typename U, typename = enable_if_same_type_t<U>>
    void on_next(U&& val) const
    {
        if (!m_subscription.is_subscribed())
            return;

        std::visit([&](auto& obs) { obs.on_next(std::forward<U>(val)); }, m_observer);
    }

    void on_error(const std::exception_ptr& err) const
    {
        if (!m_subscription.is_subscribed())
            return;

        subscription_guard   guard{m_subscription};
        std::visit([&](auto& obs) { obs.on_error(err); }, m_observer);
    }

    void on_completed() const
    {
        if (!m_subscription.is_subscribed())
            return;

        subscription_guard   guard{m_subscription};
        std::visit([&](auto& obs) { obs.on_completed(); }, m_observer);
    }

    [[nodiscard]] bool is_subscribed() const { return m_subscription.is_subscribed(); }
    void               unsubscribe() const { m_subscription.unsubscribe(); }
private:
    std::variant<observer<Type>,
                 observer<Type&>,
                 observer<const Type&>,
                 observer<Type&&>> m_observer;

    subscription m_subscription;
};

template<typename T>
subscriber(observer<T> observer) -> subscriber<std::decay_t<T>>;
} // namespace rpp

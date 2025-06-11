//                  ReactivePlusPlus library
//
//          Copyright Aleksey Loginov 2023 - present.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/victimsnino/ReactivePlusPlus
//

#pragma once

#include <rpp/observables/fwd.hpp>
#include <rpp/operators/fwd.hpp>

#include <rpp/operators/details/repeating_strategy.hpp>

namespace rpp::operators::details
{
    template<rpp::constraint::observer TObserver,
             typename TObservable,
             typename TNotifier>
    struct repeat_when_impl_strategy final : public repeating_observer_strategy<TObserver, TObservable, TNotifier>
    {
        using self_type = repeat_when_impl_strategy<TObserver, TObservable, TNotifier>;

        using repeating_observer_strategy<TObserver, TObservable, TNotifier>::state;

        template<typename T>
        void on_next(T&& v) const
        {
            state->observer.on_next(std::forward<T>(v));
        }

        void on_error(const std::exception_ptr& err) const
        {
            state->observer.on_error(err);
        }

        void on_completed() const
        {
            try
            {
                state->notifier().subscribe(repeating_inner_observer_strategy<self_type, TObserver, TObservable, TNotifier>{this->state});
            }
            catch (...)
            {
                state->observer.on_error(std::current_exception());
            }
        }
    };

    template<rpp::constraint::decayed_type TNotifier>
    struct repeat_when_t
    {
        RPP_NO_UNIQUE_ADDRESS TNotifier notifier;

        template<rpp::constraint::decayed_type T>
        struct operator_traits
        {
            using result_type = T;
        };

        template<rpp::details::observables::constraint::disposables_strategy Prev>
        using updated_optimal_disposables_strategy = rpp::details::observables::fixed_disposables_strategy<1>;

        template<rpp::constraint::observer TObserver, typename TObservable>
        void subscribe(TObserver&& observer, TObservable&& observable) const
        {
            const auto d   = disposable_wrapper_impl<repeating_state<std::decay_t<TObserver>, std::decay_t<TObservable>, std::decay_t<TNotifier>>>::make(std::forward<TObserver>(observer), std::forward<TObservable>(observable), notifier);
            auto       ptr = d.lock();

            ptr->observer.set_upstream(d.as_weak());

            drain<repeat_when_impl_strategy<std::decay_t<TObserver>, std::decay_t<TObservable>, std::decay_t<TNotifier>>>(ptr);
        }
    };
} // namespace rpp::operators::details

namespace rpp::operators
{
    /**
     * @brief If observable completes, invoke @p notifier and when returned observable emits a value
     * resubscribe to the source observable. If the notifier throws or returns an error/empty
     * observable, then error/completed emission is forwarded to original subscription.
     *
     * @param notifier callable taking no arguments and returning observable notifying when to resubscribe
     *
     * @note `#include <rpp/operators/repeat_when.hpp>`
     *
     * @ingroup creational_operators
     * @see https://reactivex.io/documentation/operators/repeat.html
     */
    template<typename TNotifier>
        requires rpp::constraint::observable<std::invoke_result_t<TNotifier>>
    auto repeat_when(TNotifier&& notifier)
    {
        return details::repeat_when_t<std::decay_t<TNotifier>>{std::forward<TNotifier>(notifier)};
    }
} // namespace rpp::operators

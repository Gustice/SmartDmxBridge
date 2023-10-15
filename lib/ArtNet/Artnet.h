#pragma once
#ifndef ARDUINO_ARTNET_H
#define ARDUINO_ARTNET_H

// Spec (Art-Net 4) : http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/


namespace arx { // others

    template <class AlwaysVoid, template<class...> class Check, class... T>
    struct is_detected_impl : std::false_type {};
    template <template <class...> class Check, class... T>
    struct is_detected_impl <std::void_t<Check<T...>>, Check, T...> : std::true_type {};
    template <template <class...> class Check, class... T>
    using is_detected = is_detected_impl<void, Check, T...>;


    template <typename T>
    struct is_callable {
        template <typename U, decltype(&U::operator()) = &U::operator()>
        struct checker {};
        template <typename U> static std::true_type  test(checker<U> *);
        template <typename>   static std::false_type test(...);
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };
    template <typename R, typename ... Arguments>
    struct is_callable<R(*)(Arguments ...)> {
        static constexpr bool value = true;
    };
    template <typename R, typename ... Arguments>
    struct is_callable<R(*&)(Arguments ...)> {
        static constexpr bool value = true;
    };
    template <typename R, typename ... Arguments>
    struct is_callable<R(&)(Arguments ...)> {
        static constexpr bool value = true;
    };
    template <typename R, typename ... Arguments>
    struct is_callable<R(Arguments ...)> {
        static constexpr bool value = true;
    };
    template <typename R, typename ... Arguments>
    struct is_callable<std::function<R(Arguments ...)>> {
        static constexpr bool value = true;
    };


    namespace detail {
        template <typename ret, typename ... arguments>
        struct function_traits {
            static constexpr size_t arity = sizeof...(arguments);
            using result_type = ret;
            using arguments_types_tuple = std::tuple<arguments ...>;
            // template <size_t index>
            // using argument_type = type_at<index, arguments ...>;
            using function_type = std::function<ret(arguments ...)>;
            template <typename function_t>
            static constexpr function_type cast(function_t f) {
                return static_cast<function_type>(f);
            }
        };
    }
    template <typename T>
    struct function_traits : public function_traits<decltype(&T::operator())> {};
    template <typename class_type, typename ret, typename ... arguments>
    struct function_traits<ret(class_type::*)(arguments ...) const>
    : detail::function_traits<ret, arguments ...> {};

    template <typename class_type, typename ret, typename ... arguments>
    struct function_traits<ret(class_type::*)(arguments ...)>
    : detail::function_traits<ret, arguments ...> {};

    template <typename ret, typename ... arguments>
    struct function_traits<ret(*)(arguments ...)>
    : detail::function_traits<ret, arguments ...> {};

    template <typename ret, typename ... arguments>
    struct function_traits<ret(*&)(arguments ...)>
    : detail::function_traits<ret, arguments ...> {};

    template <typename ret, typename ... arguments>
    struct function_traits<ret(arguments ...)>
    : detail::function_traits<ret, arguments ...> {};

    template <typename ret, typename ... arguments>
    struct function_traits<std::function<ret(arguments ...)>>
    : detail::function_traits<ret, arguments ...> {};

} // namespace arx

#include "ArtnetCommon.h"
// using Artnet = arx::artnet::Manager;
// using ArtnetSender = arx::artnet::Sender;
using ArtnetReceiver = arx::artnet::Receiver;

#endif  // ARDUINO_ARTNET_H

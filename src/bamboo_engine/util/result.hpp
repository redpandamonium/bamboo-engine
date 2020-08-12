//
// Created by Leon Suchy on 06.08.20.
//

#ifndef BAMBOOENGINE_RESULT_HPP
#define BAMBOOENGINE_RESULT_HPP

#include <variant>
#include <utility>
#include <experimental/source_location>
#include "logging.hpp"

namespace bbge {

    /**
     * @brief Result class sort of inspired by Rust
     * @tparam Result If successful this type is held
     * @tparam Error If unsuccessful this type is held. Expected to be implement a std::exception like what() function.
     */
    template <typename Result, typename Error>
    class result {
    public:

        using ok_type = Result;
        using err_type = Error;

        static_assert(!std::is_same_v<ok_type, err_type>); // implementation limitation

        /**
         * @brief Construct a successful result
         * @param o Ok result
         */
        result(ok_type&& o);

        /**
         * @brief Construct an error
         * @param e Error result
         */
        result(err_type&& e);

        /**
         * @brief Construct a successful result
         * @param o Ok result
         */
        result(const ok_type& o);

        /**
         * @brief Construct an error
         * @param e Error result
         */
        result(const err_type& e);

        /**
         * @brief Construct a successful result in place
         * @tparam Args Argument types for ok_type ctor
         * @param args Arguments
         */
        template <typename... Args>
        explicit result(std::in_place_type_t<ok_type>, Args&&... args);

        /**
         * @brief Construct an error in place
         * @tparam Args Argument types for err_type ctor
         * @param args Arguments
         */
        template <typename... Args>
        explicit result(std::in_place_type_t<err_type>, Args&&... args);

        /**
         * @brief Check if the result is an error
         * @return True if error
         */
        [[nodiscard]] bool is_err() const noexcept;

        /**
         * @brief Check if the result is a result
         * @return True if successful
         */
        [[nodiscard]] bool is_ok() const noexcept;

        /**
         * @brief Get the result or an alternative value
         * @param otherwise Alternative value to use if this is err
         * @return The result or otherwise when this is err
         */
        const ok_type& or_else(const ok_type& otherwise) const;

        /**
         * @brief Get the result or an alternative value
         * @param otherwise Alternative value to use if this is err
         * @return The result or otherwise when this is err
         */
        ok_type& or_else(ok_type& otherwise);

        /**
         * @brief Get the result or throw an error on err
         * @return Result if this is ok
         */
        const ok_type& or_throw() const;

        /**
         * @brief Get the result or throw an error on err
         * @return Result if this is ok
         */
        ok_type& or_throw();

        /**
         * @brief Get the optional value.
         * @return Value or nullptr if this is err
         */
        ok_type* ok();

        /**
         * @brief Get the optional value.
         * @return Value or nullptr if this is err
         */
        const ok_type* ok() const;

        /**
         * @brief Get the optional error.
         * @return Error or nullptr if this is ok
         */
        err_type* err();

        /**
         * @brief Get the optional error.
         * @return Error or nullptr if this is ok
         */
        const err_type* err() const;

        /**
         * @brief If this is err then log it.
         */
        const result<ok_type, err_type>& log_err(
            spdlog::level::level_enum l = spdlog::level::err,
            const std::experimental::source_location& loc = std::experimental::source_location::current()
        ) const;

        /**
         * @brief If this is err then log it.
         */
        result<ok_type, err_type>& log_err(
            spdlog::level::level_enum l = spdlog::level::err,
            const std::experimental::source_location& loc = std::experimental::source_location::current()
        );

        [[nodiscard]] bool operator!() const;

    private:

        // Consider doing something more performant. Some compilers really struggle with variant.
        std::variant<ok_type, err_type> m_variant;
    };

    template <typename Result, typename Error>
    result<Result, Error>::result(ok_type&& o) : m_variant(o) {

    }

    template <typename Result, typename Error>
    typename result<Result, Error>::err_type* result<Result, Error>::err() {
        if (is_err()) return &std::get<err_type>(m_variant);
        return nullptr;
    }

    template <typename Result, typename Error>
    const typename result<Result, Error>::err_type* result<Result, Error>::err() const {
        if (is_err()) return &std::get<err_type>(m_variant);
        return nullptr;
    }

    template <typename Result, typename Error>
    typename result<Result, Error>::ok_type* result<Result, Error>::ok() {
        if (is_ok()) return &std::get<ok_type>(m_variant);
        return nullptr;
    }

    template <typename Result, typename Error>
    const typename result<Result, Error>::ok_type* result<Result, Error>::ok() const {
        if (is_ok()) return &std::get<ok_type>(m_variant);
        return nullptr;
    }

    template <typename Result, typename Error>
    bool result<Result, Error>::is_err() const noexcept {
        return std::holds_alternative<err_type>(m_variant);
    }

    template <typename Result, typename Error>
    bool result<Result, Error>::is_ok() const noexcept {
        return std::holds_alternative<ok_type>(m_variant);
    }

    template <typename Result, typename Error>
    const typename result<Result, Error>::ok_type& result<Result, Error>::or_throw() const {
        if (is_err()) throw *err();
        return *ok();
    }

    template <typename Result, typename Error>
    typename result<Result, Error>::ok_type& result<Result, Error>::or_throw() {
        if (is_err()) throw *err();
        return *ok();
    }

    template <typename Result, typename Error>
    const typename result<Result, Error>::ok_type& result<Result, Error>::or_else(const ok_type& otherwise) const {
        if (is_err()) return otherwise;
        return *ok();
    }

    template <typename Result, typename Error>
    typename result<Result, Error>::ok_type& result<Result, Error>::or_else(ok_type& otherwise) {
        if (is_err()) return otherwise;
        return *ok();
    }

    template <typename Result, typename Error>
    template <typename... Args>
    result<Result, Error>::result(std::in_place_type_t<ok_type> t, Args&& ... args)
      : m_variant(t, std::forward<Args>(args)...) {

    }

    template <typename Result, typename Error>
    result<Result, Error>::result(const ok_type& o) : m_variant(o) {

    }

    template <typename Result, typename Error>
    const result<typename result<Result, Error>::ok_type, typename result<Result, Error>::err_type>&
    result<Result, Error>::log_err(spdlog::level::level_enum l, const std::experimental::source_location& loc) const {
        if (is_err()) {
            auto src_loc = spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());
            spdlog::log(src_loc, l, err()->what());
        }
        return *this;
    }

    template <typename Result, typename Error>
    result<typename result<Result, Error>::ok_type, typename result<Result, Error>::err_type>&
    result<Result, Error>::log_err(spdlog::level::level_enum l, const std::experimental::source_location& loc) {
        if (is_err()) {
            auto src_loc = spdlog::source_loc(loc.file_name(), loc.line(), loc.function_name());
            spdlog::log(src_loc, l, err()->what());
        }
        return *this;
    }

    template <typename Result, typename Error>
    bool result<Result, Error>::operator!() const {
        return is_err();
    }

    template <typename Result, typename Error>
    result<Result, Error>::result(const err_type& e) : m_variant(e) {

    }

    template <typename Result, typename Error>
    template <typename... Args>
    result<Result, Error>::result(std::in_place_type_t<err_type> t, Args&& ... args)
        : m_variant(t, std::forward<Args>(args)...) {

    }

    template <typename Result, typename Error>
    result<Result, Error>::result(err_type&& e) : m_variant(e) {

    }
}

#endif //BAMBOOENGINE_RESULT_HPP

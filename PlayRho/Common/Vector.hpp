/*
 * Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
 * Modified work Copyright (c) 2017 Louis Langholtz https://github.com/louis-langholtz/PlayRho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PLAYRHO_COMMON_VECTOR_HPP
#define PLAYRHO_COMMON_VECTOR_HPP

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <iterator>
#include <algorithm>
#include <functional>
#include <iostream>
#include <PlayRho/Common/InvalidArgument.hpp>
#include <PlayRho/Common/Real.hpp>
#include <PlayRho/Common/Templates.hpp>

namespace playrho {

/// @brief Vector.
/// @details This is a <code>PLAYRHO_CONSTEXPR inline</code> and constructor enhanced
///   <code>std::array</code>-like template class for types supporting the +, -, *, /
///   arithmetic operators ("arithmetic types" as defined by the <code>IsArithmetic</code>
///   type trait) that itself comes with non-member arithmetic operator support making
///   Vector instances arithmetic types as well.
/// @note This type is trivially default constructible - i.e. default construction
///   performs no actions (no initialization).
/// @sa IsArithmetic
template <typename T, std::size_t N>
struct Vector
{
    static_assert(N > 0, "Number of elements must be greater than 0");

    /// @brief Value type.
    using value_type = T;

    /// @brief Size type.
    using size_type = std::size_t;
    
    /// @brief Difference type.
    using difference_type = std::ptrdiff_t;
    
    /// @brief Reference type.
    using reference = value_type&;
    
    /// @brief Constant reference type.
    using const_reference = const value_type&;
    
    /// @brief Pointer type.
    using pointer = value_type*;
    
    /// @brief Constant pointer type.
    using const_pointer = const value_type*;
    
    /// @brief Iterator type.
    using iterator = value_type*;
    
    /// @brief Constant iterator type.
    using const_iterator = const value_type*;
    
    /// @brief Reverse iterator type.
    using reverse_iterator = std::reverse_iterator<iterator>;
    
    /// @brief Constant reverse iterator type.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    /// @brief Default constructor.
    /// @note Defaulted explicitly.
    /// @note This constructor performs no action.
    PLAYRHO_CONSTEXPR inline Vector() = default;
    
    /// @brief Initializing constructor.
    template<typename... Tail>
    PLAYRHO_CONSTEXPR inline explicit Vector(typename std::enable_if<sizeof...(Tail)+1 == N, T>::type
                     head, Tail... tail) noexcept: elements{head, T(tail)...}
    {
        // Intentionally empty.
    }

    /// @brief Gets the max size.
    PLAYRHO_CONSTEXPR inline size_type max_size() const noexcept { return N; }
    
    /// @brief Gets the size.
    PLAYRHO_CONSTEXPR inline size_type size() const noexcept { return N; }
    
    /// @brief Whether empty.
    /// @note Always false for N > 0.
    PLAYRHO_CONSTEXPR inline bool empty() const noexcept { return N == 0; }
    
    /// @brief Gets a "begin" iterator.
    iterator begin() noexcept { return iterator(elements); }

    /// @brief Gets an "end" iterator.
    iterator end() noexcept { return iterator(elements + N); }
    
    /// @brief Gets a "begin" iterator.
    const_iterator begin() const noexcept { return const_iterator(elements); }
    
    /// @brief Gets an "end" iterator.
    const_iterator end() const noexcept { return const_iterator(elements + N); }
    
    /// @brief Gets a "begin" iterator.
    const_iterator cbegin() const noexcept { return begin(); }
    
    /// @brief Gets an "end" iterator.
    const_iterator cend() const noexcept { return end(); }

    /// @brief Gets a reverse "begin" iterator.
    reverse_iterator rbegin() noexcept { return reverse_iterator{elements + N}; }

    /// @brief Gets a reverse "end" iterator.
    reverse_iterator rend() noexcept { return reverse_iterator{elements}; }
    
    /// @brief Gets a reverse "begin" iterator.
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{elements + N};
    }
    
    /// @brief Gets a reverse "end" iterator.
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{elements};
    }

    /// @brief Gets a reverse "begin" iterator.
    const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }
    
    /// @brief Gets a reverse "end" iterator.
    const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    /// @brief Gets a reference to the requested element.
    /// @note No bounds checking is performed.
    /// @warning Behavior is undefined if given a position equal to or greater than size().
    PLAYRHO_CONSTEXPR inline reference operator[](size_type pos) noexcept
    {
        assert(pos < size());
        return elements[pos];
    }
    
    /// @brief Gets a constant reference to the requested element.
    /// @note No bounds checking is performed.
    /// @warning Behavior is undefined if given a position equal to or greater than size().
    PLAYRHO_CONSTEXPR inline const_reference operator[](size_type pos) const noexcept
    {
        assert(pos < size());
        return elements[pos];
    }
    
    /// @brief Gets a reference to the requested element.
    /// @throws InvalidArgument if given a position that's >= size().
    PLAYRHO_CONSTEXPR inline reference at(size_type pos)
    {
        if (pos >= size())
        {
            throw InvalidArgument("Vector::at: position >= size()");
        }
        return elements[pos];
    }
    
    /// @brief Gets a constant reference to the requested element.
    /// @throws InvalidArgument if given a position that's >= size().
    PLAYRHO_CONSTEXPR inline const_reference at(size_type pos) const
    {
        if (pos >= size())
        {
            throw InvalidArgument("Vector::at: position >= size()");
        }
        return elements[pos];
    }
    
    /// @brief Direct access to data.
    PLAYRHO_CONSTEXPR inline pointer data() noexcept
    {
        return elements;
    }
    
    /// @brief Direct access to data.
    PLAYRHO_CONSTEXPR inline const_pointer data() const noexcept
    {
        return elements;
    }
    
    /// @brief Elements.
    /// @warning Don't access this directly!
    /// @warning Data is not initialized on default construction. This is intentional
    ///   to avoid any performance overhead that default initialization might incur.
    value_type elements[N];
};

/// @brief Trait class for checking if type is a Vector type.
template <typename>
struct IsVector: std::false_type {};

/// @brief Trait class specialization for checking if type is a Vector type.
template <typename T, std::size_t N>
struct IsVector<Vector<T, N>>: std::true_type {};

/// @brief Equality operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline bool operator== (const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept
{
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }
    return true;
}

/// @brief Inequality operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline bool operator!= (const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept
{
    return !(lhs == rhs);
}

/// @brief Unary plus operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T, decltype(+T{})>::value, Vector<T, N>>::type
operator+ (Vector<T, N> v) noexcept
{
    return v;
}

/// @brief Unary negation operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T, decltype(-T{})>::value, Vector<T, N>>::type
operator- (Vector<T, N> v) noexcept
{
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        v[i] = -v[i];
    }
    return v;
}

/// @brief Increments the left hand side value by the right hand side value.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T, decltype(T{} + T{})>::value, Vector<T, N>&>::type
operator+= (Vector<T, N>& lhs, const Vector<T, N> rhs) noexcept
{
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

/// @brief Decrements the left hand side value by the right hand side value.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T, decltype(T{} - T{})>::value, Vector<T, N>&>::type
operator-= (Vector<T, N>& lhs, const Vector<T, N> rhs) noexcept
{
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

/// @brief Adds two vectors component-wise.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T, decltype(T{} + T{})>::value, Vector<T, N>>::type
operator+ (Vector<T, N> lhs, const Vector<T, N> rhs) noexcept
{
    return lhs += rhs;
}

/// @brief Subtracts two vectors component-wise.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T, decltype(T{} - T{})>::value, Vector<T, N>>::type
operator- (Vector<T, N> lhs, const Vector<T, N> rhs) noexcept
{
    return lhs -= rhs;
}

/// @brief Multiplication assignment operator.
/// @relatedalso Vector
template <typename T1, typename T2, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T1, decltype(T1{} * T2{})>::value, Vector<T1, N>&>::type
operator*= (Vector<T1, N>& lhs, const T2 rhs) noexcept
{
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        lhs[i] *= rhs;
    }
    return lhs;
}

/// @brief Division assignment operator.
/// @relatedalso Vector
template <typename T1, typename T2, std::size_t N>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<std::is_same<T1, decltype(T1{} / T2{})>::value, Vector<T1, N>&>::type
operator/= (Vector<T1, N>& lhs, const T2 rhs) noexcept
{
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        lhs[i] /= rhs;
    }
    return lhs;
}

/// @brief Multiplies an A-by-B vector of vectors by a B-by-C vector of vectors.
/// @note From Wikipedia:
///   > Multiplication of two matrices is defined if and only if the number of columns
///   > of the left matrix is the same as the number of rows of the right matrix.
/// @note Matrix multiplication is not commutative.
/// @note Algorithmically speaking, this implementation is called the "naive" algorithm.
///   For small matrices, like 3-by-3 or smaller matrices, its complexity shouldn't be an issue.
///   The matrix dimensions are compile time constants anyway which can help compilers
///   automatically identify loop unrolling and hardware level parallelism opportunities.
/// @param lhs Left-hand-side matrix.
/// @param rhs Right-hand-side matrix.
/// @return A-by-C matrix product of the left-hand-side matrix and the right-hand-side matrix.
/// @sa https://en.wikipedia.org/wiki/Matrix_multiplication
/// @sa https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm
/// @sa https://en.wikipedia.org/wiki/Commutative_property
/// @relatedalso Vector
template <typename T1, typename T2, std::size_t A, std::size_t B, std::size_t C, typename OT = decltype(T1{} * T2{})>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<IsMultipliable<T1, T2>::value, Vector<Vector<OT, C>, A>>::type
operator* (const Vector<Vector<T1, B>, A>& lhs, const Vector<Vector<T2, C>, B>& rhs) noexcept
{
    //using OT = decltype(T1{} * T2{});
    auto result = Vector<Vector<OT, C>, A>{};
    for (auto a = static_cast<std::size_t>(0); a < A; ++a)
    {
        for (auto c = static_cast<std::size_t>(0); c < C; ++c)
        {
            // So for 2x3 lhs matrix * 3*2 rhs matrix... result is 2x2 matrix:
            // result[0][0] = lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0] + lhs[0][2] * rhs[2][0]
            // result[0][1] = lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1] + lhs[0][2] * rhs[2][1]
            // result[1][0] = lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0] + lhs[1][2] * rhs[2][0]
            // result[1][1] = lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1] + lhs[1][2] * rhs[2][1]
            auto element = OT{};
            for (auto b = static_cast<std::size_t>(0); b < B; ++b)
            {
                element += lhs[a][b] * rhs[b][c];
            }
            result[a][c] = element;
        }
    }
    return result;
}

// Have to make sure a lhs Vector<T1, N> is not multipliable by a rhs vector unless
// rhs is Vector<Vector<T2, M>, N> where IsMultipliable<T1, T2>::value.

/// @brief Multiplication operator for non vector times vector.
/// @relatedalso Vector
/// @note Explicitly disabled for Vector * Vector to prevent this function from existing
///   in that case and prevent errors like "use of overloaded operator '*' is ambiguous".
template <std::size_t N, typename T1, typename T2, typename OT = decltype(T1{} * T2{})>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<IsMultipliable<T1, T2>::value && !IsVector<T1>::value, Vector<OT, N>>::type
operator* (const T1 s, Vector<T2, N> a) noexcept
{
    // Can't base this off of *= since result type in this case can be different
    auto result = Vector<OT, N>{};
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        result[i] = s * a[i];
    }
    return result;
}

/// @brief Multiplication operator for vector times non vector.
/// @relatedalso Vector
/// @note Explicitly disabled for Vector * Vector to prevent this function from existing
///   in that case and prevent errors like "use of overloaded operator '*' is ambiguous".
template <std::size_t N, typename T1, typename T2, typename OT = decltype(T1{} * T2{})>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<IsMultipliable<T1, T2>::value && !IsVector<T2>::value, Vector<OT, N>>::type
operator* (Vector<T1, N> a, const T2 s) noexcept
{
    // Can't base this off of *= since result type in this case can be different
    auto result = Vector<OT, N>{};
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        result[i] = a[i] * s;
    }
    return result;
}

/// @brief Division operator.
/// @relatedalso Vector
template <std::size_t N, typename T1, typename T2, typename OT = decltype(T1{} / T2{})>
PLAYRHO_CONSTEXPR inline
typename std::enable_if<IsDivisable<T1, T2>::value, Vector<OT, N>>::type
operator/ (Vector<T1, N> a, const T2 s) noexcept
{
    // Can't base this off of /= since result type in this case can be different
    auto result = Vector<OT, N>{};
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        result[i] = a[i] / s;
    }
    return result;
}

#if 0
/// @brief Multiplication operator.
/// @relatedalso Vector
template <std::size_t N, typename T1, typename T2, typename OT = decltype(T1{} * T2{})>
PLAYRHO_CONSTEXPR inline auto operator* (const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept
{
    auto result = Vector<OT, N>{};
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        result[i] = lhs[i] * rhs[i];
    }
    return result;
}

/// @brief Division operator.
/// @relatedalso Vector
template <std::size_t N, typename T1, typename T2, typename OT = decltype(T1{} / T2{})>
PLAYRHO_CONSTEXPR inline auto operator/ (const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept
{
    auto result = Vector<OT, N>{};
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        result[i] = lhs[i] / rhs[i];
    }
    return result;
}
#endif

/// @brief Lexicographical less-than operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline bool operator< (const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept
{
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
                                        std::less<T>{});
}

/// @brief Lexicographical less-than or equal-to operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline bool operator<= (const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept
{
    const auto lhsEnd = std::cend(lhs);
    const auto rhsEnd = std::cend(rhs);
    const auto diff = std::mismatch(std::cbegin(lhs), lhsEnd, std::cbegin(rhs), rhsEnd);
    return (std::get<0>(diff) == lhsEnd) || (*std::get<0>(diff) < *std::get<1>(diff));
}

/// @brief Lexicographical greater-than operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline bool operator> (const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept
{
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
                                        std::greater<T>{});
}

/// @brief Lexicographical greater-than or equal-to operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
PLAYRHO_CONSTEXPR inline bool operator>= (const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept
{
    const auto lhsEnd = std::cend(lhs);
    const auto rhsEnd = std::cend(rhs);
    const auto diff = std::mismatch(std::cbegin(lhs), lhsEnd, std::cbegin(rhs), rhsEnd);
    return (std::get<0>(diff) == lhsEnd) || (*std::get<0>(diff) > *std::get<1>(diff));
}

/// @brief Gets the I'th element of the given collection.
/// @relatedalso Vector
template <std::size_t I, std::size_t N, typename T>
PLAYRHO_CONSTEXPR inline auto& Get(Vector<T, N>& v) noexcept
{
    static_assert(I < N, "Index out of bounds in playrho::Get<> (playrho::Vector)");
    return v[I];
}

/// @brief Gets the I'th element of the given collection.
template <std::size_t I, std::size_t N, typename T>
PLAYRHO_CONSTEXPR inline auto Get(const Vector<T, N>& v) noexcept
{
    static_assert(I < N, "Index out of bounds in playrho::Get<> (playrho::Vector)");
    return v[I];
}

/// @brief Output stream operator.
/// @relatedalso Vector
template <typename T, std::size_t N>
::std::ostream& operator<< (::std::ostream& os, const Vector<T, N>& value)
{
    os << "{";
    for (auto i = static_cast<std::size_t>(0); i < N; ++i)
    {
        if (i > static_cast<std::size_t>(0))
        {
            os << ',';
        }
        os << value[i];
    }
    os << "}";
    return os;
}

} // namespace playrho

namespace std {
    
    /// @brief Tuple size info for playrho::Vector
    template<class T, std::size_t N>
    class tuple_size< playrho::Vector<T, N> >: public std::integral_constant<std::size_t, N> {};
    
    /// @brief Tuple element type info for playrho::Vector
    template<std::size_t I, class T, std::size_t N>
    class tuple_element<I, playrho::Vector<T, N>>
    {
    public:
        /// @brief Type alias revealing the actual element type of the given Vector.
        using type = T;
    };
    
} // namespace std

#endif // PLAYRHO_COMMON_VECTOR_HPP

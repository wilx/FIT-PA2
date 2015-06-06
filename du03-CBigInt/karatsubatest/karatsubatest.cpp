// karatsubatest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <cassert>


typedef unsigned unit_type;
typedef unsigned long long uint64_type;
typedef uint64_type accum_type;
typedef std::vector<unit_type> unit_vec_type;
typedef unit_vec_type::iterator iterator;
typedef unit_vec_type::const_iterator const_iterator;


static unit_type const modulus = 10;
static unsigned const modulus_digits = 1;
static unit_vec_type const zero_value (1, 0);


template <typename T>
void
xswap (T & a, T & b)
{
    T tmp (a);
    a = b;
    b = tmp;
}


class print_range;

std::ostream & operator << (std::ostream &, print_range const &);


class print_range
{
public:
    print_range (const_iterator b, const_iterator e)
        : begin (b)
        , end (e)
    { }


    friend 
    std::ostream &
    operator << (std::ostream & os, print_range const & pr)
    {
        unsigned const v_size = pr.end - pr.begin;
        if (v_size == 0)
            return os;

        const_iterator it = pr.end;

        char const old_fill = os.fill ('0');
        std::streamsize const old_width = os.width ();

        os << "" << it[- 1];

        for (--it; it != pr.begin; --it)
        {
            os.width (modulus_digits);
            os << it[-1];
        }

        os.fill (old_fill);
        os.width (old_width);

        return os;        
    }

private:
    const_iterator begin;
    const_iterator end;
};


class print_vec;

std::ostream & operator << (std::ostream &, print_vec const &);


class print_vec
{
public:
    print_vec (unit_vec_type const & vec)
        : v (vec)
    { }
    
    friend 
    std::ostream &
    operator << (std::ostream & os, print_vec const & pv)
    {
        unit_vec_type const & v = pv.v;
        unsigned const v_size = v.size ();

        assert (v.size () >= 1);

        os << "" << v[v_size - 1];

        char const old_fill = os.fill ('0');
        std::streamsize const old_width = os.width ();

        for (unsigned i = v_size - 1; i != 0; --i)
        {
            os.width (modulus_digits);
            os << v[i - 1];
        }

        os.fill (old_fill);
        os.width (old_width);

        return os;        
    }

private:
    unit_vec_type const & v;
};


void
modulus_power (unit_vec_type & v, unsigned power)
{
    unsigned v_size = v.size ();
    if (v_size == 1 && v[0] == 0)
        return;

    unsigned v_new_size = v_size + power;

    v.resize (v_new_size);
    for (unsigned i = v_size; i != 0; --i)
        v[i + power - 1] = v[i - 1];

    for (unsigned i = 0; i != power; ++i)
        v[i] = 0;
}


void
remove_leading_zeroes (unit_vec_type & v)
{
    unsigned i;
    for (i = v.size (); i != 0; --i)
    {
        if (v[i - 1] != 0)
            break;
    }

    if (i == 0)
        v.resize (1);
    else
        v.resize (i);
}


void
resize_for_add (unit_vec_type & dest, const_iterator a_begin, const_iterator a_end,
    const_iterator b_begin, const_iterator b_end)
{
    unsigned a_size = a_end - a_begin;
    unsigned b_size = b_end - b_begin;

    dest.resize (a_size > b_size ? a_size + 1 : b_size + 1);
}


void
resize_for_add (unit_vec_type & dest, unit_vec_type const & a,
    unit_vec_type const & b)
{
    resize_for_add (dest, a.begin (), a.end (), b.begin (), b.end ());
}


void
resize_for_mul (unit_vec_type & dest, const_iterator a_begin, const_iterator a_end,
    const_iterator b_begin, const_iterator b_end)
{
    unsigned a_size = a_end - a_begin;
    unsigned b_size = b_end - b_begin;

    dest.resize (a_size > b_size ? a_size * 2 + 1 : b_size * 2 + 1);
}


void
resize_for_mul (unit_vec_type & dest, unit_vec_type const & a,
    unit_vec_type const & b)
{
    resize_for_mul (dest, a.begin (), a.end (), b.begin (), b.end ());
}


void
add (unit_vec_type & dest, const_iterator a_begin, const_iterator a_end,
    const_iterator b_begin, const_iterator b_end, unit_type init_carry = 0)
{
    std::cout << "add(" << print_range (a_begin, a_end) << ", "
        << print_range (b_begin, b_end) << ")" << std::endl;

    unsigned a_size = a_end - a_begin;
    unsigned b_size = b_end - b_begin;

    assert (dest.size () >= a_size + 1);

    iterator it = dest.begin ();
    iterator const end = dest.end ();
    const_iterator a_it = a_begin;
    const_iterator b_it = b_begin;
    unit_type carry = init_carry;
    accum_type sum;

    for (; b_it != b_end; ++a_it, ++b_it, ++it)
    {
        sum = static_cast<accum_type>(*a_it) + *b_it + carry;
        *it = static_cast<unit_type>(sum % modulus);
        if (sum >= modulus)
            carry = static_cast<unit_type>(sum / modulus);
        else
            carry = 0;
    }

    for (; a_it != a_end; ++a_it, ++it)
    {
        sum = static_cast<accum_type>(*a_it) + carry;
        *it = static_cast<unit_type>(sum % modulus);
        carry = static_cast<unit_type>(sum / modulus);
    }

    if (carry != 0)
        *it = static_cast<unit_type>(carry);
    
    remove_leading_zeroes (dest);

    std::cout << "dest: " << print_vec (dest) << std::endl;
}


void
add (unit_vec_type & dest, unit_vec_type const & src_a, unit_vec_type const & src_b,
    unit_type init_carry = 0)
{
    unit_vec_type const * a;
    unit_vec_type const * b;

    if (src_a.size () > src_b.size ())
    {
        a = &src_a;
        b = &src_b;
    }
    else
    {
        a = &src_b;
        b = &src_a;
    }

    unsigned a_size = a->size ();
    unsigned b_size = b->size ();

    dest.resize (a->size () + 1);

    // We need to use a->begin () + a_size instead of a->end () because one of
    // the operands can be also output operand and the resize above will break
    // the a_size >= b_size assumption.

    add (dest, a->begin (), a->begin () + a_size,
        b->begin (), b->begin () + b_size, init_carry);
}


//! a - b
void
sub (unit_vec_type & dest, unit_vec_type const & src_a, unit_vec_type src_b)
{
    std::cout << "sub(" << print_vec (src_a) << ", "
        << print_vec (src_b) << ")" << std::endl;


    unsigned a_size;

    assert (src_a.size () >= src_b.size ());

    a_size = src_a.size ();
    src_b.resize (a_size);

    for (unsigned i = 0; i != a_size; ++i)
        src_b[i] = modulus - 1 - src_b[i];

    add (dest, src_a, src_b, 1);

    if (dest.size () > 1)
        dest.resize (dest.size () - 1);
    
    remove_leading_zeroes (dest);

    std::cout << "sub dest: " << print_vec (dest) << std::endl;
}


void
karatsuba_rec (unit_vec_type & dest,
    const_iterator const a_begin, const_iterator const a_end,
    const_iterator const b_begin, const_iterator const b_end)
{
    std::cout << "karatsuba_rec(" << print_range (a_begin, a_end) << ", "
        << print_range (b_begin, b_end) << ")" << std::endl;

    unsigned const a_size = a_end - a_begin;
    unsigned const b_size = b_end - b_begin;

    std::cout << "a_size: " << a_size << "  b_size: " << b_size << std::endl;

    if (a_size == 1 && *a_begin == 0
        || b_size == 1 && *b_begin == 0)
    {
        dest = zero_value;
        std::cout << "dest: " << print_vec (dest) << std::endl;
        return;
    }
    else if (a_size == 1 && b_size == 1)
    {
        accum_type prod = static_cast<accum_type>(*a_begin) * *b_begin;
        dest[0] = static_cast<unit_type>(prod % modulus);

        unit_type high = static_cast<unit_type>(prod / modulus);
        if (high != 0)
        {
            dest.resize (2);
            dest[1] = high;
        }

        remove_leading_zeroes (dest);

        std::cout << "dest: " << print_vec (dest) << std::endl;
        return;
    }

    unsigned split_size;
    if (a_size > b_size && b_size != 1)
        split_size = b_size / 2;
    else
        split_size = a_size / 2;

    std::cout << "split_size: " << split_size << std::endl;

    const_iterator const & a0_begin = a_begin;
    const_iterator const a1_begin = a_begin + split_size;
    const_iterator const a0_end = a1_begin;
    const_iterator const & a1_end = a_end;
    unsigned a0_size = a0_end - a0_begin;
    unsigned a1_size = a1_end - a1_begin;

    const_iterator const & b0_begin = b_begin;
    const_iterator const b1_begin = b_begin + split_size;
    const_iterator const b0_end = b1_begin;
    const_iterator const & b1_end = b_end;
    unsigned b0_size = b0_end - b0_begin;
    unsigned b1_size = b1_end - b1_begin;

    // z2 = x1 * y1
    unit_vec_type z2 (zero_value);
    if (a1_size != 0 && b1_size != 0)
        karatsuba_rec (z2, a1_begin, a1_end, b1_begin, b1_end);

    // z0 = x0 * y0
    unit_vec_type z0 (zero_value);
    if (a0_size != 0 && b1_size != 0)
        karatsuba_rec (z0, a0_begin, a0_end, b0_begin, b0_end);
    
    // z1 = (x1 + x0)(y1 + y0) - (z2 + z0)
    unit_vec_type z1, a01_sum, b01_sum, z02_sum;

    resize_for_add (a01_sum, a0_begin, a0_end, a1_begin, a1_end);
    add (a01_sum, a0_begin, a0_end, a1_begin, a1_end);
    
    resize_for_add (b01_sum, b0_begin, b0_end, b1_begin, b1_end);
    add (b01_sum, b0_begin, b0_end, b1_begin, b1_end);

    add (z02_sum, z0, z2);
    
    unit_vec_type z1_ab_prod;
    resize_for_mul (z1_ab_prod, a01_sum.begin (), a01_sum.end (),
        b01_sum.begin (), b01_sum.end ());
    karatsuba_rec (z1_ab_prod, a01_sum.begin (), a01_sum.end (),
        b01_sum.begin (), b01_sum.end ());

    sub (z1, z1_ab_prod, z02_sum);

    modulus_power (z2, split_size * 2);
    modulus_power (z1, split_size);
    
    dest = zero_value;
    add (dest, dest, z2);
    add (dest, dest, z1);
    add (dest, dest, z0);

    std::cout << "dest: " << print_vec (dest) << std::endl;
}


void
karatsuba (unit_vec_type & dest, unit_vec_type const & a, unit_vec_type const & b)
{
    dest = zero_value;
    resize_for_mul (dest, a, b);
    karatsuba_rec (dest, a.begin (), a.end (), b.begin (), b.end ());
}


void
test_sub ()
{
    // 1 024
    const unit_type a_array[] = { 4, 2, 0, 1 };
    
    // 2 048
    const unit_type b_array[] = { 8, 4, 0, 2 };

    unit_vec_type a (a_array, a_array + 4);
    unit_vec_type b (b_array, b_array + 4);
    unit_vec_type result;

    sub (result, b, a);
    return;
}


void
test_mul ()
{
    // 1 024
    //const unit_type a_array[] = { 4, 2, 0, 1 };    
    // 2 048
    //const unit_type b_array[] = { 8, 4, 0, 2 };

    // 1 024
    const unit_type a_array[] = { 1, 4 };    
    // 2 048
    const unit_type b_array[] = { 2, 3 };


    unit_vec_type a (a_array, a_array + 2);
    unit_vec_type b (b_array, b_array + 2);
    unit_vec_type result;

    karatsuba (result, b, a);
    return;
}


int _tmain(int argc, _TCHAR* argv[])
{
    //test_sub ();
    test_mul ();

    return 0;
}


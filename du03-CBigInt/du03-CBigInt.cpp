// du03-CBigInt.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#ifndef __PROGTEST__
#include <iostream>
#include <string>
#include <cstdlib>

#endif // #ifndef __PROGTEST__


template <typename T>
class vec
{
public:
    typedef T value_type;
    typedef T * iterator;
    typedef T const * const_iterator;


    vec ()
    {
        init (1);
    }


    explicit
    vec (unsigned len)
    {
        init (len == 0 ? 1 : len);
    }


    vec (unsigned len, T const & val)
    {
        init (len == 0 ? 1 : len);
        iterator end_it = end ();
        for (iterator it = begin (); it != end_it; ++it)
            *it = val;
    }

    
    ~vec ()
    {
        free (buf);
    }


    iterator
    begin ()
    {
        return buf;
    }


    iterator
    end ()
    {
        return buf_last;
    }


    const_iterator
    begin () const
    {
        return buf;
    }


    const_iterator
    end () const
    {
        return buf_last;
    }


    unsigned
    size () const
    {
        return buf_last - buf;
    }


    unsigned
    capacity () const
    {
        return buf_end - buf;
    }


    void
    resize (unsigned new_size)
    {
        unsigned old_size = size ();
        if (new_size > old_size)
            reserve (new_size);

        buf_last = buf + new_size;
    }


    void
    reserve (unsigned new_cap)
    {
        unsigned old_size = size ();
        unsigned old_cap = capacity ();
        if (new_cap <= old_cap)
            return;
        
        value_type * new_buf = vec_realloc (buf, new_cap);
        if (! new_buf)
            throw "realloc";

        buf = new_buf;
        buf_last = buf + old_size;
        buf_end = buf + new_cap;
    }


private:
    void
    init (unsigned cap)
    {
        buf = vec_malloc (cap);
        buf_last = buf;
        buf_end = buf + cap;
    }


    static
    value_type *
    vec_malloc (unsigned count)
    {
        return static_cast<value_type *>(malloc (count * sizeof (value_type)));
    }
    

    static
    value_type *
    vec_realloc (value_type * buf, unsigned count)
    {
        value_type * new_buf
            = static_cast<value_type *>(
                realloc (buf, count * sizeof (value_type)));

        return new_buf;
    }


    value_type * buf;
    value_type * buf_last;
    value_type * buf_end;
};


class CBigInt;


std::ostream & operator << (std::ostream &, CBigInt const &);
std::istream & operator >> (std::istream &, CBigInt &);


class CBigInt
{
public:
    typedef unsigned short unit_type;

    CBigInt ()
    {
        init (1);
    }

    CBigInt (unsigned x)
    {

    }

    CBigInt (char const *)
    { }

    CBigInt (CBigInt const &)
    { }

    ~CBigInt ()
    { }

    CBigInt & operator = (CBigInt & other)
    {
        other.swap (*this);
    }

    void
    swap (CBigInt & other)
    {
    }

    friend std::ostream & operator << (std::ostream &, CBigInt const &);
    friend std::istream & operator >> (std::istream &, CBigInt &);


private:
    typedef vec<unit_type> storage_type;

    void
    init (unsigned size)
    {
        buf.resize (size);
    }

    storage_type buf;
};


std::ostream &
operator << (std::ostream & os, CBigInt const & x)
{
    return os;
}


std::istream &
operator >> (std::istream & is, CBigInt & x)
{
    return is;
}


#ifndef __PROGTEST__
int
main(int argc, char * argv[])
{
    CBigInt x;
	return 0;
}

#endif // __PROGTEST__

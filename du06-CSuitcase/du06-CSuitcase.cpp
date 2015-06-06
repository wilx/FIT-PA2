#ifndef __PROGTEST__
#include <ostream>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>

#define xassert(X) assert(X)

#else
#define xassert(X) /*empty*/

#endif


template <typename T>
void
xswap (T & a, T & b)
{
    T tmp (a);
    a = b;
    b = tmp;
}


static
char *
xstrdup (char const * s)
{
    size_t len = std::strlen (s);
    char * buf = new char[len + 1];
    std::strncpy (buf, s, len + 1);
    return buf;
}


template <typename T>
class vec
{
public:
    typedef T value_type;
    typedef T * iterator;
    typedef T const * const_iterator;
    typedef T & reference;
    typedef T const & const_reference;
    typedef unsigned size_type;

    vec ()
    {
        init (1);
    }


    vec (vec const & other)
    {
        unsigned other_size = other.size ();
        init (other_size);

        buf_last = buf + other_size;

        const_iterator other_it = other.begin ();
        for (iterator it = begin (); it != end (); ++it, ++other_it)
            *it = *other_it;
    }


    explicit
    vec (unsigned len)
    {
        init (len == 0 ? 1 : len);
        resize (len);
    }


    vec (unsigned len, T const & val)
    {
        init (len == 0 ? 1 : len);
        buf_last = buf + len;
        iterator end_it = end ();
        for (iterator it = begin (); it != end_it; ++it)
            *it = val;
    }


    vec (const_iterator b, const_iterator e)
    {
        unsigned len = e - b;
        init (len == 0 ? 1 : len);
        buf_last = buf + len;
        iterator end_it = end ();
        for (iterator it = begin (); it != end_it; ++it, ++b)
            *it = *b;
    }


    vec &
    operator = (vec const & other)
    {
        vec (other).swap (*this);
        return *this;
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


    reference
    operator [] (unsigned i)
    {
        return buf[i];
    }


    const_reference
    operator [] (unsigned i) const
    {
        return buf[i];
    }


    unsigned
    size () const
    {
        return static_cast<size_type>(buf_last - buf);
    }


    unsigned
    capacity () const
    {
        return static_cast<size_type>(buf_end - buf);
    }


    void
    resize (unsigned new_size, T const & val = T ())
    {
        unsigned old_size = size ();
        if (new_size > old_size)
        {
            reserve (new_size);

            buf_last = buf + new_size;

            for (iterator it = begin () + old_size; it != end (); ++it)
                *it = val;
        }
        else
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


    void
    push_back (T const & x)
    {
        unsigned const old_size = size ();
        resize (old_size + 1);
        buf[old_size] = x;
    }


    void
    swap (vec & other)
    {
        xswap (buf, other.buf);
        xswap (buf_last, other.buf_last);
        xswap (buf_end, other.buf_end);
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
        return static_cast<value_type *>(std::malloc (count * sizeof (value_type)));
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


struct Item
{
    virtual ~Item () = 0;

    virtual unsigned weight () const = 0;
    virtual bool danger () const = 0;
    virtual void print (std::ostream & os) const = 0;
    virtual Item * clone () const = 0;
};


Item::~Item ()
{ }


struct SafeItem
    : public Item
{
    virtual
    bool
    danger () const
    {
        return false;
    }
};


class CClothes
    : public SafeItem
{
public:
    CClothes (char const * d)
        : desc (xstrdup (d))
    { }


    ~CClothes ()
    {
        delete[] desc;
    }

    CClothes (CClothes const & other)
        : SafeItem (other)
        , desc (xstrdup (other.desc))
    { }

    CClothes &
    operator = (CClothes const & other)
    {
        CClothes (other).swap (*this);
        return *this;
    }

    virtual
    unsigned
    weight () const
    {
        return 500;
    }

    virtual
    void
    print (std::ostream & os) const
    {
        os << "Clothes (" << desc << ")";
    }

    virtual
    Item *
    clone () const
    {
        return new CClothes (*this);
    }

    void
    swap (CClothes & other)
    {
        xswap (desc, other.desc);
    }

private:
    char const * desc;
};


class CShoes
    : public SafeItem
{
public:
    virtual
    unsigned
    weight () const
    {
        return 750;
    }

    virtual
    void
    print (std::ostream & os) const
    {
        os << "Shoes";
    }

    virtual
    Item *
    clone () const
    {
        return new CShoes (*this);
    }
};


class CKnife
    : public Item
{
public:
    explicit
    CKnife (unsigned l)
        : len (l)
    { }

    virtual
    unsigned
    weight () const
    {
        return 100;
    }

    virtual
    void
    print (std::ostream & os) const
    {
        os << "Knife " << len << " cm";
    }

    virtual
    bool
    danger () const
    {
        return len > 7;
    }

    virtual
    Item *
    clone () const
    {
        return new CKnife (*this);
    }

private:
    unsigned len;
};


class CGPS
    : public SafeItem
{
    virtual
    unsigned
    weight () const
    {
        return 250;
    }

    virtual
    void
    print (std::ostream & os) const
    {
        os << "GPS";
    }

    virtual
    Item *
    clone () const
    {
        return new CGPS (*this);
    }
};


class CSuitcase
{
public:
    CSuitcase ()
    { }

    CSuitcase (CSuitcase const & other)
    {
        items.resize (other.items.size ());
        storage_type::iterator it = items.begin ();
        storage_type::const_iterator other_it = other.items.begin ();
        for (; it != items.end (); ++it, ++other_it)
            *it = (*other_it)->clone ();
    }

    CSuitcase &
    operator = (CSuitcase const & other)
    {
        CSuitcase (other).swap (*this);
        return *this;
    }


    void
    swap (CSuitcase & other)
    {
        items.swap (other.items);
    }


    virtual
    ~CSuitcase ()
    {
        for (storage_type::iterator it = items.begin (); it != items.end ();
            ++it)
            delete *it;
    }


    CSuitcase &
    Add (Item const & cl)
    {
        items.push_back (cl.clone ());
        return *this;
    }

    unsigned
    Weight () const
    {
        unsigned w = 0;
        for (storage_type::const_iterator it = items.begin ();
            it != items.end (); ++it)
            w += (*it)->weight ();

        w += 2000;

        return w;
    }

    unsigned
    Count () const
    {
        return static_cast<unsigned>(items.size ()) + 1;
    }

    bool
    Danger () const
    {
        for (storage_type::const_iterator it = items.begin ();
            it != items.end (); ++it)
            if ((*it)->danger ())
                return true;

        return false;
    }

    void
    print (std::ostream & os) const
    {
        os << "Suitcase" << std::endl;
        storage_type::const_iterator end_it = items.end ();
        for (storage_type::const_iterator it = items.begin (); it != end_it;
            ++it)
        {
            if (it + 1 == end_it)
                os << "\\-";
            else
                os << "+-";

            (*it)->print (os);
            os << std::endl;
        }
    }

    friend
    std::ostream &
    operator << (std::ostream & os, CSuitcase const & sc)
    {
        sc.print (os);
        return os;
    }

private:
    //typedef std::vector<Item *> storage_type;
    typedef vec<Item *> storage_type;

    storage_type items;
};


#ifndef __PROGTEST__
int 
main()
{
    using namespace std;

    CSuitcase T0;
    T0 . Add ( CKnife ( 7 ) );
    T0 . Add ( CGPS () );
    T0 . Add ( CClothes ( "red T-shirt" ) );
    T0 . Add ( CShoes () );
    cout << T0;
    /*-----------------
    Suitcase
    +-Knife 7 cm
    +-GPS
    +-Clothes (red T-shirt)
    \-Shoes
    -------------------*/
    xassert (T0 . Weight () == 3600);
    xassert (T0 . Count () == 5);
    xassert (T0 . Danger () == false);

    CSuitcase T1;
    T1 . Add ( CKnife ( 5 ) );
    T1 . Add ( CClothes ( "red T-shirt" ) );
    T1 . Add ( CClothes ( "blue jeans" ) );
    T1 . Add ( CShoes () );
    cout << T1;
    /*-----------------
    Suitcase
    +-Knife 5 cm
    +-Clothes (red T-shirt)
    +-Clothes (blue jeans)
    \-Shoes
    -------------------*/
    xassert (T1 . Weight () == 3850);
    xassert (T1 . Count () == 5);
    xassert (T1 . Danger () == false);

    CSuitcase T2;
    T2 . Add ( CKnife ( 8 ) ) . Add ( CClothes ( "white underwear" ) ) . Add ( CShoes () );
    T2 . Add ( CClothes ( "red T-shirt" ) );
    T2 . Add ( CClothes ( "brown jacket" ) );
    T2 . Add ( CShoes () );
    cout << T2;
    /*-----------------
    Suitcase
    +-Knife 8 cm
    +-Clothes (white underwear)
    +-Shoes
    +-Clothes (red T-shirt)
    +-Clothes (brown jacket)
    \-Shoes
    -------------------*/
    xassert (T2 . Weight () == 5100);
    xassert (T2 . Count () == 7);
    xassert (T2 . Danger () == true);

    T2 = T1;

	return 0;
}

#endif

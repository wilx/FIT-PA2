#define xassert(X) assert (X)
//#define xassert(X)


template <class T>
class CList
{
public:
    CList ()
        : m_First (0)
        , m_Last (0)
        , m_Size (0)
    { }


    ~CList ()
    {
        delete_list ();
    }


    CList (CList const & other)
        : m_First (0)
        , m_Last (0)
        , m_Size (0)
    {
        for (TItem const * it = other.m_First; it; it = it->m_Next)
            InsEnd (it->m_Val);
    }


    CList &
    operator = (CList const & other)
    {
        CList (other).swap (*this);
        return *this;
    }


    void
    swap (CList & other)
    {
        xswap (m_First, other.m_First);
        xswap (m_Last, other.m_Last);
        xswap (m_Size, other.m_Size);
    }

    int
    Size () const
    {
        return m_Size;
    }


    bool
    InsBefore (int pos, const T & x)
    {
        TItem * pos_it;
        bool ret = find_pos (pos_it, pos);
        if (! ret)
            return false;

        TItem * prev_node = pos_it->m_Prev;
        TItem * next_node = pos_it;
        TItem * new_node = new TItem (x, prev_node, next_node);
        
        if (prev_node)
            prev_node->m_Next = new_node;
        else
            m_First = new_node;

        if (next_node)
            next_node->m_Prev = new_node;
        else
            m_Last = new_node;

        ++m_Size;

        return true;
    }


    bool
    InsAfter (int pos, const T & x)
    {
        TItem * pos_it;
        bool ret = find_pos (pos_it, pos);
        if (! ret)
            return false;

        TItem * prev_node = pos_it;
        TItem * next_node = pos_it->m_Next;
        TItem * new_node = new TItem (x, prev_node, next_node);
        
        if (prev_node)
            prev_node->m_Next = new_node;
        else
            m_First = new_node;

        if (next_node)
            next_node->m_Prev = new_node;
        else
            m_Last = new_node;

        ++m_Size;

        return true;
    }


    bool
    InsStart (const T & x)
    {
        TItem * new_node = new TItem (x, 0, m_First);
        if (m_First)
            m_First->m_Prev = new_node;

        m_First = new_node;

        if (! m_Last)
            m_Last = new_node;

        ++m_Size;

        return true;
    }


    bool
    InsEnd (const T & x)
    {
        TItem * new_node = new TItem (x, m_Last, 0);
        if (m_Last)
            m_Last->m_Next = new_node;
            
        m_Last = new_node;

        if (! m_First)
            m_First = new_node;

        ++m_Size;

        return true;
    }


    bool
    Read (int pos, T & x) const
    {
        TItem const * pos_it;
        bool ret = find_pos (pos_it, pos);
        if (! ret)
            return false;

        x = pos_it->m_Val;

        return true;
    }


    bool
    Delete (int pos)
    {
        TItem * pos_it;
        bool ret = find_pos (pos_it, pos);
        if (! ret)
            return false;

        TItem * prev_node = pos_it->m_Prev;
        TItem * next_node = pos_it->m_Next;

        if (prev_node)
            prev_node->m_Next = next_node;

        if (next_node)
            next_node->m_Prev = prev_node;

        if (pos_it == m_First)
            m_First = next_node;

        if (pos_it == m_Last)
            m_Last = prev_node;

        delete pos_it;
        --m_Size;

        if (m_Size == 0)
        {
            m_First = 0;
            m_Last = 0;
        }

        return true;
    }

protected:
    template <typename U>
    void
    xswap (U & a, U & b)
    {
        U tmp (a);
        a = b;
        b = tmp;
    }


    struct TItem;


    void
    delete_list ()
    {
        for (TItem const * it = m_First; it;)
        {
            TItem const * tmp = it;
            advance_ptr (it);
            delete tmp;
        }

        m_First = 0;
        m_Last = 0;
    }


    static
    void
    advance_ptr (TItem const * & ptr)
    {
        ptr = ptr->m_Next;
    }


    bool
    find_pos (TItem const * & ptr, int pos) const
    {
        if (! (0 <= pos && pos < m_Size))
            return false;

        int i;
        TItem const * pos_it = m_First;

        for (i = 0; i != pos && pos_it; advance_ptr (pos_it), ++i)
            ;

        if (pos_it)
        {
            //xassert (i == pos);
            ptr = pos_it;
            return true;
        }
        else
            return false;
    }


    bool
    find_pos (TItem * & ptr, int pos)
    {
        return find_pos (const_cast<TItem const * &>(ptr), pos);
    }


    struct TItem
    {
        TItem   * m_Next;
        TItem   * m_Prev;
        T         m_Val;

        TItem (T const & x)
            : m_Next (0)
            , m_Prev (0)
            , m_Val (x)
        { }

        TItem (T const & x, TItem * l, TItem * r)
            : m_Next (r)
            , m_Prev (l)
            , m_Val (x)
        { }
    };

    TItem      * m_First;
    TItem      * m_Last;
    int m_Size;
};



#ifndef __PROGTEST__
#include <iostream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <ctime>


template <typename T>
void
test_random_ops (CList<T> & list, unsigned ro)
{
    unsigned rand_pos = list.Size () != 0 ? std::rand () % list.Size () : std::rand ();
    bool status = true;
    const unsigned NUM = 5;
    switch (ro % NUM)
    {
    case 0:
        std::cout << "InsBefore (" << rand_pos << ")" << std::endl;
        status = list.InsBefore (rand_pos, T ());
        break;

    case 1:
        std::cout << "InsBefore (" << rand_pos << ")" << std::endl;
        status = list.InsAfter (rand_pos, T ());
        break;

    case 2:
        std::cout << "InsStart ()" << std::endl;
        status = list.InsStart (T ());
        break;

    case 3:
        std::cout << "InsEnd ()" << std::endl;
        status = list.InsEnd (T ());
        break;

    case 4:
    {
        std::cout << "Read(" << rand_pos << ")" << std::endl;
        T x;
        status = list.Read (rand_pos, x);
        break;
    }

    case 5:
        std::cout << "Delete(" << rand_pos << ")" << std::endl;
        status = list.Delete (rand_pos);
        break;
    }

    std::cout << "status == " << status << std::endl;
}


int main()
{
    using namespace std;

    std::srand (std::time (0));

    bool   status;
    int    tempInt;
    string tempStr;
    CList<int>  LI1;
    status = LI1 . InsEnd ( 3000 );
    xassert (status == true);
    xassert (LI1 . Size () == 1);
    // (3000)

    status = LI1 . InsEnd ( 5000 );
    xassert (status == true);
    xassert (LI1 . Size () == 2);
    // (3000, 5000)
    status = LI1 . InsEnd ( 1000 );
    xassert (status == true);
    xassert (LI1 . Size () == 3);
    // (3000, 5000, 1000)
    status = LI1 . Read  ( 0, tempInt );
    xassert (status == true);
    xassert (tempInt==3000);
    status = LI1 . Read  ( 1, tempInt );
    xassert (status == true);
    xassert (tempInt==5000);
    status = LI1 . Read  ( 2, tempInt );
    xassert (status == true);
    xassert (tempInt==1000);

    while (LI1.Size () > 0)
        LI1.Delete (LI1.Size () - 1);

    CList<int>  LI2;
    status = LI2 . InsStart ( 10 );
    xassert (status == true);
    xassert (LI2 . Size () == 1);
    // (10)
    status = LI2 . InsAfter ( 0, 20 );
    xassert (status == true);
    xassert (LI2 . Size () == 2);
    // (10, 20)
    status = LI2 . InsEnd ( 30 );
    xassert (status == true);
    xassert (LI2 . Size () == 3);
    // (10, 20, 30)
    status = LI2 . InsBefore ( 2, 40 );
    xassert (status == true);
    xassert (LI2 . Size () == 4);
    // (10, 20, 40, 30)
    status = LI2 . Read  ( 0, tempInt );
    xassert (status == true);
    xassert (tempInt==10);
    status = LI2 . Read  ( 1, tempInt );
    xassert (status == true);
    xassert (tempInt==20);
    status = LI2 . Read  ( 2, tempInt );
    xassert (status == true);
    xassert (tempInt==40);
    status = LI2 . Read  ( 3, tempInt );
    xassert (status == true);
    xassert (tempInt==30);
    status = LI2.Delete (-1);
    xassert (status == false);
    status = LI2.Delete (1000);
    xassert (status == false);

    CList<int>  LI3;
    status = LI3 . InsEnd ( 10 );
    xassert (status == true);
    xassert (LI3 . Size () == 1);
    // (10)
    status = LI3 . InsEnd ( 20 );
    xassert (status == true);
    xassert (LI3 . Size () == 2);
    // (10, 20)
    status = LI3 . InsEnd ( 30 );
    xassert (status == true);
    xassert (LI3 . Size () == 3);
    // (10, 20, 30)
    status = LI3 . InsEnd ( 40 );
    xassert (status == true);
    xassert (LI3 . Size () == 4);
    // (10, 20, 30, 40)
    status = LI3 . Delete ( 2 );
    xassert (status == true);
    xassert (LI3 . Size () == 3);
    // (10, 20, 40)
    status = LI3 . InsBefore ( 2, 50 );
    xassert (status == true);
    xassert (LI3 . Size () == 4);
    // (10, 20, 50, 40)
    status = LI3 . Read  ( 3, tempInt );
    xassert (status == true);
    xassert (tempInt==40);
    status = LI3 . InsBefore ( 4, 60 );
    xassert (status == false);
    xassert (LI3 . Size () == 4);
    // (10, 20, 50, 40)
    status = LI3 . InsAfter ( 3, 70 );
    xassert (status == true);
    xassert (LI3 . Size () == 5);
    // (10, 20, 50, 40, 70)

    CList<string>  LS;
    status = LS . InsEnd ( "first" );
    xassert (status == true);
    xassert (LS . Size () == 1);
    // (first)
    status = LS . InsEnd ( "third" );
    xassert (status == true);
    xassert (LS . Size () == 2);
    // (first, third)
    status = LS . InsAfter ( 0, "second" );
    xassert (status == true);
    xassert (LS . Size () == 3);
    // (first, second, third)
    status = LS . Read  ( 2, tempStr );
    xassert (status == true);
    xassert (tempStr=="third");


    CList<string> LS2 (LS), LS3 (LS);

    while (LS2.Size () > 0)
        LS2.Delete (0);

    while (LS2.Size () > 0)
        LS2.Delete (LS2.Size () - 1);


    CList<string> rl;
    const unsigned LOOPS = 100;
    for (unsigned i = 0; i != LOOPS; ++i)
    {
        test_random_ops (rl, std::rand ());
    }

    return 0;
}
#endif

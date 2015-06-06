#ifndef __PROGTEST__

#endif /* __PROGTEST__ */
#include <fstream>
#include <iostream>
#include <bitset>
#include <locale>

bool UTF8toUTF16 (const char * src, const char * dst);
bool UTF16toUTF8 (const char * src, const char * dst);

static unsigned
count_leading_ones (unsigned char x)
{
    unsigned i;
    unsigned mask = 0x80;
    for (i = 0; mask != 0 && (x & mask) != 0; mask >>= 1, ++i)
        ;

    return i;
}


class UTF8ToUnicode
{
public:
    UTF8ToUnicode ()
        : cp (0)
        , to_read (0)
        , initial (true)
        , complete (false)
    { }


    bool
    put_octet (unsigned char ch)
    {
        if (initial)
        {
            unsigned const leading_ones = count_leading_ones (ch);
            
            // There should never be more than six ones in initial UTF-8 byte.
            if (leading_ones > 6)
                return false;

            std::cout << "count_leading_ones(" << std::hex << +ch << std::dec
                << "/" << std::bitset<8>(ch).to_string () << "): "
                << leading_ones << std::endl;

            if (leading_ones == 0)
            {
                // ASCII
                to_read = 0;
                cp = ch;
            }
            else
            {
                // Above ASCII.
                initial = false;
                to_read = leading_ones - 1;                
                unsigned char const mask
                    = ((0xFF << leading_ones) & 0xFF) >> leading_ones;
                cp = ch & mask;
            }
        }
        else
        {
            // Two upper bits should be 10xxxxxx.
            if ((ch & ~0xC0) == 0x80)
                return false;

            cp = (cp << 6) | (ch & 0x7F);
            --to_read;
        }

        if (to_read == 0)
        {
            complete = true;
        }

        return true;
    }


    unsigned
    get_cp () const
    {
        return cp;
    }


    bool
    is_complete () const
    {
        return complete;
    }


    void
    reset ()
    {
        *this = UTF8ToUnicode ();
    }

private:
    //! Code point value.
    unsigned cp;

    //! Trailing octets to read.
    unsigned to_read;

    //! Initial state.
    bool initial;

    //! Complete code point.
    bool complete;
};


class UnicodeToUTF16
{
public:
    UnicodeToUTF16 ()
    { }


    bool
    put_cp (unsigned cp)
    {
        return true;
    }




private:
    unsigned char buf[4];
};


bool
UTF8toUTF16 (const char * src, const char * dst)
try
{
    std::fstream infile;
    infile.imbue (std::locale::classic ());
    infile.exceptions (std::ios_base::badbit);
    infile.open (src, std::ios_base::binary | std::ios_base::in);
    if (! infile.good ())
        return false;

    std::ofstream outfile;
    outfile.exceptions (std::ios_base::badbit);
    outfile.open (dst, std::ios_base::trunc | std::ios_base::binary);
    if (! outfile.good ())
        return false;

    char ch;
    UTF8ToUnicode src_state;
    UnicodeToUTF16 dest_state;
    infile.seekg (0);
    while (infile.read (&ch, 1).good ())
    {
        if (! src_state.put_octet (ch))
            return false;

        if (src_state.is_complete ())
        {
            dest_state.put_cp (src_state.get_cp ());
        }
    }

    return true;
}
catch (std::exception const &)
{
    return false;
}

#ifndef __PROGTEST__
int
main ()   
{
    std::locale::global (std::locale::classic ());
    UTF8toUTF16 ("C:\\wilx\\utf8.txt", "output-utf16.txt");
  
    return (0);
}

#endif /* __PROGTEST__ */

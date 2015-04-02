/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/10/27
    Description: 

*******************************************************************************/

#pragma once

namespace HTMLParserLibrary
{
    class ConvertedStream : public ByteStream
    {
    private:
        ByteStream *str;
        iconv_t conv;

        char *buffer_in;
        char *ptr_to_buffer_in;
        char *buffer_out;
        char *ptr_to_buffer_out;
        size_t in_buffer_size;
        size_t out_buffer_size;
        size_t in_counter;
        size_t out_counter;

        size_t chars_ready_out;
        char *out_ptr;

        void ConvertInput();
        void LoadInput();
        void LoadNext();
        bool firstTime;
    protected:
    public:
        void set_input(ByteStream *_str);
        void set_encoding(const char *enc);
        void reset();
        virtual int get();
        ConvertedStream(size_t in_size = 16, size_t out_size = 64, ByteStream *in_stream = NULL, const char *enc = "UTF-8");
        virtual ~ConvertedStream();
    };
}


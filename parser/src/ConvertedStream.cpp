/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/10/27

*******************************************************************************/

#include "ParserStartup.h"
#include "HTMLParserIncludes.h"

using namespace HTMLParserLibrary;

void ConvertedStream::LoadNext()
{
    LoadInput();
    ConvertInput();
    char *tmp_ptr = buffer_in;
    for(size_t i = 0; i < in_counter; i++)
    {
        buffer_in[i] = ptr_to_buffer_in[i];
        tmp_ptr++;
    }
    ptr_to_buffer_in = tmp_ptr;
    in_counter = in_buffer_size - in_counter;
}

void ConvertedStream::LoadInput()
{
    for(; in_counter > 0; in_counter--)
    {
        int val = str->get();
        if(val == -1)
            break;
        else
        {
            *ptr_to_buffer_in = (char)val;
            ptr_to_buffer_in++;
        }
    }

    in_counter = in_buffer_size - in_counter;
    ptr_to_buffer_in = buffer_in;
}

void ConvertedStream::ConvertInput()
{
    // convert
    iconv(conv,const_cast<const char**>(&ptr_to_buffer_in),&in_counter,&ptr_to_buffer_out,&out_counter);

    // prepare for output
    chars_ready_out = out_buffer_size - out_counter;
    out_ptr = buffer_out;

    // prepare for further read
    ptr_to_buffer_out = buffer_out;
    out_counter = out_buffer_size;
}

int ConvertedStream::get()
{
    if(str == NULL)
        return -1;
    if(chars_ready_out <= 0)
    {
        LoadNext();
        if(chars_ready_out <= 0)
            return -1;
        else
        {
            char out = *out_ptr;
            out_ptr++;
            chars_ready_out--;
#ifdef BENCHMARK
			byteCount++;
#endif
        	return out;
        }
    }
    else
    {
        char out = *out_ptr;
        out_ptr++;
        chars_ready_out--;
#ifdef BENCHMARK
		byteCount++;
#endif
        return out;
    }
}

ConvertedStream::ConvertedStream(size_t in_size, size_t out_size, ByteStream *in_stream, const char *cd)
{
	firstTime = true;

    str = in_stream;
    set_encoding(cd);
    
    firstTime = false;

    buffer_in = new char[in_size];
    ptr_to_buffer_in = buffer_in;

    buffer_out = new char[out_size];
    ptr_to_buffer_out = buffer_out;

    in_buffer_size = in_size;
    out_buffer_size = out_size;

    in_counter = in_buffer_size;
    out_counter = out_buffer_size;

    chars_ready_out = 0;
    out_ptr = ptr_to_buffer_out;
}

ConvertedStream::~ConvertedStream()
{
    delete buffer_in;
    delete buffer_out;
}

void ConvertedStream::reset()
{
    ptr_to_buffer_in = buffer_in;

    ptr_to_buffer_out = buffer_out;

    in_counter = in_buffer_size;
    out_counter = out_buffer_size;

    chars_ready_out = 0;
    out_ptr = ptr_to_buffer_out;
}

void ConvertedStream::set_encoding(const char *enc)
{
	if(!firstTime)
    	iconv_close(conv);
    conv = iconv_open("UTF-8",enc);
}

void ConvertedStream::set_input(ByteStream *_str)
{
    str = _str;
}

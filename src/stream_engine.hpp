/*
    Copyright (c) 2009-2012 250bpm s.r.o.
    Copyright (c) 2007-2009 iMatix Corporation
    Copyright (c) 2007-2011 Other contributors as noted in the AUTHORS file

    This file is part of Crossroads I/O project.

    Crossroads I/O is free software; you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Crossroads is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __XS_STREAM_ENGINE_HPP_INCLUDED__
#define __XS_STREAM_ENGINE_HPP_INCLUDED__

#include <stddef.h>

#include "fd.hpp"
#include "i_engine.hpp"
#include "io_object.hpp"
#include "encoder.hpp"
#include "decoder.hpp"
#include "options.hpp"
#include "wire.hpp"

namespace xs
{

    class io_thread_t;
    class session_base_t;

    //  This engine handles any socket with SOCK_STREAM semantics,
    //  e.g. TCP socket or an UNIX domain socket.

    class stream_engine_t : public io_object_t, public i_engine
    {
    public:

        stream_engine_t (fd_t fd_, const options_t &options_);
        ~stream_engine_t ();

        //  i_engine interface implementation.
        void plug (xs::io_thread_t *io_thread_,
           xs::session_base_t *session_);
        void unplug ();
        void terminate ();
        void activate_in ();
        void activate_out ();

        //  i_poll_events interface implementation.
        void in_event (fd_t fd_);
        void out_event (fd_t fd_);

    private:

        //  Function to handle network disconnections.
        void error ();

        //  Writes data to the socket. Returns the number of bytes actually
        //  written (even zero is to be considered to be a success). In case
        //  of error or orderly shutdown by the other peer -1 is returned.
        int write (const void *data_, size_t size_);

        //  Reads data from the socket (up to 'size' bytes). Returns the number
        //  of bytes actually read (even zero is to be considered to be
        //  a success). In case of error or orderly shutdown by the other
        //  peer -1 is returned.
        int read (void *data_, size_t size_);

        //  Underlying socket.
        fd_t s;

        handle_t handle;

        unsigned char *inpos;
        size_t insize;
        decoder_t decoder;

        unsigned char *outpos;
        size_t outsize;
        encoder_t encoder;

        //  The session this engine is attached to.
        xs::session_base_t *session;

        //  Detached transient session.
        xs::session_base_t *leftover_session;

        options_t options;

        bool plugged;

        //  Outgoing protocol header.
        sp_header_t out_header;

        //  Desired protocol header.
        sp_header_t desired_header;

        //  Incoming protocol header.
        sp_header_t in_header;

        unsigned char *header_pos;
        size_t header_remaining;

        //  Protocol header has been received/sent.
        bool header_received;
        bool header_sent;

        stream_engine_t (const stream_engine_t&);
        const stream_engine_t &operator = (const stream_engine_t&);
    };

}

#endif

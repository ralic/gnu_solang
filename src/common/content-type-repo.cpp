/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Santanu Sinha <santanu.sinha@gmail.com>
 *
 * Solang is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solang is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <algorithm>
#include <iterator>

#include <gdkmm.h>
#include <glibmm.h>
#include <gio/gio.h>
#include <giomm.h>

#include "content-type-repo.h"

namespace Solang
{

ContentTypeRepo::ContentTypeRepo() throw()
    :supportedTypes_()
{
}

ContentTypeRepo::~ContentTypeRepo() throw()
{
}

void
ContentTypeRepo::init() throw()
{
    PixbufFormatList formats = Gdk::Pixbuf::get_formats();
    for( PixbufFormatList::iterator format = formats.begin();
                    format != formats.end(); format++ )
    {
        std::vector<Glib::ustring> content_types
                                    = (*format).get_mime_types();
        for( std::vector<Glib::ustring>::iterator content
                        = content_types.begin();
                        content != content_types.end(); content++ )
        {
            supportedTypes_.push_back(
                g_content_type_from_mime_type( content->c_str() ));
        }
    }
    std::sort( supportedTypes_.begin(), supportedTypes_.end() );
    std::unique( supportedTypes_.begin(), supportedTypes_.end() );
    return;
}

Glib::ustring
ContentTypeRepo::get_content_type(const std::string &filename ) const throw()
{
    DataInputStreamPtr fin;

    if (true == Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
    {
        const FilePtr file = Gio::File::create_for_path(filename);

        try
        {
            fin = Gio::DataInputStream::create(file->read());
        }
        catch(const Gio::Error & e)
        {
            g_warning("%s", e.what().c_str());
        }
    }

    const gint SNIFF_BUFFER_SIZE = 4096; // From gdk-pixbuf-io.c
    guint8 buffer[SNIFF_BUFFER_SIZE];
    gssize nread = 0;

    if (0 != fin)
    {
        nread = fin->read(buffer, sizeof(buffer));
    }

    bool uncertain = false;
    Glib::ustring contentType
        = Gio::content_type_guess(
                filename, buffer, nread, uncertain );

    if( uncertain )
        return Glib::ustring();

    return contentType;
}

bool
ContentTypeRepo::is_gdk_supported(const Glib::ustring & content_type)
                                  const throw()
{
    return std::binary_search(supportedTypes_.begin(),
                              supportedTypes_.end(),
                              content_type);
}

const ContentTypeRepoPtr
ContentTypeRepo::instance() throw()
{
    static ContentTypeRepo repoObj;
    return &repoObj;
}

} //namespace Solang

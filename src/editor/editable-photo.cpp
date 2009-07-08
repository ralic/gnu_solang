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
#endif

#include <iostream>

#include <glibmm/i18n.h>
#include <giomm.h>

#include "edit-action.h"
#include "editable-photo.h"

namespace Solang
{

EditablePhoto::EditablePhoto( const PhotoPtr &photo ) throw()
    :photo_( photo ),
    buffer_( 0 ),
    image_(),
    toSave_( false )
{
    setup_photo_for_edit();
}

EditablePhoto::~EditablePhoto()
{
}

void
EditablePhoto::set_photo( const PhotoPtr &photo ) throw()
{
    photo_ = photo;
    setup_photo_for_edit();
}

void
EditablePhoto::set_buffer(const PixbufPtr &buffer )
{
    buffer_ = buffer;
//    if( photo_->get_buffer() )
    {
        photo_->set_buffer( buffer_ );
    }
}

void
EditablePhoto::save( Engine &engine ) throw(Error)
{
    if( photo_->get_content_type().find("jpeg") != 0 ) //JPEG
    {
        std::vector<Glib::ustring> keys;
        keys.push_back("quality");
        std::vector<Glib::ustring> values;
        values.push_back("100");

        gchar *buf = NULL;
        gsize size = 0L;

        buffer_->save_to_buffer( buf, size, "jpeg", keys, values );

        Exiv2::Image::AutoPtr target = Exiv2::ImageFactory::open(
                                         (Exiv2::byte *)buf, size );
        target->setMetadata( *image_ );
        target->writeMetadata();

        Exiv2::FileIo file( "/tmp/test.jpg" );
        file.open("w");
        file.write( target->io() );
        file.close();

        Glib::RefPtr<Gio::File> src = Gio::File::create_for_path(
                                            "/tmp/test.jpg");
        Glib::RefPtr<Gio::File> dest = Gio::File::create_for_path(
                                        photo_->get_disk_file_path());
        src->copy( dest, Gio::FILE_COPY_OVERWRITE );
        engine.save( photo_ );
    }
}

void
EditablePhoto::setup_photo_for_edit( ) throw()
{
    if( !photo_ )
        return;

    if( photo_->get_disk_file_path().empty() )
        return;

    image_ = Exiv2::ImageFactory::open(
                            photo_->get_disk_file_path() );

    image_->readMetadata();

#if 0

    gchar *buf;

    image_->io()->read( (Exiv2::byte *)buf, image_->io()->size());

    Glib::Ref<Gdk::PixbufLoader> loader = Gdk::PixbufLoader::create();

    loader->write( buf, image_->io()->size() );

    buffer_ = loader->get_pixbuf();
#endif

    buffer_ = photo_->get_buffer();
    if( ! buffer_ )
    {
        buffer_ = Gdk::Pixbuf::create_from_file(
                            photo_->get_disk_file_path());
    }
}

void
EditablePhoto::set_to_save( bool value ) throw()
{
    toSave_ = value;
}

void
EditablePhoto::apply_action( const EditActionPtr &action ) throw(Error)
{
    try
    {
        action->execute( *this );
    }
    catch( Error & e )
    {
        e.add_call_info( __FUNCTION__, __FILE__, __LINE__ );
        throw;
    }
    appliedActions_.insert( action );
}

void
EditablePhoto::undo_last_action( ) throw(Error)
{
    if( !appliedActions_.is_undo_possible() )
        return;

    EditActionList actions = appliedActions_.undo_actions( 1 );

    EditActionPtr action = *actions.begin();
    try
    {
        action->reverse( *this );
    }
    catch( Error & e )
    {
        e.add_call_info( __FUNCTION__, __FILE__, __LINE__ );
        throw;
    }
    if( actions.empty() )
        photo_->set_has_unsaved_data( false );
}

void
EditablePhoto::redo_last_action( ) throw(Error)
{
    if( !appliedActions_.is_redo_possible() )
        return;

    EditActionList actions = appliedActions_.redo_actions( 1 );

    EditActionPtr action = *actions.begin();
    try
    {
        action->execute( *this );
    }
    catch( Error & e )
    {
        e.add_call_info( __FUNCTION__, __FILE__, __LINE__ );
        throw;
    }
}

} //namespace Solang
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_PHOTO_TAG_H
#define SOLANG_PHOTO_TAG_H

#include <exception>

#include <glibmm.h>

#include "db-object.h"
#include "types.h"

namespace Solang
{

//A photo - to - tag association

class PhotoTag :
    public DBObject
{
    public:
        PhotoTag(const PhotoPtr & photo, const TagPtr & tag) throw();

        ~PhotoTag() throw();

        virtual void
        delete_async(Database & database, const SlotAsyncReady & slot)
                     const throw();

        virtual Glib::ustring
        get_delete_query() const throw();

        virtual Glib::ustring
        get_save_query() const throw();

        virtual void
        save_async(Database & database, const SlotAsyncReady & slot)
                   const throw();

        virtual DeleteActionPtr
        get_delete_action() throw();

    private:
        PhotoPtr photo_;

        TagPtr tag_;
};

} // namespace Solang

#endif // SOLANG_PHOTO_TAG_H

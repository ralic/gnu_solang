/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * db-table-factory.h
 * Copyright (C) Santanu Sinha 2009 <santanu.sinha@gmail.com>
 * 
 * db-table-factory.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * d-b-table-factory.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLANG_DB_TABLE_FACTORY_H
#define SOLANG_DB_TABLE_FACTORY_H

#include "types.h"

namespace Solang
{

class DBTableFactory
{
    public:
        static DBTablePtr
        create(const Glib::ustring & table_type_name);
};

} //namespace Solang

#endif // SOLANG_DB_TABLE_FACTORY_H

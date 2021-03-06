/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010 Debarshi Ray <rishi@gnu.org>
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

#include <glibmm/i18n.h>

#include "rotate-clock-operation.h"

namespace Solang
{

RotateClockOperation::RotateClockOperation() throw() :
    RotateOperation()
{
    set_degrees(-90.0);
}

RotateClockOperation::~RotateClockOperation() throw()
{
}

Glib::ustring
RotateClockOperation::get_description() const throw()
{
    return Glib::ustring(_("Rotating clockwise..."));
}

Gtk::Widget *
RotateClockOperation::get_widget() throw()
{
    Glib::signal_idle().connect_once(
        sigc::mem_fun(signal_ready(),
                      &SignalReady::emit));
    return 0;
}

} // namespace Solang

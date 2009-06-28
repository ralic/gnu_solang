/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_SPINNER_DIALOG_H
#define SOLANG_SPINNER_DIALOG_H

#include <glibmm.h>
#include <gtkmm.h>

#include "spinner.h"

namespace Solang
{

class SpinnerDialog :
    public Gtk::Dialog
{
    public:
        SpinnerDialog() throw();

        SpinnerDialog(const Glib::ustring & title,
                      const Glib::ustring & text) throw();

        virtual
        ~SpinnerDialog() throw();

        void
        set_busy(bool busy) throw();

        void
        set_text(const Glib::ustring & text) throw();

    protected:
        virtual bool
        on_delete_event(GdkEventAny * event);

        virtual void
        on_response(int response_id);

        Gtk::HBox hBox_;

        Spinner spinner_;

        Gtk::Label primaryLabel_;

    private:
        void
        configure(const Glib::ustring & title,
                  const Glib::ustring & text) throw();
};

} // namespace Solang

#endif // SOLANG_SPINNER_DIALOG_H

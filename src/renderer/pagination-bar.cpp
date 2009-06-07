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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>

#include <glibmm/i18n.h>
#include <sigc++/sigc++.h>

#include "pagination-bar.h"

namespace Solang
{

PaginationBar::PaginationBar() throw() :
    Gtk::HBox(false, 12),
    firstHBox_(false, 12),
    previousButton_(),
    previousImage_(Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_LARGE_TOOLBAR),
    shownItemsLabel_(),
    nextButton_(),
    nextImage_(Gtk::Stock::GO_FORWARD, Gtk::ICON_SIZE_LARGE_TOOLBAR),
    secondHBox_(false, 12),
    pageSizeLabel_(_("Photos per page")),
    pageSizeSpinButton_(1.0, 0),
    lowerLimit_(0),
    upperLimit_(0),
    total_(0)
{
    pageSizeSpinButton_.set_increments(1.0, 10.0);
    pageSizeSpinButton_.set_numeric(true);
    pageSizeSpinButton_.set_range(1.0, 500.0);
    pageSizeSpinButton_.set_snap_to_ticks(false);
    pageSizeSpinButton_.set_value(50.0);
    pageSizeSpinButton_.set_wrap(false);
    upperLimit_ = static_cast<guint>(
                      pageSizeSpinButton_.get_value_as_int());

    previousButton_.set_image(previousImage_);
    previousButton_.set_sensitive(false);
    firstHBox_.pack_start(previousButton_, Gtk::PACK_SHRINK, 0);

    std::ostringstream sout;
    sout << lowerLimit_ << " - " << upperLimit_ << " of " << total_;
    shownItemsLabel_.set_label(sout.str());
    firstHBox_.pack_start(shownItemsLabel_, Gtk::PACK_SHRINK, 0);

    nextButton_.set_image(nextImage_);
    firstHBox_.pack_start(nextButton_, Gtk::PACK_SHRINK, 0);

    pack_start(firstHBox_, Gtk::PACK_SHRINK, 0);

    secondHBox_.pack_start(pageSizeLabel_, Gtk::PACK_SHRINK, 0);
    secondHBox_.pack_start(pageSizeSpinButton_, Gtk::PACK_SHRINK, 0);

    pack_start(secondHBox_, Gtk::PACK_SHRINK, 0);

    limitsChanged_.connect(
        sigc::mem_fun(*this,
                      &PaginationBar::on_limits_changed));

    pageSizeSpinButton_.signal_activate().connect(
        sigc::mem_fun(*this,
                      &PaginationBar::on_spin_button_activate));

    previousButton_.signal_clicked().connect(
        sigc::mem_fun(*this,
                      &PaginationBar::on_action_go_previous));

    nextButton_.signal_clicked().connect(
        sigc::mem_fun(*this,
                      &PaginationBar::on_action_go_next));

    show_all_children();
}

PaginationBar::~PaginationBar() throw()
{
}

guint
PaginationBar::get_step() const throw()
{
    const guint step = static_cast<guint>(
                           pageSizeSpinButton_.get_value_as_int());
    return step;
}

guint
PaginationBar::get_lower_limit() const throw()
{
    return lowerLimit_;
}

guint
PaginationBar::get_upper_limit() const throw()
{
    return upperLimit_;
}

sigc::signal<void> &
PaginationBar::limits_changed() throw()
{
    return limitsChanged_;
}

void
PaginationBar::on_action_go_previous() throw()
{
    const guint step = get_step();

    if (step > lowerLimit_)
    {
        lowerLimit_ = 0;
        upperLimit_ = step;
    }
    else
    {
        lowerLimit_ -= step;
        upperLimit_ = lowerLimit_ + step;
    }

    limitsChanged_.emit();
}

void
PaginationBar::on_action_go_next() throw()
{
    const guint step = get_step();

    upperLimit_ += step;
    if (total_ < upperLimit_)
    {
        upperLimit_ = total_;
    }

    if (step > upperLimit_)
    {
        lowerLimit_ = 0;
    }
    else
    {
        lowerLimit_ = upperLimit_ - step;
    }

    limitsChanged_.emit();
}

void
PaginationBar::on_limits_changed() throw()
{
    previousButton_.set_sensitive((0 < lowerLimit_) ? true : false);
    nextButton_.set_sensitive((total_ > upperLimit_) ? true : false);

    std::ostringstream sout;
    sout << lowerLimit_ << " - " << upperLimit_ << " of " << total_;
    shownItemsLabel_.set_label(sout.str());
}

void
PaginationBar::on_spin_button_activate() throw()
{
    const guint step = get_step();

    upperLimit_ = lowerLimit_ + step;
    if (total_ < upperLimit_)
    {
        upperLimit_ = total_;
    }

    limitsChanged_.emit();
}

void
PaginationBar::scroll_to_position(guint position) throw()
{
    if (position >= total_)
    {
        return;
    }
    else if (position >= lowerLimit_ && position < upperLimit_)
    {
        return;
    }

    const guint step = get_step();

    if (position < lowerLimit_)
    {
        const double delta = lowerLimit_ - position;
        const guint pages = static_cast<guint>(
                                ceil(delta / static_cast<double>(step)));

        if (pages * step > lowerLimit_)
        {
            lowerLimit_ = 0;
            upperLimit_ = step;
        }
        else
        {
            lowerLimit_ -= pages * step;
            upperLimit_ = lowerLimit_ + step;
        }
    }
    else if (position >= upperLimit_)
    {
        const double delta = position - upperLimit_ + 1;
        const guint pages = static_cast<guint>(
                                ceil(delta / static_cast<double>(step)));

        upperLimit_ += pages * step;
        if (total_ < upperLimit_)
        {
            upperLimit_ = total_;
        }

        if (step > upperLimit_)
        {
            lowerLimit_ = 0;
        }
        else
        {
            lowerLimit_ = upperLimit_ - step;
        }
    }

    limitsChanged_.emit();
}

void
PaginationBar::set_total(guint total) throw()
{
    total_ = total;

    lowerLimit_ = 0;
    upperLimit_ = get_step();
    if (total_ < upperLimit_)
    {
        upperLimit_ = total_;
    }

    limitsChanged_.emit();
}

} // namespace Solang
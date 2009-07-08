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

#include <algorithm>
#include <glibmm/i18n.h>

#include "application.h"
#include "edit-action.h"
#include "editable-photo.h"
#include "editor.h"
#include "flip.h"
#include "main-window.h"
#include "renderer.h"
#include "rotate.h"
#include "save-photos-window.h"

namespace Solang
{

static const std::string uiFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/ui/"
          PACKAGE_TARNAME"-editor.ui";

static const std::string flipHorzImageFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/pixmaps/flip-object-horizontal.png";

static const std::string flipVertImageFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/pixmaps/flip-object-vertical.png";

static const std::string rotateLeftFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/pixmaps/object-rotate-left.png";

static const std::string rotateRightFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/pixmaps/object-rotate-right.png";

static const std::string scaleFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/pixmaps/stock-resize-16.png";

Editor::Editor( )
    :application_( NULL ),
    modifiedPhotos_(),
    mutex_(),
    actionGroup_( Gtk::ActionGroup::create() ),
    iconFactory_( Gtk::IconFactory::create()),
    uiID_( 0 ),
    actionPerformed_(),
    currentPhoto_()
{
    Gtk::IconSource icon_source;

    Gtk::IconSet icon_set_flip_horz;
    icon_source.set_filename( flipHorzImageFile );
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_flip_horz.add_source(icon_source);
    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-flip-horz"),
                    icon_set_flip_horz);

    Gtk::IconSet icon_set_flip_vert;
    icon_source.set_filename( flipVertImageFile );
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_flip_vert.add_source(icon_source);
    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-flip-vert"),
                    icon_set_flip_vert);

    Gtk::IconSet icon_set_rotate_left;
    icon_source.set_filename( rotateLeftFile );
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_rotate_left.add_source(icon_source);
    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-rotate-left"),
                    icon_set_rotate_left);

    Gtk::IconSet icon_set_rotate_right;
    icon_source.set_filename( rotateRightFile );
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_rotate_right.add_source(icon_source);
    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-rotate-right"),
                    icon_set_rotate_right);

    Gtk::IconSet icon_set_scale;
    icon_source.set_filename( scaleFile );
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_scale.add_source(icon_source);
    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-scale"),
                    icon_set_scale);

    iconFactory_->add_default();

    actionGroup_->add(
        Gtk::Action::create(
            "ActionEditMenu", _("_Edit")));

     actionGroup_->add(
         Gtk::Action::create(
            "ActionEditUndo",
            Gtk::Stock::UNDO,
            _("Undo"),
            _("Undo last action")),
            Gtk::AccelKey("<control>z"),
            sigc::mem_fun(*this, &Editor::on_action_undo));

     actionGroup_->add(
         Gtk::Action::create(
            "ActionEditRedo",
            Gtk::Stock::REDO,
            _("Redo"),
            _("Redo last action")),
            Gtk::AccelKey("<control>y"),
            sigc::mem_fun(*this, &Editor::on_action_redo));

    actionGroup_->add(
         Gtk::Action::create(
            "ActionEditFlipHorizontal",
            Gtk::StockID(PACKAGE_TARNAME"-flip-horz"),
            _("Flip _Horizontally"),
            _("Flip the selected image(s) horizontally")),
            Gtk::AccelKey(""),
            sigc::mem_fun(*this, &Editor::on_action_flip_horz));

     actionGroup_->add(
         Gtk::Action::create(
            "ActionEditFlipVertical",
            Gtk::StockID(PACKAGE_TARNAME"-flip-vert"),
            _("Flip _Vertically"),
            _("Flip the selected image(s) vertically")),
            Gtk::AccelKey(""),
            sigc::mem_fun(*this, &Editor::on_action_flip_vert));

     actionGroup_->add(
         Gtk::Action::create(
            "ActionEditRotateClockwise",
            Gtk::StockID(PACKAGE_TARNAME"-rotate-right"),
            _("Rotate _Clockwise"),
            _("Rotate the selected image(s) clockwise")),
            Gtk::AccelKey(""),
            sigc::mem_fun(*this, &Editor::on_action_rotate_right));

     actionGroup_->add(
         Gtk::Action::create(
            "ActionEditRotateCounterClockwise",
            Gtk::StockID(PACKAGE_TARNAME"-rotate-left"),
            _("Rotate C_ounter Clockwise"),
            _("Rotate the selected image(s) counter-clockwise")),
            Gtk::AccelKey(""),
            sigc::mem_fun(*this, &Editor::on_action_rotate_left));

     actionGroup_->add(
         Gtk::Action::create(
             "ActionEditScale",
            Gtk::StockID(PACKAGE_TARNAME"-scale"),
            _("Sca_le"),
            _("Scale/resize the selected image(s)")),
            Gtk::AccelKey(""),
            sigc::mem_fun(*this, &Editor::on_action_scale));

     actionGroup_->add(
         Gtk::Action::create(
             "ActionEditSaveAs", Gtk::Stock::SAVE,
            _("_Save")),
            Gtk::AccelKey(""),
            sigc::mem_fun(*this, &Editor::on_action_save));

}

Editor::~Editor() throw()
{
    iconFactory_->remove_default();
}

void
Editor::apply( const EditActionPtr &action,
                const EditablePhotoPtr &photo) throw(Error)
{
    if( !currentPhoto_ )
        return;

    {
        currentPhoto_->apply_action( action );

        Glib::Mutex::Lock lock( mutex_ );
        if( !photo->get_photo()->get_has_unsaved_data() )
        {
            EditablePhotoList::iterator ptr = std::find_if(
                    modifiedPhotos_.begin(), modifiedPhotos_.end(),
                    EditablePhoto::Searcher( photo->get_photo() ) );
            if( ptr == modifiedPhotos_.end() )
                modifiedPhotos_.push_back( currentPhoto_ );
            currentPhoto_->get_photo()->set_has_unsaved_data( true );
        }
    }
    actionPerformed_.emit();
}

void
Editor::apply( const EditActionPtr &action)
{
    apply( action, currentPhoto_ );
}

void
Editor::apply( const EditActionPtr &action,
               const EditablePhotoList &photos) throw(Error)
{
    for( EditablePhotoList::const_iterator photo = photos.begin();
                            photo != photos.end(); photo++ )
    {
        apply( action, *photo );
    }
}

void
Editor::init( Application &app )
{
    application_ = &app;
}

void
Editor::register_ui() throw()
{
    MainWindow & main_window = application_->get_main_window();

    const Glib::RefPtr<Gtk::UIManager> & ui_manager
        = main_window.get_ui_manager();

    uiID_ = ui_manager->add_ui_from_file(uiFile);
    if (0 == uiID_)
    {
        // FIXME: error condition.
    }

    ui_manager->insert_action_group(actionGroup_);

    return;
}

void
Editor::unregister_ui() throw()
{
    if( 0 == uiID_ )
        return;

    MainWindow & main_window = application_->get_main_window();
    const Glib::RefPtr<Gtk::UIManager> & ui_manager
        = main_window.get_ui_manager();
    ui_manager->remove_action_group(actionGroup_);
    ui_manager->remove_ui(uiID_);
    uiID_ = 0;
}

void
Editor::final( Application &app )
{
    unregister_ui();
}

void
Editor::save()
{
    if( modifiedPhotos_.empty() )
    {
        return;
    }
    bool hasUnsavedData = false;
    for( EditablePhotoList::iterator photo = modifiedPhotos_.begin();
                    photo != modifiedPhotos_.end(); photo++ )
    {
        hasUnsavedData = hasUnsavedData
                    || (*photo)->get_photo()->get_has_unsaved_data();
    }
    if( !hasUnsavedData )
        return;
    SavePhotosWindow saveWindow(
                        application_->get_engine(), modifiedPhotos_ );
    const gint response = saveWindow.run();
    if( Gtk::RESPONSE_OK == response )
    {
        for( EditablePhotoList::iterator photo = modifiedPhotos_.begin();
                    photo != modifiedPhotos_.end(); photo++ )
        {
            if( (*photo)->get_photo()->get_has_unsaved_data()
                && (*photo)->get_to_save() )
            {
                (*photo)->save( application_->get_engine() );
            }
        }
    }

    modifiedPhotos_.clear();

    return;
}

void
Editor::set_current_photo( const EditablePhotoPtr &photo )
{
    currentPhoto_ = photo;
}

void
Editor::on_action_flip_horz() throw()
{
    EditActionPtr action( new Flip( true ) );
    apply( action );
}

void
Editor::on_action_flip_vert() throw()
{
    EditActionPtr action( new Flip( false ) );
    apply( action );
}

void
Editor::on_action_rotate_left() throw()
{
    EditActionPtr action(
        new Rotate( Rotate::ROTATE_COUNTERCLOCKWISE ) );
    apply( action );
}

void
Editor::on_action_rotate_right() throw()
{
    EditActionPtr action(
        new Rotate( Rotate::ROTATE_CLOCKWISE ) );
    apply( action );
}

void
Editor::on_action_scale() throw()
{
}

void
Editor::on_action_save() throw()
{
    save();
}

void
Editor::on_action_undo() throw()
{
    if( !currentPhoto_ )
    {
        return;
    }
    currentPhoto_->undo_last_action( );
    actionPerformed_.emit();
}

void
Editor::on_action_redo() throw()
{
    if( !currentPhoto_ )
    {
        return;
    }
    currentPhoto_->redo_last_action( );
    actionPerformed_.emit();
}

} // namespace Solang

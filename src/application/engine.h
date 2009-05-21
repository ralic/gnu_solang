/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) Debarshi Ray 2009 <rishi@gnu.org>
 * Copyright (C) Santanu Sinha 2009 <santanu.sinha@gmail.com>
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

#ifndef SOLANG_ENGINE_H
#define SOLANG_ENGINE_H

#include <map>

#include <glibmm.h>
#include <gtkmm.h>

#include "database.h"
#include "i-storage.h"
#include "non-copyable.h"
#include "photo.h"
#include "photo-search-criteria.h"
#include "search-criterion-repo.h"
#include "types.h"

namespace Solang
{

class Engine :
    public NonCopyable
{
    public:
        typedef std::map<Glib::ustring, IStoragePtr> StorageMap;

    public:
        Engine(int & argc, char ** & argv,
               const ProgressObserverPtr & observer
                   = ProgressObserverPtr()) throw();

        ~Engine() throw();

        void
        init(Glib::ustring);

        void
        import(const PhotoPtr & photo,
               const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags) throw();

        void
        import(const PhotoPtr & photo,
               const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags,
               const ProgressObserverPtr & observer) throw();

        void
        import(const PhotoList & photos,
               const IPhotoSourcePtr & source, 
               const IStoragePtr & selected_storage,
               const TagList & tags) throw();

        void
        import(const PhotoList & photos,
               const IPhotoSourcePtr & source, 
               const IStoragePtr & selected_storage,
               const TagList & tags,
               const ProgressObserverPtr & observer) throw();

        void
        import(const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags) throw();

        void
        import(const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags,
               const ProgressObserverPtr & observer) throw();

        PhotoList
        search(const PhotoSearchCriteriaList & criterion,
               const ProgressObserverPtr & observer
                   = ProgressObserverPtr()) throw();

        void
        on_criterion_changed();

        void
        show(const PhotoSearchCriteriaList & criterion) throw();

        void
        show(const PhotoSearchCriteriaList & criterion,
             const ProgressObserverPtr & observer) throw();

        void
        export_photos(const PhotoList & photos, 
                      const IPhotoDestinationPtr & destination,
                      const ProgressObserverPtr & observer 
                          = ProgressObserverPtr());

        void
        erase(const PhotoList & photos,
              const ProgressObserverPtr & observer 
                  = ProgressObserverPtr());

        TagList
        get_tags() const throw();

        void
        apply_tag_to_photos( PhotoList &photos, const TagPtr &tag );

        DatePhotoInfoList
        get_dates_with_picture_count(
                    gint year = -1, gint month = -1, gint day = -1);
    
        Glib::Dispatcher &
        photo_import_begin() throw();

        Glib::Dispatcher &
        photo_import_end() throw();

        Glib::Dispatcher &
        photo_render_begin() throw();

        Glib::Dispatcher &
        photo_render_end() throw();

        Glib::Dispatcher &
        tag_add_begin() throw();

        Glib::Dispatcher &
        tag_add_end() throw();

        Glib::Dispatcher &
        criterion_changed() throw();

        sigc::signal<void, const Gtk::TreeIter &> &
        item_activated() throw();

        sigc::signal<void> &
        selection_changed() throw();

        inline DatabasePtr
        get_db()
        {
            return DatabasePtr(&database_);
        }

        void
        set_current_storage_systems(const StorageMap & storages);

        void
        add_current_storage_system(const Glib::ustring & prefix,
                                   const IStoragePtr & storage);

        IStoragePtr
        get_current_storage_system(const Glib::ustring & prefix)
                                   const throw();

        RendererPtr
        get_current_renderer() throw();

        void
        set_current_renderer(const RendererPtr & renderer);

        PhotoList
        get_photos() throw();

        inline const ProgressObserverPtr &
        get_default_observer();

        inline SearchCriterionRepo &
        get_criterion_repo();

    private:
        PhotoList
        create_renderable_list_from_photos(
            const PhotoList & photos,
            const ProgressObserverPtr & observer
                = ProgressObserverPtr());

        ProgressObserverPtr observer_;

        Glib::Dispatcher photoImportBegin_;

        Glib::Dispatcher photoImportEnd_;

        Glib::Dispatcher photoRenderBegin_;

        Glib::Dispatcher photoRenderEnd_;
        
        Glib::Dispatcher tagAddBegin_;

        Glib::Dispatcher tagAddEnd_;

        Glib::Dispatcher criterionChanged_;

        sigc::signal<void, const Gtk::TreeIter &> itemActivated_;

        sigc::signal<void> selectionChanged_;

        Glib::Mutex mutex_;

        PhotoList photos_;

        //The following are populated 
        //from gconf

        StorageMap currentStorageSystems_;

        RendererPtr currentRenderer_;

        Database database_;

        SearchCriterionRepo criterionRepo_;
};

inline const ProgressObserverPtr &
Engine::get_default_observer()
{
    return observer_;
}

inline SearchCriterionRepo &
Engine::get_criterion_repo()
{
    return criterionRepo_;
}

} // namespace Solang

#endif // SOLANG_ENGINE_H

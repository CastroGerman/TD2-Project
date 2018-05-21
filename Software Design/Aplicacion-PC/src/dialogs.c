/* MIT License
 *
 * Copyright (c) 2017 spino.tech Guillaume Chereau & Raphael Seghier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "spino.h"

#if defined __linux__

// GTK implementation.
#include <gtk/gtk.h>

bool dialog_open(int flags, const char *filters, char **out)
{
    GtkWidget *dialog;
    GtkFileFilter *filter;
    GtkFileChooser *chooser;
    GtkFileChooserAction action;
    gint res;
    const char *extension;
    char default_name[128];

    action = flags & DIALOG_FLAG_SAVE ? GTK_FILE_CHOOSER_ACTION_SAVE :
                                        GTK_FILE_CHOOSER_ACTION_OPEN;
    if (flags & DIALOG_FLAG_DIR)
        action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

    gtk_init_check(NULL, NULL);
    dialog = gtk_file_chooser_dialog_new(
            flags & DIALOG_FLAG_SAVE ? "Save File" : "Open File",
            NULL,
            action,
            "_Cancel", GTK_RESPONSE_CANCEL,
            flags & DIALOG_FLAG_SAVE ? "_Save" : "_Open",
            GTK_RESPONSE_ACCEPT,
            NULL );
    chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    if ((flags & DIALOG_FLAG_SAVE) && filters) {
        extension = filters + strlen(filters) + 3;
        sprintf(default_name, "untitled.%s", extension);
        gtk_file_chooser_set_current_name(chooser, default_name);
    }

    while (filters && *filters) {
        filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, filters);
        filters += strlen(filters) + 1;
        gtk_file_filter_add_pattern(filter, filters);
        gtk_file_chooser_add_filter(chooser, filter);
        filters += strlen(filters) + 1;
    }
    // Add a default 'any' file pattern.
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "*");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(chooser, filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        *out = gtk_file_chooser_get_filename(chooser);
    }
    gtk_widget_destroy(dialog);
    while (gtk_events_pending()) gtk_main_iteration();
    return res == GTK_RESPONSE_ACCEPT;
}

#endif

#ifdef WIN32

#include "Commdlg.h"
#include "Shlobj.h"


bool dialog_open(int flags, const char *filters, char **out)
{
    OPENFILENAME ofn;       // common dialog box structure
    BROWSEINFO   bif;       // only used to open directory
    LPITEMIDLIST lpItem;    // only for open directory
    char szFile[260];       // buffer for file name
    const char *extension;
    int ret;

    if (!(flags & DIALOG_FLAG_DIR)) {
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filters;
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (filters) { // Set the default file name.
            extension = filters + strlen(filters) + 3;
            sprintf(ofn.lpstrFile, "untitled.%s", extension);
        }
        if (flags & DIALOG_FLAG_OPEN)
            ret = GetOpenFileName(&ofn);
        else
            ret = GetSaveFileName(&ofn);
        if (ret == TRUE) {
            *out = strdup(szFile);
            return true;
        } else {
            return false;
        }
    } else { // Open a directory.
        ZeroMemory(&bif, sizeof(bif));
        bif.pszDisplayName = szFile;

        lpItem = SHBrowseForFolder(&bif);
        if (lpItem) {
            SHGetPathFromIDList(lpItem, szFile);
            *out = strdup(szFile);
            return true;
        } else {
            return false;
        }
    }
}

#endif

/*
 * gtv - a Gtk+ based SMPEG player
 * (c) Michael Vance, 1999
 *     briareos@lokigames.com
 *
 * TODO
 * - SMPEG returns an incorrect current_frame after a
 * call to SMPEG_renderFinal().
 * - SMPEG loops audio incorrectly when audio stream is
 * shorter than a video stream, etc.
 */

#include "gtv.h"
#include "SDL_syswm.h"

#define TIMER_TIMEOUT 100

static GtkWidget* create_gtv_window( void );
static void gtv_connect( gpointer, gchar*, gchar*, GtkSignalFunc );
static void gtv_set_frame( gpointer, int );
static void gtv_set_fps( gpointer, float );
static void gtv_set_trackbar( gpointer, int );
static void gtv_set_sensitive( gpointer, gchar*, gboolean );
static void gtv_clear_screen( gpointer );
static void gtv_fix_toggle_state( gpointer );

static void gtv_double( GtkWidget*, gpointer );
static void gtv_loop( GtkWidget*, gpointer );
static void gtv_audio( GtkWidget*, gpointer );
static void gtv_filter( GtkWidget*, gpointer );

static void gtv_open( GtkWidget*, gpointer );
static void gtv_open_file( gchar const*, gpointer );
static void gtv_close( GtkWidget*, gpointer );
static void gtv_info( GtkWidget*, gpointer );
static void gtv_quit( GtkWidget*, gpointer );
static void gtv_about( GtkWidget*, gpointer );

static void gtv_play( GtkWidget*, gpointer );
static void gtv_pause( GtkWidget*, gpointer );
static void gtv_stop( GtkWidget*, gpointer );
static void gtv_step( GtkWidget*, gpointer );
static void gtv_to_end( GtkWidget*, gpointer );
static void gtv_seek( GtkAdjustment*, gpointer );
static void gtv_drag_data_received(GtkWidget * widget,
                                   GdkDragContext * context,
                                   gint x, gint y,
                                   GtkSelectionData * selection_data,
                                   guint info, guint time);


static int gtv_trackbar_dragging;
static gchar * gtv_default_directory = 0;

static void gtv_fix_toggle_state( gpointer raw )
{
    SMPEG* mpeg = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    /* If no mpeg is loaded, pop all the buttons. */
    if( !mpeg ) {
	GtkWidget* twotimes = NULL;
	GtkWidget* loop = NULL;
	GtkWidget* audio = NULL;
	GtkWidget* filter = NULL;

	twotimes = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "twotimes" ) );
	loop = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "loop" ) );
	audio = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "audio" ) );
	filter = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "filter" ) );

#if 1 /* Sam 5/31/2000 - Default to doubled video and audio on */
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( twotimes ), FALSE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( loop ), FALSE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( audio ), TRUE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( filter ), FALSE );
#else
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( twotimes ), FALSE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( loop ), FALSE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( audio ), FALSE );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( filter ), FALSE );
#endif
    }

}

static void gtv_set_sensitive( gpointer raw, gchar* name, gboolean sensitive )
{
    GtkWidget* item = NULL;

    assert( raw );
    assert( name );

    item = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), name ) );
    assert( item );
    gtk_widget_set_sensitive( item, sensitive );
}

static void gtv_set_buttons_sensitive( gpointer raw, gboolean sensitive )
{
    SMPEG_Info* info = NULL;

    /* HACK If no MPEG is loaded, info has been memset()'ed to 0. */
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    gtv_set_sensitive( raw, "play", sensitive );
    gtv_set_sensitive( raw, "pause", sensitive );
    gtv_set_sensitive( raw, "stop", sensitive );
    gtv_set_sensitive( raw, "step", sensitive && info->has_video );
    gtv_set_sensitive( raw, "to_end", sensitive && info->has_video );
    gtv_set_sensitive( raw, "loop", sensitive );
    gtv_set_sensitive( raw, "close", sensitive );
    gtv_set_sensitive( raw, "file_info", sensitive );

    gtv_set_sensitive( raw, "twotimes", sensitive && info->has_video );
    gtv_set_sensitive( raw, "audio", sensitive && info->has_audio && info->has_video);
    gtv_set_sensitive( raw, "filter", sensitive && info->has_video );

    gtv_fix_toggle_state( raw );
}

static void gtv_dialog_cleanup( gpointer raw )
{
    GtkWidget* dialog = NULL;

    dialog = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "dialog" ) );
    gtk_grab_remove( dialog );
    gtk_widget_destroy( dialog );
    gtk_object_set_data( GTK_OBJECT( raw ), "dialog", NULL );
}

static void gtv_dialog_cancel( GtkWidget* item, gpointer raw )
{
    gtv_dialog_cleanup( raw );
}

static void gtv_open_ok( GtkWidget* item, gpointer raw )
{
    GtkWidget* file_sel = NULL;
    gchar const* filename = NULL;

    /* HACK HACK HACK */
    file_sel = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "dialog" ) );
    filename = gtk_file_selection_get_filename( GTK_FILE_SELECTION( file_sel ) );

    if( filename ) {
	gtv_open_file( filename, raw );
	if( strrchr( filename, '/') )
	{
	  if( gtv_default_directory ) free( gtv_default_directory );
	  gtv_default_directory = (gchar *) malloc( (strlen(filename) + 1) * sizeof(gchar) );
	  strcpy( gtv_default_directory, filename);
	  *(strrchr( gtv_default_directory, '/' ) + 1) = 0;
	}
    }

    gtv_dialog_cleanup( raw );
}

static void gtv_open( GtkWidget* item, gpointer raw )
{
    GtkWidget* file_sel = NULL;

    file_sel = gtk_file_selection_new( "Select an MPEG" );
    gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( file_sel )->ok_button ), "clicked",
			GTK_SIGNAL_FUNC( gtv_open_ok ), raw );
    gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( file_sel )->cancel_button ), "clicked",
			GTK_SIGNAL_FUNC( gtv_dialog_cancel ), raw );
    if( gtv_default_directory )
      gtk_file_selection_complete( GTK_FILE_SELECTION( file_sel ), gtv_default_directory );
    gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( file_sel ) );

    /* HACK HACK HACK */
    gtk_object_set_data( GTK_OBJECT( raw ), "dialog", file_sel );

    gtk_grab_add( file_sel );
    gtk_widget_show( file_sel );
}

static void gtv_center_window(SDL_Surface *screen)
{
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version);
    if ( SDL_GetWMInfo(&info) > 0 ) {
        int x, y;
        int w, h;
#ifdef unix
        if ( info.subsystem == SDL_SYSWM_X11 ) {
            info.info.x11.lock_func();
            w = DisplayWidth(info.info.x11.display,
                             DefaultScreen(info.info.x11.display));
            h = DisplayHeight(info.info.x11.display,
                             DefaultScreen(info.info.x11.display));
            x = (w - screen->w)/2;
            y = (h - screen->h)/2;
            XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
            info.info.x11.unlock_func();
        }
#else
#ifdef WIN32
	/* FIXME: implement centering code for Windows */
#else
#warning Need to implement these functions for other systems
#endif // WIN32
#endif // unix
    }
}

static void gtv_open_file( gchar const* name, gpointer raw )
{
    SMPEG_Info* info = NULL;
    SMPEG* mpeg = NULL;
    gchar buffer[FILENAME_BUFFER_SIZE];

    assert( name );
    assert( raw );

    gtv_close( NULL, raw );

    if( SDL_Init( SDL_INIT_AUDIO | SDL_INIT_VIDEO ) < 0 ) {
	fprintf( stderr, "gtv: couldn't initialize SDL: %s\n", SDL_GetError( ) );
	exit( 1 );
    }

    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );
    assert( info );

    mpeg = SMPEG_new( name, info, 1 );

    if( SMPEG_error( mpeg ) ) {
	fprintf( stderr, "gtv: %s\n", SMPEG_error( mpeg ) );
	SMPEG_delete( mpeg );
	gtk_object_set_data( GTK_OBJECT( raw ), "mpeg", NULL );
	return;
    }

    gtk_object_set_data( GTK_OBJECT( raw ), "mpeg", mpeg );
    strncpy( (char*) gtk_object_get_data( GTK_OBJECT( raw ), "filename_buffer" ),
	     name,
	     FILENAME_BUFFER_SIZE );
    gtv_set_frame( raw, 0 );
    gtv_set_fps( raw, 0.0 );
    g_snprintf( buffer, FILENAME_BUFFER_SIZE, "gtv - %s", name );
    gtk_window_set_title( GTK_WINDOW( raw ), buffer );

    if( info->has_video ) {
	SDL_Surface* sdl_screen = NULL;
        const SDL_VideoInfo *video_info;
        int video_bpp;

        /* Get the "native" video mode */
        video_info = SDL_GetVideoInfo();
        switch (video_info->vfmt->BitsPerPixel) {
            case 16:
            case 32:
                video_bpp = video_info->vfmt->BitsPerPixel;
                break;
            default:
                video_bpp = 16;
                break;
        }
#ifdef linux
	putenv("SDL_VIDEO_CENTERED=1");
#endif
	sdl_screen = SDL_SetVideoMode( info->width , info->height , video_bpp, SDL_ASYNCBLIT);
        SDL_WM_SetCaption(name, "gtv movie");
        gtv_center_window(sdl_screen);
	SMPEG_setdisplay( mpeg, sdl_screen, NULL, NULL );
	gtk_object_set_data( GTK_OBJECT( raw ), "sdl_screen", sdl_screen );
	gtv_double( NULL, raw );
    }

    gtv_loop( NULL, raw );
    gtv_audio( NULL, raw );
    gtv_filter( NULL, raw );
    gtv_set_buttons_sensitive( raw, TRUE );
}

static void gtv_close( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    assert( raw != NULL );

    gtv_stop( NULL, raw );
    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    if( mpeg ) {
	SDL_Surface* sdl_screen = NULL;

	SMPEG_delete( mpeg );
	gtk_object_set_data( GTK_OBJECT( raw ), "mpeg", NULL );
	info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );
	memset( info, 0, sizeof( SMPEG_Info ) );

	sdl_screen = (SDL_Surface*) gtk_object_get_data( GTK_OBJECT( raw ), "sdl_screen" );
	SDL_FreeSurface( sdl_screen );
	gtk_object_set_data( GTK_OBJECT( raw ), "sdl_screen", NULL );

	SDL_Quit( );
    }

    /* Reset title. */
    gtk_window_set_title( GTK_WINDOW( raw ), "gtv" );
    gtv_set_frame( raw, 0 );
    gtv_set_fps( raw, 0.0 );
    gtv_set_buttons_sensitive( raw, FALSE );
}

static GtkWidget* create_file_info_dialog( void )
{
  GtkWidget *file_info_dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox2;
  GtkWidget *label5;
  GtkWidget *scrolledwindow1;
  GtkWidget *viewport1;
  GtkWidget *text;
  GtkWidget *dialog_action_area1;
  GtkWidget *ok;

  file_info_dialog = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (file_info_dialog), "file_info_dialog", file_info_dialog);
  gtk_window_set_title (GTK_WINDOW (file_info_dialog), "File Info");
  gtk_window_set_default_size (GTK_WINDOW (file_info_dialog), 256, 192);

  dialog_vbox1 = GTK_DIALOG (file_info_dialog)->vbox;
  gtk_object_set_data (GTK_OBJECT (file_info_dialog), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (file_info_dialog), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 5);

  label5 = gtk_label_new ("File info:");
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (file_info_dialog), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (vbox2), label5, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (file_info_dialog), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport1);
  gtk_object_set_data_full (GTK_OBJECT (file_info_dialog), "viewport1", viewport1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), viewport1);

  text = gtk_text_view_new ();
  gtk_widget_ref (text);
  gtk_object_set_data_full (GTK_OBJECT (file_info_dialog), "text", text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text);
  gtk_container_add (GTK_CONTAINER (viewport1), text);

  dialog_action_area1 = GTK_DIALOG (file_info_dialog)->action_area;
  gtk_object_set_data (GTK_OBJECT (file_info_dialog), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  ok = gtk_button_new_with_label ("  OK  ");
  gtk_widget_ref (ok);
  gtk_object_set_data_full (GTK_OBJECT (file_info_dialog), "ok", ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), ok, FALSE, FALSE, 0);

  return file_info_dialog;
}

static void gtv_info( GtkWidget* item, gpointer raw )
{
    GtkWidget* dialog = NULL;
    GtkWidget* ok = NULL;
    gchar buffer[1024];
    gint ignored = 0;
    GtkWidget* text = NULL;
    SMPEG_Info* info = NULL;
    int hh,mm,ss;

    dialog = create_file_info_dialog( );
    ok = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( dialog ), "ok" ) );
    gtk_signal_connect( GTK_OBJECT( ok ), "clicked",
			GTK_SIGNAL_FUNC( gtv_dialog_cancel ), raw );
    /* HACK HACK HACK */
    gtk_object_set_data( GTK_OBJECT( raw ), "dialog", dialog );

    /* Actually stuff some data in there. */
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );
    hh = info->total_time / 3600;
    mm = (info->total_time - hh * 3600)/60;
    ss = ((int)info->total_time % 60);
    g_snprintf( buffer, 1024, "Filename: %s\nStream: %s\nVideo: %dx%d resolution\nAudio: %s\nSize: %d\nTime %d:%02d:%02d\n",
		(gchar*) gtk_object_get_data( GTK_OBJECT( raw ), "filename_buffer" ),
		( info->has_audio && info->has_video ) ? "system" :
		( info->has_video ? "video" :
		( info->has_audio ? "audio" : "not MPEG" ) ),
		info->width, info->height,
		( info->has_audio ? info->audio_string : "none" ),
		info->total_size , hh,mm,ss);
    text = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( dialog ), "text" ) );
    gtk_editable_insert_text( GTK_EDITABLE( text ), buffer, strlen( buffer ), &ignored );

    gtk_grab_add( dialog );
    gtk_widget_show( dialog );
}

static GtkWidget* create_about_dialog( void )
{
  GtkWidget *about_dialog;
  GtkWidget *dialog_vbox2;
  GtkWidget *scrolledwindow2;
  GtkWidget *viewport2;
  GtkWidget *text;
  GtkWidget *dialog_action_area2;
  GtkWidget *ok;

  about_dialog = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (about_dialog), "about_dialog", about_dialog);
  gtk_window_set_title (GTK_WINDOW (about_dialog), "About gtv");
  gtk_window_set_default_size (GTK_WINDOW (about_dialog), 384, 256);
  gtk_window_set_policy (GTK_WINDOW (about_dialog), TRUE, TRUE, FALSE);

  dialog_vbox2 = GTK_DIALOG (about_dialog)->vbox;
  gtk_object_set_data (GTK_OBJECT (about_dialog), "dialog_vbox2", dialog_vbox2);
  gtk_widget_show (dialog_vbox2);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (about_dialog), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), scrolledwindow2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow2), 5);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport2 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport2);
  gtk_object_set_data_full (GTK_OBJECT (about_dialog), "viewport2", viewport2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), viewport2);

  text = gtk_text_view_new ();
  gtk_widget_ref (text);
  gtk_object_set_data_full (GTK_OBJECT (about_dialog), "text", text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text);
  gtk_container_add (GTK_CONTAINER (viewport2), text);

  dialog_action_area2 = GTK_DIALOG (about_dialog)->action_area;
  gtk_object_set_data (GTK_OBJECT (about_dialog), "dialog_action_area2", dialog_action_area2);
  gtk_widget_show (dialog_action_area2);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area2), 10);

  ok = gtk_button_new_with_label ("  OK  ");
  gtk_widget_ref (ok);
  gtk_object_set_data_full (GTK_OBJECT (about_dialog), "ok", ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok);
  gtk_box_pack_start (GTK_BOX (dialog_action_area2), ok, FALSE, FALSE, 0);

  return about_dialog;
}

static void gtv_about( GtkWidget* item, gpointer raw )
{
    static const char* msg = "gtv - Gtk+ MPEG Player\n" \
                             "(c) Michael Vance, 1999\n" \
	                     "    briareos@lokigames.com\n" \
                             "\n" \
                             "Built using:\n" \
	                     "SMPEG - http://www.lokigames.com/development\n" \
	                     "Gtk+ - http://www.gtk.org\n" \
	                     "Glade - http://glade.pn.org\n" \
                             "\n" \
                             "Distributed under the GPL.\n";
    GtkWidget* dialog = NULL;
    GtkWidget* ok = NULL;
    GtkWidget* text = NULL;
    gint ignored = 0;

    dialog = create_about_dialog( );
    ok = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( dialog ), "ok" ) );
    gtk_signal_connect( GTK_OBJECT( ok ), "clicked",
			GTK_SIGNAL_FUNC( gtv_dialog_cancel ), raw );
    text = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( dialog ), "text" ) );
    gtk_editable_insert_text( GTK_EDITABLE( text ), msg, strlen( msg ), &ignored );

    /* HACK HACK HACK */
    gtk_object_set_data( GTK_OBJECT( raw ), "dialog", dialog );

    gtk_grab_add( dialog );
    gtk_widget_show( dialog );
}

static void gtv_rewind( gpointer raw )
{
    SMPEG* mpeg = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    if( mpeg ) {
	SMPEG_Info* info = NULL;

	SMPEG_rewind( mpeg );
	gtv_clear_screen( raw );
	info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );
	SMPEG_getinfo( mpeg, info );
	gtv_set_frame( raw, info->current_frame );
    }

}

static void gtv_double( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SDL_Surface* sdl_screen = NULL;
    SMPEG_Info* info = NULL;
    int stopped=0;
    int width,height;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    sdl_screen = (SDL_Surface*) gtk_object_get_data( GTK_OBJECT( raw ), "sdl_screen" );

    if( mpeg && sdl_screen ) {
	GtkWidget* twotimes = NULL;
	gboolean active = FALSE;

	twotimes = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "twotimes" ) );
	active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( twotimes ) );

	info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );
	assert( info );
        if (SMPEG_status(mpeg)==SMPEG_PLAYING) {
		SMPEG_pause(mpeg);
		stopped=1;
	}
	if( active ) {
	    width = info->width * 2;
	    height = info->height * 2;
	} else {
	    width = info->width;
	    height = info->height;
	}
	sdl_screen = SDL_SetVideoMode(width, height,
		sdl_screen->format->BitsPerPixel,sdl_screen->flags);
        gtv_center_window(sdl_screen);
	SMPEG_setdisplay( mpeg, sdl_screen, NULL, NULL );
	
	gtk_object_set_data( GTK_OBJECT( raw ), "sdl_screen", sdl_screen );
	SMPEG_scaleXY( mpeg, sdl_screen->w, sdl_screen->h );
        gtv_center_window(sdl_screen);
        if (stopped) SMPEG_pause(mpeg);
    }

}

static void gtv_loop( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    if( mpeg ) {
	GtkWidget* loop = NULL;
	gboolean active = FALSE;

	loop = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "loop" ) );
	active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( loop ) );
	SMPEG_loop( mpeg, active );
    }

}

static void gtv_audio( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    if( mpeg ) {
	GtkWidget* audio = NULL;
	gboolean active = FALSE;
	SMPEG_Info* info = NULL;

	audio = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "audio" ) );
	info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );
	active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( audio ) );
	SMPEG_enableaudio( mpeg, active && info->has_audio );
    }

}

static void gtv_filter( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg && info->has_video ) {
	GtkWidget* filter = NULL;
	gboolean active = FALSE;

	filter = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "filter" ) );
	active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( filter ) );

	if(active)
	{
	  SMPEG_Filter * filter;

	  /* Activate the bilinear filter */
       	  filter = SMPEGfilter_bilinear();
	  filter = SMPEG_filter( mpeg, filter );
	  filter->destroy(filter);
	}
	else
	{
	  SMPEG_Filter * filter;

	  /* Reset to default (null) filter */
	  filter = SMPEGfilter_null();
	  filter = SMPEG_filter( mpeg, filter );
	  filter->destroy(filter);
	}
    }

}

static void gtv_play( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    if( mpeg ) {
      //	gtv_rewind( raw );
	SMPEG_play( mpeg );
    }

}

static void gtv_pause( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );

    if( mpeg ) {
	SMPEG_pause( mpeg );
    }

}

static void gtv_stop( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg ) {
	SMPEG_stop( mpeg );
	gtv_rewind( raw );
	SMPEG_getinfo( mpeg, info );
	gtv_set_trackbar( raw, info->current_offset );
    }

}

static void gtv_step( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg && info && info->has_video && ( SMPEG_status( mpeg ) != SMPEG_PLAYING ) ) {
	int next_frame = 0;
	GtkWidget* twotimes = NULL;
	gboolean active = FALSE;
	SDL_Surface* sdl_screen = NULL;

	SMPEG_getinfo( mpeg, info );
	next_frame = info->current_frame + 1;
	twotimes = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "twotimes" ) );
	active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( twotimes ) );
	sdl_screen = (SDL_Surface*) gtk_object_get_data( GTK_OBJECT( raw ), "sdl_screen" );

	SMPEG_renderFrame( mpeg, next_frame );

	SMPEG_getinfo( mpeg, info );

	if( info->current_frame != next_frame ) {
            GtkWidget *looping;

            /* Sam 5/31/2000 - Only loop if the looping toggle is set */
            looping = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(raw), "loop"));
            if ( ! gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(looping)) ) {
                return;
            }
	    gtv_rewind( raw );
	    gtv_step( NULL, raw );
	}

	gtv_set_frame( raw, info->current_frame );
    }

}

static void gtv_to_end( GtkWidget* item, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    assert( raw );

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg && info->has_video ) {
	GtkWidget* twotimes = NULL;
	gboolean active = FALSE;
	SDL_Surface* sdl_screen = NULL;

	SMPEG_stop( mpeg );

	twotimes = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "twotimes" ) );
	active = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( twotimes ) );
	sdl_screen = (SDL_Surface*) gtk_object_get_data( GTK_OBJECT( raw ), "sdl_screen" );

	if( active ) {
	    SMPEG_renderFinal( mpeg, sdl_screen, 0, 0 );
	} else {
	    SMPEG_renderFinal( mpeg, sdl_screen,
			       ( sdl_screen->w - info->width ) / 2,
			       ( sdl_screen->h - info->height ) / 2 );
	}

	SMPEG_getinfo( mpeg, info );
	gtv_set_frame( raw, info->current_frame );
    }

}

static void gtv_seek( GtkAdjustment* adjust, gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    assert( raw );
    if(gtv_trackbar_dragging) return;

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg && info && info->total_size ) {

      /* In case file is growing while we're playing */
      SMPEG_getinfo( mpeg, info );
      
      SMPEG_seek(mpeg, (int)((info->total_size*adjust->value)/100));
      SMPEG_getinfo( mpeg, info );
      gtv_set_frame( raw, info->current_frame );
    }
}

static gboolean gtv_trackbar_drag_on(GtkWidget *widget,
                                     GdkEventButton *event, gpointer raw)
{
    gtv_trackbar_dragging = 1;
    return FALSE;
}

static gboolean gtv_trackbar_drag_off(GtkWidget *widget,
                                      GdkEventButton *event, gpointer raw)
{
    gtv_trackbar_dragging = 0;

    GtkWidget *scale = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "scale" ) );
    GtkAdjustment *seek  = gtk_range_get_adjustment ( GTK_RANGE( scale ) );
    gtk_adjustment_changed( seek );

    return FALSE;
}

static void gtv_set_frame( gpointer raw, int value )
{
    GtkWidget* frame = NULL;
    gchar buffer[32];

    assert( raw );
    assert( value >= 0 );

    frame = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "frame" ) );
    g_snprintf( buffer, 32, "%d", value );
    gtk_label_set_text( GTK_LABEL( frame ), buffer );
}

static void gtv_set_fps( gpointer raw, float value )
{
    GtkWidget* fps = NULL;
    gchar buffer[32];

    assert( raw );
    assert( value >= 0.0 );

    fps = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "fps" ) );
    g_snprintf( buffer, 32, "%2.2f", value );
    gtk_label_set_text( GTK_LABEL( fps ), buffer );
}

static void gtv_set_trackbar( gpointer raw, int value )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;
    GtkWidget* scale = NULL;
    GtkAdjustment* seek = NULL;

    assert( raw );
    assert( value >= 0 );
    if(gtv_trackbar_dragging) return;

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg && info && info->total_size ) {
      scale = GTK_WIDGET( gtk_object_get_data( GTK_OBJECT( raw ), "scale" ) );
      seek  = gtk_range_get_adjustment ( GTK_RANGE( scale ) );
      seek->value = 100. * value / info->total_size;
      gtk_range_set_adjustment ( GTK_RANGE( scale ), seek );
      gtk_adjustment_changed( seek );
    }
}

/* Drag-N-Drop support, contributed by */
static void gtv_drag_data_received(GtkWidget * widget,
                                   GdkDragContext * context,
                                   gint x,
                                   gint y,
                                   GtkSelectionData * selection_data,
                                   guint info,
                                   guint time)
{
    gchar *temp, *string;

    string = (gchar *)selection_data->data;

    /* remove newline at end of line, and the file:// url header
       at the begining, copied this code from the xmms source */
    temp = strchr(string, '\n');
    if (temp)
    {
        if (*(temp - 1) == '\r')
            *(temp - 1) = '\0';
        *temp = '\0';
    }
    if (!strncmp(string, "file:", 5))
        string = string + 5;

    gtv_open_file(string, widget);
    gtv_play(NULL, widget);
}

static gint gtv_timer( gpointer raw )
{
    SMPEG* mpeg = NULL;
    SMPEG_Info* info = NULL;

    mpeg = (SMPEG*) gtk_object_get_data( GTK_OBJECT( raw ), "mpeg" );
    info = (SMPEG_Info*) gtk_object_get_data( GTK_OBJECT( raw ), "info" );

    if( mpeg && info ) {

	if( SMPEG_status( mpeg ) == SMPEG_PLAYING ) {
	    SMPEG_getinfo( mpeg, info );
	    gtv_set_frame( raw, info->current_frame );
	    gtv_set_fps( raw, info->current_fps );
	    gtv_set_trackbar( raw, info->current_offset );
	}

    }

    return 1;
}

static void gtv_clear_screen( gpointer raw )
{
    SDL_Surface* sdl_screen = NULL;

    sdl_screen = (SDL_Surface*) gtk_object_get_data( GTK_OBJECT( raw ), "sdl_screen" );

    if( sdl_screen ) {
	SDL_FillRect( sdl_screen, NULL, SDL_MapRGB( sdl_screen->format, 0, 0, 0 ) );
	SDL_UpdateRect( sdl_screen, 0, 0, 0, 0 );
    }

}

static void gtv_quit( GtkWidget* item, gpointer raw )
{
    gtv_close( NULL, raw );
    gtk_exit( 0 );
}

static void gtv_connect( gpointer raw, gchar* name, gchar* signal, GtkSignalFunc func )
{
    GtkObject* item = NULL;

    assert( raw );

    item = GTK_OBJECT( gtk_object_get_data( GTK_OBJECT( raw ), name ) );
    assert( item );
    gtk_signal_connect(item, signal, func, raw );
}

int main( int argc, char* argv[] )
{
    static GtkTargetEntry drop_types[] = 
    {
        { "text/plain", 0, 1 }
    };
    /* These are always on the stack until program exit. */
    SMPEG_Info info;
    gchar filename_buffer[FILENAME_BUFFER_SIZE];

    GtkWidget* window = NULL;

    memset( &info, 0, sizeof( info ) );
    memset( &filename_buffer, 0, sizeof( gchar ) * FILENAME_BUFFER_SIZE );
    gtk_set_locale();
    gtk_init( &argc, &argv );

    window = create_gtv_window( );
    gtk_drag_dest_set(window, GTK_DEST_DEFAULT_ALL, drop_types, 1, GDK_ACTION_COPY);
    gtk_signal_connect( GTK_OBJECT( window ), "drag_data_received",
			GTK_SIGNAL_FUNC( gtv_drag_data_received ), window );
    gtk_signal_connect( GTK_OBJECT( window ), "destroy",
			GTK_SIGNAL_FUNC( gtv_quit ), window );
    gtk_object_set_data( GTK_OBJECT( window ), "info", &info );
    gtk_object_set_data( GTK_OBJECT( window ), "mpeg", NULL );
    gtk_object_set_data( GTK_OBJECT( window ), "sdl_screen", NULL );
    gtk_object_set_data( GTK_OBJECT( window ), "filename_buffer", filename_buffer );
    gtk_widget_show( window );

    gtv_connect( window, "open", "activate", GTK_SIGNAL_FUNC( gtv_open ) );
    gtv_connect( window, "close", "activate", GTK_SIGNAL_FUNC( gtv_close ) );
    gtv_connect( window, "file_info", "activate", GTK_SIGNAL_FUNC( gtv_info ) );
    gtv_connect( window, "quit", "activate", GTK_SIGNAL_FUNC( gtv_quit ) );
    gtv_connect( window, "about", "activate", GTK_SIGNAL_FUNC( gtv_about ) );

    gtv_connect( window, "play", "clicked", GTK_SIGNAL_FUNC( gtv_play ) );
    gtv_connect( window, "pause", "clicked", GTK_SIGNAL_FUNC( gtv_pause ) );
    gtv_connect( window, "stop", "clicked", GTK_SIGNAL_FUNC( gtv_stop ) );
    gtv_connect( window, "step", "clicked", GTK_SIGNAL_FUNC( gtv_step ) );
    gtv_connect( window, "to_end", "clicked", GTK_SIGNAL_FUNC( gtv_to_end ) );

    gtv_connect( window, "twotimes", "toggled", GTK_SIGNAL_FUNC( gtv_double ) );
    gtv_connect( window, "loop", "toggled", GTK_SIGNAL_FUNC( gtv_loop ) );
    gtv_connect( window, "audio", "toggled", GTK_SIGNAL_FUNC( gtv_audio ) );
    gtv_connect( window, "filter", "toggled", GTK_SIGNAL_FUNC( gtv_filter ) );

    gtv_connect( window, "seek", "value_changed", GTK_SIGNAL_FUNC( gtv_seek ) );

    gtv_connect( window, "scale", "button_press_event", GTK_SIGNAL_FUNC( gtv_trackbar_drag_on ) );
    gtv_connect( window, "scale", "button_release_event", GTK_SIGNAL_FUNC( gtv_trackbar_drag_off ) );

    /*    gtk_idle_add_priority( G_PRIORITY_LOW, gtv_timer, window );*/
    gtk_timeout_add( TIMER_TIMEOUT, gtv_timer, window );

    gtv_set_frame( window, 0 );
    gtv_set_fps( window, 0.0 );
    gtv_set_buttons_sensitive( window, FALSE );

    if( argc > 1 ) {
	gtv_open_file( argv[1], window );
    }

    gtk_main( );

    return 0;
}

static GtkWidget* create_gtv_window( void )
{
  GtkWidget *gtv_window;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *file;
  GtkWidget *file_menu;
  GtkWidget *open;
  GtkWidget *close;
  GtkWidget *separator1;
  GtkWidget *file_info;
  GtkWidget *separator2;
  GtkWidget *quit;
  GtkWidget *help;
  GtkWidget *help_menu;
  GtkWidget *about;
  GtkWidget *table1;
  GtkObject *seek;
  GtkWidget *scale;
  GtkWidget *play;
  GtkWidget *pause;
  GtkWidget *stop;
  GtkWidget *step;
  GtkWidget *to_end;
  GtkWidget *twotimes;
  GtkWidget *loop;
  GtkWidget *audio;
  GtkWidget *filter;
  GtkWidget *label3;
  GtkWidget *fps;
  GtkWidget *label2;
  GtkWidget *frame;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  gtv_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (gtv_window), "gtv_window", gtv_window);
  gtk_window_set_title (GTK_WINDOW (gtv_window), "gtv");
  gtk_widget_set_uposition (GTK_WIDGET (gtv_window), 0, 0);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (gtv_window), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_ref (menubar1);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "menubar1", menubar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  file = gtk_menu_item_new_with_label ("File");
  gtk_widget_ref (file);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "file", file,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (file);
  gtk_container_add (GTK_CONTAINER (menubar1), file);

  file_menu = gtk_menu_new ();
  gtk_widget_ref (file_menu);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "file_menu", file_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), file_menu);

  open = gtk_menu_item_new_with_label ("Open");
  gtk_widget_ref (open);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "open", open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open);
  gtk_container_add (GTK_CONTAINER (file_menu), open);
  gtk_widget_add_accelerator (open, "activate", accel_group,
                              GDK_O, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  close = gtk_menu_item_new_with_label ("Close");
  gtk_widget_ref (close);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "close", close,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (close);
  gtk_container_add (GTK_CONTAINER (file_menu), close);
  gtk_widget_add_accelerator (close, "activate", accel_group,
                              GDK_W, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator1 = gtk_menu_item_new ();
  gtk_widget_ref (separator1);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (file_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  file_info = gtk_menu_item_new_with_label ("Info");
  gtk_widget_ref (file_info);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "file_info", file_info,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (file_info);
  gtk_container_add (GTK_CONTAINER (file_menu), file_info);
  gtk_widget_add_accelerator (file_info, "activate", accel_group,
                              GDK_I, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator2 = gtk_menu_item_new ();
  gtk_widget_ref (separator2);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (file_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  quit = gtk_menu_item_new_with_label ("Quit");
  gtk_widget_ref (quit);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "quit", quit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (quit);
  gtk_container_add (GTK_CONTAINER (file_menu), quit);
  gtk_widget_add_accelerator (quit, "activate", accel_group,
                              GDK_Q, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  help = gtk_menu_item_new_with_label ("Help");
  gtk_widget_ref (help);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "help", help,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (help);
  gtk_container_add (GTK_CONTAINER (menubar1), help);
  gtk_menu_item_right_justify (GTK_MENU_ITEM (help));

  help_menu = gtk_menu_new ();
  gtk_widget_ref (help_menu);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "help_menu", help_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), help_menu);

  about = gtk_menu_item_new_with_label ("About");
  gtk_widget_ref (about);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "about", about,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (about);
  gtk_container_add (GTK_CONTAINER (help_menu), about);
  gtk_widget_add_accelerator (about, "activate", accel_group,
                              GDK_F1, 0,
                              GTK_ACCEL_VISIBLE);

  table1 = gtk_table_new (4, 5, FALSE);
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 5);

  seek = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 0.0, 0.0);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "seek", seek,
                            (GtkDestroyNotify) (0));
  scale = gtk_hscale_new(GTK_ADJUSTMENT(seek));
  gtv_trackbar_dragging = 0;
  gtk_widget_ref (scale);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "scale", scale,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_range_set_update_policy (GTK_RANGE (scale), GTK_UPDATE_DELAYED);
  gtk_widget_show (scale);
  gtk_table_attach (GTK_TABLE (table1), scale, 0, 5, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  play = gtk_button_new_with_label ("  Play  ");
  gtk_widget_ref (play);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "play", play,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (play);
  gtk_table_attach (GTK_TABLE (table1), play, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  pause = gtk_button_new_with_label ("  Pause  ");
  gtk_widget_ref (pause);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "pause", pause,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pause);
  gtk_table_attach (GTK_TABLE (table1), pause, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  stop = gtk_button_new_with_label ("  Stop  ");
  gtk_widget_ref (stop);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "stop", stop,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (stop);
  gtk_table_attach (GTK_TABLE (table1), stop, 2, 3, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  step = gtk_button_new_with_label ("  Step  ");
  gtk_widget_ref (step);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "step", step,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (step);
  gtk_table_attach (GTK_TABLE (table1), step, 3, 4, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  to_end = gtk_button_new_with_label ("  To End  ");
  gtk_widget_ref (to_end);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "to_end", to_end,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (to_end);
  gtk_table_attach (GTK_TABLE (table1), to_end, 4, 5, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  twotimes = gtk_check_button_new_with_label ("Double");
  gtk_widget_ref (twotimes);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "twotimes", twotimes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (twotimes);
  gtk_table_attach (GTK_TABLE (table1), twotimes, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  loop = gtk_check_button_new_with_label ("Loop");
  gtk_widget_ref (loop);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "loop", loop,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (loop);
  gtk_table_attach (GTK_TABLE (table1), loop, 1, 2, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  audio = gtk_check_button_new_with_label ("Audio");
  gtk_widget_ref (audio);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "audio", audio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (audio);
  gtk_table_attach (GTK_TABLE (table1), audio, 2, 3, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  filter = gtk_check_button_new_with_label ("Filter");
  gtk_widget_ref (filter);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "filter", filter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (filter);
  gtk_table_attach (GTK_TABLE (table1), filter, 3, 4, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label3 = gtk_label_new ("Current frame:");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 1, 0.5);

  fps = gtk_label_new ("<fps>");
  gtk_widget_ref (fps);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "fps", fps,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fps);
  gtk_table_attach (GTK_TABLE (table1), fps, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (fps), 1, 0.5);

  label2 = gtk_label_new ("fps");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 4, 5, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  frame = gtk_label_new ("<frame>");
  gtk_widget_ref (frame);
  gtk_object_set_data_full (GTK_OBJECT (gtv_window), "frame", frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame);
  gtk_table_attach (GTK_TABLE (table1), frame, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (frame), 0, 0.5);

  gtk_window_add_accel_group (GTK_WINDOW (gtv_window), accel_group);

  return gtv_window;
}

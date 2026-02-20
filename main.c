#include <gtk-3.0/gtk/gtk.h>
#include <fontconfig/fontconfig.h>
#include <cairo/cairo.h>
#include <gst/gst.h>

// Struct das configurações para o gif
struct Gifsettings{

    GtkWidget *gif_path;
    GtkWidget *music_path;
    GtkWidget *home_window;
    GtkWidget *gif_window;
    GstElement *play_music;
    gint *x_pos;
    gint *y_pos;

};

GdkPixbufAnimation *gif = NULL;

// Widget para exibir o gif
GtkWidget *gif_display = NULL;

// Iterador para iterar os frames do gif
GdkPixbufAnimationIter *iter = NULL; 
GTimeVal current_time;
gint x_pos = 0;
gint y_pos = 0;

// Janela que mostra os erros
void Error_Message(GtkWidget *window_parent, gchar *message){

    GtkDialogFlags flag = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *error_window = gtk_message_dialog_new(GTK_WINDOW(window_parent), flag, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, message);
    gtk_window_set_decorated(GTK_WINDOW(error_window), FALSE);

    // Linkando a error_window com a classe ErrorWindow do css
    GtkStyleContext *context_error_window = gtk_widget_get_style_context(error_window);
    gtk_style_context_add_class(context_error_window, "ErrorWindow");

    gtk_dialog_run(GTK_DIALOG(error_window));
    gtk_widget_destroy(error_window);

}

// Movimentar a gif_window 
gboolean Move_Gif_Window(GtkWidget *window, GdkEventButton *event, gpointer settings){

    GtkWindow *gif_window = GTK_WINDOW(window);

    if(event->type == GDK_BUTTON_PRESS && event->button == 1){

        gtk_window_begin_move_drag(gif_window, event->button, event->x_root, event->y_root, event->time);

    }

    return TRUE;

}

// Fechar a janela
void Close_Window(GtkWidget *home_window, gpointer close_window){

    GtkWidget *gif_window = (GtkWidget *)close_window;

    if(GTK_IS_WINDOW(gif_window)){

        gtk_widget_destroy(gif_window);

    }

    gtk_main_quit();

}

// Atualiza a animação/quadro do gif
gboolean Gif_Update(gpointer settings){

    if (iter == NULL || gif == NULL){

        return FALSE;

    }

    GtkWindow *gif_window = (GtkWindow *)settings;

    if(GTK_IS_WINDOW(gif_window)){

        // Obtém a posição atual da janela gif_window
        gtk_window_get_position(gif_window, &x_pos, &y_pos);

    }

    // Obtém o quadro atual da animação usando o iterador
    GdkPixbuf *frame = gdk_pixbuf_animation_iter_get_pixbuf(iter);

    // Guarda o tempo atual
    g_get_current_time(&current_time);

    // Exibe o quadro na janela
    if(frame && GTK_IS_WIDGET(gif_display)){

        gtk_image_set_from_pixbuf(GTK_IMAGE(gif_display), frame);

    }

    // Avança para o próximo quadro
    gdk_pixbuf_animation_iter_advance(iter, &current_time);

    return TRUE;

}

// Tornar a janela transparente
gboolean Make_Transparent(GtkWidget *gif_window, cairo_t *cr, gpointer settings){
    
    // Define a cor de fundo como transparente
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);

    return FALSE;

}

// Fazer loop da música
gboolean Music_Loop(GstBus *bus, GstMessage *msg, gpointer data){

    GstElement *play_music = GST_ELEMENT(data);

    if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS){

        // Voltando para o começo da música
        gst_element_seek_simple(play_music, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, 0);

    }
}

// Janela do gif
gboolean Gif_Window(GtkWidget *button_clicked, gpointer settings){

    // Fazendo o casting do tipo gpointer para o tipo struct Gifsettings
    struct Gifsettings *gif_settings = (struct Gifsettings *)settings;

    // impede que mais de uma gif_window seja criada e que mais de uma música seja tocada ao clicar nos botões de pré-visualização e salvar
    if(gif_settings->gif_window != NULL){

        gtk_widget_destroy(gif_settings->gif_window);
        
    }

    // Caminho do gif
    char *gif_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gif_settings->gif_path));
    
    // Caminho da música
    char *music_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gif_settings->music_path));
    
    if(music_path){

        char *uri = g_strdup(g_filename_to_uri(music_path, NULL, NULL));

        // Parando a música anterior, caso exista
        gst_element_set_state(gif_settings->play_music, GST_STATE_NULL);

        // Setando a música selecionada e reproduzindo a música
        g_object_set(gif_settings->play_music, "uri", uri, NULL);
        gst_element_set_state(gif_settings->play_music, GST_STATE_PLAYING);

        // Liberando a memória alocada pelo g_filename_to_uri e g_strdup
        g_free(uri);
        g_free(music_path);


    }
    
    // Verificando se o caminho do gif é válido
    if(!gif_path){

        Error_Message(gif_settings->home_window, "\n\nINVALID GIF PATH");
        return FALSE; 

    }

    // Carregando o gif
    gif = gdk_pixbuf_animation_new_from_file(gif_path, NULL);

    // Carregando o iterador
    iter = gdk_pixbuf_animation_get_iter(gif, NULL);

    gint width = gdk_pixbuf_animation_get_width(gif);
    gint height = gdk_pixbuf_animation_get_height(gif);

    // Criação da janela
    gif_settings->gif_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(gif_settings->gif_window), width, height);
    gtk_window_move(GTK_WINDOW(gif_settings->gif_window), x_pos, y_pos);
    gtk_window_set_resizable(GTK_WINDOW(gif_settings->gif_window), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(gif_settings->gif_window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(gif_settings->gif_window), TRUE);
    gtk_widget_set_app_paintable(gif_settings->gif_window, TRUE);

    // Configuração do RGBA para ter a transparência
    GdkScreen *screen = gtk_widget_get_screen(gif_settings->gif_window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);

    if(visual != NULL && gdk_screen_is_composited(screen)){

        gtk_widget_set_visual(gif_settings->gif_window, visual);

    }else{

        Error_Message(gif_settings->home_window, "\n\nWINDOWS COMPOSITION NOT SUPPORTED");
        return FALSE;

    }

    gif_display = gtk_image_new_from_pixbuf(gdk_pixbuf_animation_iter_get_pixbuf(iter));
    gtk_container_add(GTK_CONTAINER(gif_settings->gif_window), gif_display);

    // Sinal para tornar a janela transparente
    g_signal_connect(G_OBJECT(gif_settings->gif_window), "draw", G_CALLBACK(Make_Transparent), NULL);

    // Habilitar a movimentação da gif window
    if(g_str_equal(gtk_button_get_label(GTK_BUTTON(button_clicked)), "Preview")){

        g_signal_connect(G_OBJECT(gif_settings->gif_window), "button-press-event", G_CALLBACK(Move_Gif_Window), NULL);

    }

    // Atualização do gif em ms (16.6 ms +- 60 FPS)
    g_timeout_add(16.6, Gif_Update, gif_settings->gif_window);

    // Mostrar a janela
    gtk_widget_show_all(gif_settings->gif_window);

    // Liberando a memória alocada pelo gtk_file_chooser_get_filename
    g_free(gif_path);

    return TRUE;

}

// Janela inicial
void Home_Window(GtkApplication *app, gpointer data){

    struct Gifsettings gif_settings;
    gif_settings.gif_window = NULL;
    gif_settings.x_pos = 0;
    gif_settings.y_pos = 0;

    // Criação do elemento playbin para tocar a música selecionada
    gif_settings.play_music = gst_element_factory_make("playbin", "play_music");

    // Criação da janela
    GtkWidget *home_window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(home_window), 336, 264);
    gtk_window_set_position(GTK_WINDOW(home_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(home_window), FALSE);
    gif_settings.home_window = home_window;

    // Setando um ícone para a janela home_windows
    gtk_window_set_default_icon_from_file("./assets/images/icon.png", NULL);

    // Criando recipiente/contentor fixo
    GtkWidget *container_fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(home_window), container_fixed);
    gtk_widget_show(container_fixed);

    // Cabeçalho personalizado
    GtkWidget *header = gtk_header_bar_new();
    GtkWidget *title = gtk_label_new("GIFDisplay");
    const char *font_path = "./assets/fonts/CandyBeans.otf";

    // Mudando a fonte
    // *** tentar colocar uma pixelart na header
    FcConfigAppFontAddFile(FcConfigGetCurrent(), (const FcChar8 *)font_path);
    PangoFontDescription *font_desc = pango_font_description_from_string("CandyBeans 20");
    gtk_widget_override_font(title, font_desc);

    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header), title);
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(home_window), header);

    // Linkando a header com a classe HeaderHomeWindow do css
    GtkStyleContext *context_header = gtk_widget_get_style_context(header);
    gtk_style_context_add_class(context_header, "HeaderHomeWindow");

    // Linkando a home_window com a classe HomeWindow do css
    GtkStyleContext *context_home_window = gtk_widget_get_style_context(home_window);
    gtk_style_context_add_class(context_home_window, "HomeWindow");

    // Legenda superior do botão para selecionar o gif
    GtkWidget *label_button_choose_gif = gtk_label_new("Select gif");
    gtk_widget_override_font(label_button_choose_gif, font_desc);
    gtk_fixed_put(GTK_FIXED(container_fixed), label_button_choose_gif, 15, 19);

    // Linkando a label_button_choose_gif com a classe LabelButtonChooseGif do css
    GtkStyleContext *context_label_button_choose_gif = gtk_widget_get_style_context(label_button_choose_gif);
    gtk_style_context_add_class(context_label_button_choose_gif, "LabelButtonChooseGif");

    // Botão para selecionar o gif
    GtkWidget *button_choose_gif = gtk_file_chooser_button_new("Select gif", GTK_FILE_CHOOSER_ACTION_OPEN);
    GtkFileFilter *file_filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(file_filter, "image/gif");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(button_choose_gif), "./gifs");
    gtk_file_chooser_button_set_width_chars(GTK_FILE_CHOOSER_BUTTON(button_choose_gif), 14);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(button_choose_gif), file_filter);
    gtk_fixed_put(GTK_FIXED(container_fixed), button_choose_gif, 15, 40);
    gif_settings.gif_path = button_choose_gif;

    // Linkando o button_choose_gif com a classe ButtonChooseGif do css
    GtkStyleContext *context_button_choose_gif = gtk_widget_get_style_context(button_choose_gif);
    gtk_style_context_add_class(context_button_choose_gif, "ButtonChooseGif");

    // Legenda superior do botão para selecionar a música
    GtkWidget *label_button_choose_music = gtk_label_new("Select music");
    gtk_widget_override_font(label_button_choose_music, font_desc);
    gtk_fixed_put(GTK_FIXED(container_fixed), label_button_choose_music, 15, 73);

    // Linkando a label_button_choose_music com a classe LabelButtonChooseMusic do css
    GtkStyleContext *context_label_button_choose_music = gtk_widget_get_style_context(label_button_choose_music);
    gtk_style_context_add_class(context_label_button_choose_music, "LabelButtonChooseMusic");

    // Botão para selecionar a música
    GtkWidget *button_choose_music = gtk_file_chooser_button_new("Select music", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_filter_add_mime_type(file_filter, "audio/mpeg");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(button_choose_music), "./musics");
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(button_choose_music), "./musics/bailando.mp3");
    gtk_file_chooser_button_set_width_chars(GTK_FILE_CHOOSER_BUTTON(button_choose_music), 14);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(button_choose_music), file_filter);
    gtk_fixed_put(GTK_FIXED(container_fixed), button_choose_music, 15, 94);
    gif_settings.music_path = button_choose_music;

    // Linkando o button_choose_music com a classe ButtonChooseMusic do css
    GtkStyleContext *context_button_choose_music = gtk_widget_get_style_context(button_choose_music);
    gtk_style_context_add_class(context_button_choose_music, "ButtonChooseMusic");

    // Botão para a pré-visualização do gif
    GtkWidget *button_preview = gtk_button_new_with_label("Preview");
    gtk_fixed_put(GTK_FIXED(container_fixed), button_preview, 15, 160);

    // Sinal do clique no botão de pré-visualização do gif
    g_signal_connect(button_preview, "clicked", G_CALLBACK(Gif_Window), &gif_settings);

    // Linkando o button_preview com a classe ButtonPreview do css
    GtkStyleContext *context_button_preview = gtk_widget_get_style_context(button_preview);
    gtk_style_context_add_class(context_button_preview, "ButtonPreview");

    // Botão para fixar/salvar a posição do gif
    GtkWidget *button_save = gtk_button_new_with_label("Save");
    gtk_fixed_put(GTK_FIXED(container_fixed), button_save, 122, 160);

    // Sinal do clique no botão de fixar/salvar do gif
    g_signal_connect(button_save, "clicked", G_CALLBACK(Gif_Window), &gif_settings);

    // Linkando o button_save com a classe ButtonSave do css
    GtkStyleContext *context_button_save = gtk_widget_get_style_context(button_save);
    gtk_style_context_add_class(context_button_save, "ButtonSave");

    // Loop da música
    GstBus *bus = gst_element_get_bus(gif_settings.play_music);
    gst_bus_add_watch(bus, Music_Loop, gif_settings.play_music);    
    gst_object_unref(bus);

    // Sinal para fechar a janela
    g_signal_connect(home_window, "destroy", G_CALLBACK(Close_Window), NULL);

    // Exibe a janela
    gtk_widget_show_all(home_window);

    // Loop principal
    gtk_main();

    // Liberando a memória ocupada pela fonte 
    pango_font_description_free(font_desc);

}

int main(int argc, char **argv){

    gst_init(&argc, &argv);
    gtk_init(&argc, &argv);

    int status;

    // Criando o ícone para o lançador
    gdk_set_program_class("gifdisplay");
    gchar *path = g_build_filename(g_get_home_dir(), ".local/share/applications/GIFDisplay.desktop", NULL);
    gchar *content = g_strconcat("[Desktop Entry]\nVersion=0.1\nType=Application\nName=GIFDisplay\nExec=",g_get_current_dir(),"/GIFDisplay.out\nIcon=",g_get_current_dir(),"/assets/images/icon.png\nTerminal=false\nCategories=Application\nStartupWMClass=gifdisplay\n", NULL);
    g_file_set_contents(path, content, -1, NULL);
    g_free(path);

    // Conectando com o arquivo css
    GtkCssProvider *css_file = gtk_css_provider_new();
    // pode retornar um erro
    gtk_css_provider_load_from_path(css_file, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_file), GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkApplication *app = gtk_application_new("org.gtk.home_window", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(Home_Window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;

}
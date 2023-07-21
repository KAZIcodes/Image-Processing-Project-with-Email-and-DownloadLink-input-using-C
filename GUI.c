#include <gtk/gtk.h>

// maine khodemon



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Email and link downloading library:
#include <curl/curl.h>
#include "cdecode.h"
// filters and BMP format specifications module:
#include "filters.h"  //(bmp.h ham tooye hamin include shode)

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>




//baraye inke stb_image library dorost kar kone
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define h_addr h_addr_list[0]
#define PORT 443
#define FILTERS 14

#include "stb_image.h"
#include "stb_image_write.h"



// Email functions:
int downloadLastEmailAttachment(const char* url, const char* username, const char* password);
static size_t write_data_email(void *ptr, size_t size, size_t nmemb, void *userdata);
char* find_attachment_UID(const char* url, const char* username, const char* password, int first_time);
size_t discard_response(void *ptr, size_t size, size_t nmemb, void *userdata);
int read_int(char *input);
long last_index_of_substring(char* str, char* substr);
void decode64(const char *base64_image);
FILE * formatchecker(FILE * fp);
FILE * parsebmp(FILE * fp);
FILE * parsepng(FILE * fp);
FILE * parsejpg(FILE * fp);
void download_url(char * SERVER, char * PATH);

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

// Download_Link functions:
FILE *download_image(char *url);

char is_bmp(const BMPHeader *header, FILE *fp); // checks if the file is BMP

// function to choose filter

// get format
char get_image_format(FILE *fp);
// JPEG functions
unsigned char* load_jpeg_image2(const char* filename, int* width, int* height, int* channels);
void write_jpeg_image(const char* filename, int width, int height, int channels, unsigned char* data);
// PNG functions
unsigned char* load_png(const char* filename, int* width, int* height, int* channels);
void write_png(const char* filename, const unsigned char* data, int width, int height, int channels);
void filterapplication(int filter, int height, int width, RGB (*image_mat)[width], int is_png, int channel, unsigned char *pixels);

static int email_input = 0;


// end of maine khodemon






static GtkWidget * outerbox;
static GtkWidget *window;
static int showindex=0;
static char ogimage[] = "./filtered/org.jpg";
static char imagelist[][50]= {"./filtered/blurred.jpg","./filtered/grayscale.jpg",
"./filtered/reflect.jpg","./filtered/sepia.jpg","./filtered/artisticedge.jpg","./filtered/dark.jpg","./filtered/emboss.jpg","./filtered/posterize.jpg","./filtered/sponge.jpg","./filtered/neonglow.jpg","./filtered/g11.jpg","./filtered/a12.jpg","./filtered/improv13.jpg","./filtered/improv14.jpg","./filtered/min15.jpg"};
static char filterlist[][20] = {"Blur","grayscale","Reflect","Sepia","Artistic Edge","Dark Grain","Emboss","Posterize","Sponge","Neon Glow", "Color wash","Improv 1","Improv 2","Improv 3", "improv 4"};
static char * filteredfilename=ogimage;
static int imagefilenotfound =0;
static int emailfailed = 0;
static int approvedemail=0;
static char * file_name;
static int urlerror = 0;
static int invalidformat = 0;
static int height;
static int width;
static int channels;
static char globalformat;
char * latestfiltername;
static int globalcase;
static int invalidvalue=0;
static int enteredfilter=0;
static int invalidvalue1 = 0;
static int invalidvalue2 = 0;


// for subcases

static int case1entry;
static int case8entry;
static int case9entry;
static int case10entry1;
static int case10entry2;
static int case11entry;
static int case12entry;
char * case15entry;

// end for subcases


//static BMPHeader header;
FILE * image;
FILE * logfile;
BMPHeader myheader;

void first_page(GtkWidget *window);
void second_page(GtkWidget * window);
void get_by_url(GtkWidget * window);
void get_by_email(GtkWidget * window);
void filter_page(GtkWidget* window);
void on_text_entered(GtkEntry *entry, gpointer user_data);
void pleasewait1(GtkWidget * window);
void prev_button_clicked(GtkWidget *button, gpointer data);
void approve_button_clicked(GtkWidget * button, gpointer data);
void button_clicked1(GtkWidget *button1, gpointer data);
void button_clicked2(GtkWidget *button1, gpointer data);
void button_clicked3(GtkWidget *button1, gpointer data);

// for resizing the image
struct _resize_widgets {
   GtkWidget *image;
   GdkPixbuf *pixbuf;
};

void moveindexright(void){
    if (showindex+2<14)
    showindex++;
    gtk_widget_destroy(outerbox);
    filter_page(window);
    
    return;
}
void moveindexleft(void){
    if (showindex>0)
    showindex--;
    gtk_widget_destroy(outerbox);
    filter_page(window);
    
    return;
}

void case1entryfunc(GtkEntry *entry, gpointer user_data){
    case1entry = atoi(gtk_entry_get_text(entry));
    if (case1entry >50 || case1entry <2){
        invalidvalue = 1;

    }
    else {
        invalidvalue = 0;
    }
    enteredfilter = 1;
            gtk_widget_destroy(outerbox);
        filter_page(window);
}

void case8entryfunc(GtkEntry *entry, gpointer user_data){
    case8entry = atoi(gtk_entry_get_text(entry));
    if (case8entry > 5 || case8entry <2){
        invalidvalue = 1;

    }
    else {
        invalidvalue= 0;
    }
    enteredfilter =1;
        gtk_widget_destroy(outerbox);
        filter_page(window);
}

void case9entryfunc(GtkEntry *entry, gpointer user_data){
    case9entry = atoi(gtk_entry_get_text(entry));
    enteredfilter =1;
}

void case10entryfunc1(GtkEntry *entry, gpointer user_data){
    case10entry1 = atoi(gtk_entry_get_text(entry));
    if (case10entry1 <=0 || case10entry1 >1000
    ){
        invalidvalue =1; 
        enteredfilter =0;
        invalidvalue1 = 1;
    }
    else{
    invalidvalue =0;
    enteredfilter = 1;
    invalidvalue1 = 0;
    }
        gtk_widget_destroy(outerbox);
        filter_page(window);
}

void case10entryfunc2(GtkEntry *entry, gpointer user_data){
    case10entry2 = atoi(gtk_entry_get_text(entry));
    if (case10entry2 <= 0 || case10entry2 >255){
        invalidvalue = 1;
        enteredfilter = 0;
        invalidvalue2 = 1;
    }
    else {
        invalidvalue = 0;
        enteredfilter = 1;
        invalidvalue2 = 0;
    }
        gtk_widget_destroy(outerbox);
        filter_page(window);
}

void case11entryfunc(GtkEntry *entry, gpointer user_data){
    char * tmppointer = gtk_entry_get_text(entry);
    g_print("--%s--",tmppointer);
    case11entry =tmppointer[0];
    if (case11entry != 'R' && case11entry != 'G' && case11entry != 'B' && case11entry != 'r' && case11entry != 'g' && case11entry != 'b')
    {
        invalidvalue = 1;

    }
    else {
        invalidvalue = 0;
    }
    enteredfilter = 1;
        gtk_widget_destroy(outerbox);
        filter_page(window);
}

void case12entryfunc(GtkEntry *entry, gpointer user_data){
    char * tmppointer = gtk_entry_get_text(entry);
    g_print("--%s--",tmppointer);
    case12entry =tmppointer[0];
    if (case12entry != 'A' && case12entry != 'B' && case12entry != 'C' && case12entry != 'D' && case12entry != 'E' && case12entry != 'F'
    && case12entry != 'a' && case12entry != 'b' && case12entry != 'c' && case12entry != 'd' && case12entry != 'e' && case12entry != 'f')
    {
        invalidvalue = 1;
        enteredfilter = 0;
    }
    else {
        invalidvalue =0;
        enteredfilter = 1;
    }
        gtk_widget_destroy(outerbox);
        filter_page(window);
}
void case15entryfunc(GtkEntry *entry, gpointer user_data){
    case15entry = malloc(1024);
    strcpy(case15entry,gtk_entry_get_text(entry));
    g_print("--%s--",case15entry);
    if (strcmp(case15entry,"Min")!= 0 && strcmp(case15entry,"Max") != 0){
        invalidvalue = 1;

    }
    else {
        invalidvalue = 0;
    }
        enteredfilter =1;
        gtk_widget_destroy(outerbox);
        filter_page(window);
}


void applyfilterbuttonclicked(GtkButton * button, gpointer data){
    if (invalidvalue== 0 && enteredfilter && !invalidvalue1 && !invalidvalue2)
    {
    //g_print("MY BUTTON CLICKED: %s",latestfiltername);
    for (int index = 0; index < FILTERS+1; index++){
        if (strcmp(latestfiltername,filterlist[index])==0)
        {
            fprintf(logfile,"filter found, applying : %s\n",latestfiltername);
//////////////////////////
    /*
    if (image == NULL)
    {
        g_print("Image file could not be received :)!\n");
        fclose(image);
        exit(4);
    }
    printf("IMAGE POS%p\n",(void*)image);
    */
    BMPHeader *header= &myheader;
    
    
    fseek(image,0L,SEEK_SET);
    /*
    if (fread(&header, sizeof(BMPHeader), 1, image) != 1)
    {
        printf("Failed to read file's header:)\n");
        fclose(image);
        exit(5);
    }
    */

    //char format = is_bmp(header,image);
    
    
    switch (globalformat) {
        case 'B': {
            // returning to the first of the file because of some unusual BMPs
            fseek(image, 0, SEEK_SET);

            // reading BMP header
            if (fread(header, sizeof(BMPHeader), 1, image) != 1) {
                fprintf(logfile,"Failed to read BMP header\n");
                fclose(image);
                exit(6);
            }

            // returning to the first of the file because of some unusual BMPs
            fseek(image, 0, SEEK_SET);

            BYTE header_towrite[header->bfOffBits];
            if (fread(&header_towrite, header->bfOffBits, 1, image) != 1) {
                fprintf(logfile,"Failed to write to output header\n");
                fclose(image);
                exit(7);
            }

            //printf("\n\noffset:%i  bitdepth:%i  headersize:%i\n\n", header->bfOffBits, header->biBitCount, header->biSize);


            // Get image's dimensions
            height = abs(header->biHeight);
            width = header->biWidth;
            channels = header->biBitCount;

            // go to the start of image data(data array)
            fseek(image, header->bfOffBits, SEEK_SET);


            // Allocate memory for image
            RGB (*image_mat)[width] = calloc(height, width * sizeof(RGB));

            if (image_mat == NULL) {
                fprintf(logfile,"Not enough memory to store image.\n");
                fclose(image);
                exit(8);
            }


            // Determine padding for scanlines
            int padding = (4 - (width * sizeof(RGB)) % 4) % 4;

            // Iterate over infile's scanlines
            for (int i = 0; i < height; i++) {
                // Read row into pixel array
                fread(image_mat[i], sizeof(RGB), width, image);

                // Skip over padding
                fseek(image, padding, SEEK_CUR);
            }
            //printf("FIlter NUM:::%i",index+1);
            filterapplication(index+1,height, width, image_mat, 0, channels, NULL);

            // Write new pixels to outfile
            FILE *output = fopen("OutPic.bmp", "wb");
            if (output == NULL) {
                fprintf(logfile,"Could not write out the output file\n");
                exit(9);
            }

            // first writing the BMP header
            fwrite(&header_towrite, header->bfOffBits, 1, output);

            for (int i = 0; i < height; i++) {
                // Write row to outfile
                fwrite(image_mat[i], sizeof(RGB), width, output);

                // Write padding at end of row
                for (int k = 0; k < padding; k++) {
                    fputc(0x00, output);
                }
            }

            free(image_mat);
            // Close files
            if (!email_input) {free(file_name); }
            fclose(image);
            fclose(output);
            exit(0);
            break;
        }
        case 'J':
        {
            unsigned char *pixels = load_jpeg_image2(file_name, &width, &height, &channels);
            //making it into our RGB format
            RGB(*image_mat)[width] = calloc(height, width * sizeof(RGB));
            if (image_mat == NULL) {
                fprintf(logfile,"Not enough memory to store image.\n");
                fclose(image);
                exit(10);
            }

            int i=0;
            for (int j=0;j< height;j++){
                for (int k=0; k< width;k++){
                    image_mat[j][k].Red = pixels[i++];
                    image_mat[j][k].Green = pixels[i++];
                    image_mat[j][k].Blue = pixels[i++];
                    if (channels == 4) { i++; } //if the image was RGBA instead of RGB (it seems that JPEG can not be RGBA)
                }
            }
            //printf("FIlter NUM:::%i\n",index+1);
            filterapplication(index+1,height, width, image_mat, 0, channels, NULL);
            //printf("FILTER APPLIED!");
            i=0;
            for (int j=0;j< height;j++){
                for (int k=0; k< width;k++){
                    pixels[i++] = image_mat[j][k].Red;
                    pixels[i++] = image_mat[j][k].Green;
                    pixels[i++] = image_mat[j][k].Blue;
                    if (channels == 4) { i++; } //if the image was RGBA instead of RGB (it seems that JPEG can not be RGBA)
                }
            }

            write_jpeg_image("OutPic.jpg", width, height, channels, pixels);
            free(pixels);
            free(image_mat);
            if (!email_input) {free(file_name); }
            fclose(image);
            exit(0);
            break;
        }
        case 'P':
        {
            //in avali kar nakard bara bazi image ha
            //unsigned char *pixels = load_png(file_name, &width, &height, &channels);
            unsigned char *pixels = stbi_load(file_name, &width, &height, &channels, 0);
            //making it into our RGB format
            RGB(*image_mat)[width] = calloc(height, width * sizeof(RGB));
            if (image_mat == NULL) {
                fprintf(logfile,"Not enough memory to store image.\n");
                fclose(image);
                exit(11);
            }

            int i = 0;
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    image_mat[j][k].Red = pixels[i++];
                    image_mat[j][k].Green = pixels[i++];
                    image_mat[j][k].Blue = pixels[i++];
                    if (channels == 4) { i++; } //value alpha ro skip mikonim
                }
            }

            filterapplication(index+1,height, width, image_mat, 1, channels, pixels);

            i = 0;
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    pixels[i++] = image_mat[j][k].Red;
                    pixels[i++] = image_mat[j][k].Green;
                    pixels[i++] = image_mat[j][k].Blue;
                    if (channels == 4) { i++; } //value alpha e pixel ro taghir nemidim
                }
            }
            //oon chizi ke akhar migire in png_write function e mishe width*channels
            write_png("OutPic.png", pixels, width, height, channels);
            free(pixels);
            free(image_mat);
            if (!email_input) {free(file_name); }
            fclose(image);
            exit(0);
            break;
        }

        default:
            break;
    }
/////////////////////
        break;
        exit(0);
        }
    }
    }
}

void filter_button_clicked(GtkButton * button, gpointer data){
    invalidvalue = 0;
    char * filtername = gtk_button_get_label(button);
    
    strcpy(latestfiltername ,gtk_button_get_label(button));
    fprintf(logfile,"Setting latestfiltername : %s",latestfiltername);
    if (!strcmp(filtername,filterlist[0])){
        filteredfilename = imagelist[0];
        globalcase = 1;
        enteredfilter = 0;
    }
    else {
    globalcase = 0;
    if (!strcmp(filtername, filterlist[1])){
        filteredfilename = imagelist[1];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[2])){
        filteredfilename = imagelist[2];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[3])){
        filteredfilename = imagelist[3];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[4])){
        filteredfilename = imagelist[4];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[5])){
        filteredfilename = imagelist[5];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[6])){
        filteredfilename = imagelist[6];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[7])){
        filteredfilename = imagelist[7];
        globalcase = 8;
        enteredfilter = 0;
    }
    else {
        globalcase=0;
        if (!strcmp(filtername, filterlist[8])){
        filteredfilename = imagelist[8];
        globalcase=9;
        enteredfilter = 0;
        }
    else {
        globalcase=0;
    if (!strcmp(filtername, filterlist[9])){
        filteredfilename = imagelist[9];
        globalcase = 10;
        enteredfilter = 0;
    }
    else
    {
    globalcase = 0;
    if (!strcmp(filtername, filterlist[10])){
        filteredfilename = imagelist[10];
        globalcase = 11;
        enteredfilter = 0;
    }
    else 
    {
        globalcase=0;
        if (!strcmp(filtername, filterlist[11])){
        filteredfilename = imagelist[11];
        globalcase = 12;
        enteredfilter = 0;
    }
    else {
    globalcase = 0;
    if(!strcmp(filtername, filterlist[12])){
        filteredfilename = imagelist[12];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[13])){
        filteredfilename = imagelist[13];
        enteredfilter = 1;
    }
    else if (!strcmp(filtername, filterlist[14])){
        globalcase = 15;
        filteredfilename = imagelist[14];
        enteredfilter = 0;
    }
    else{
        globalcase = 0;
    }
    }
    }
    }
    }
    }
    }
    gtk_widget_destroy(outerbox);
    filter_page(window);

}

void on_text_entered_url(GtkEntry *entry, gpointer user_data) {
    const gchar *link = gtk_entry_get_text(entry);
    fprintf(logfile,"Input: %s\n", link);
    char *domain = malloc(1024);
    char *path = malloc(1024);
    if (domain == NULL) {exit(2);}
    if(path==NULL){exit(2);}
    sscanf(link,"https://%[^/]%s",domain,path);
    download_url(domain,path);
    if (urlerror){
        gtk_widget_destroy(outerbox);
        get_by_url(window);
        return;
    }
    FILE * httpsresponse = fopen("response.txt","rb");
    if (!httpsresponse){exit(3);}
    FILE * tmp =formatchecker(httpsresponse) ;
    if (!tmp){
        urlerror=1;
        gtk_widget_destroy(outerbox);
        get_by_url(window);
        return;
    }
    fclose (tmp);

    image = fopen("downloadout.out","rb");
    strcpy(file_name,"downloadout.out");
    if(!image){
        urlerror=1;
        gtk_widget_destroy(outerbox);
        get_by_url(window);
        return;
    }
    free(domain);
    free(path);

    gtk_widget_destroy(outerbox);
    //filter_page(window);
    pleasewait1(window);
}

void on_text_entered_secondpage(GtkEntry *entry, gpointer user_data) {
    
    strcpy(file_name,gtk_entry_get_text(entry));
    g_print("Input: %s\n", file_name);
    image = fopen(file_name,"rb");
    if (!image){
        imagefilenotfound=1;
        gtk_widget_destroy(outerbox);
        second_page(window);
        
    }
    else {
        gtk_widget_destroy(outerbox);
        //filter_page(window);
        pleasewait1(window);
    }
    //printf("IMAGE POS UPON OPENING : %p",(void*)image);
}

void approve_button_clicked(GtkWidget * button, gpointer data){
    /*
    approvedemail=1;
    gtk_widget_destroy(outerbox);
    get_by_email(window);
    email_input = 1;
    */

    char *email_username = "drprojepop@gmail.com";
    char *email_pass = "tpakkywaitldpmrf";
    char *server = "imaps://imap.gmail.com";
    strcpy(file_name, "decoded_image.jpg");
    if (downloadLastEmailAttachment(server, email_username, email_pass)){
        image = fopen(file_name, "rb");
    }
    if (!image){
        emailfailed = 1;
        gtk_widget_destroy(outerbox);
        get_by_email(window);
    }

    else {
    g_print("Approved!");
    gtk_widget_destroy(outerbox);
    //filter_page(window);
    pleasewait1(window);
    }
}

void prev_button_clicked(GtkWidget *button, gpointer data){
    gtk_widget_destroy(outerbox);
    first_page(window);
}

void button_clicked1(GtkWidget *button1, gpointer data) {
    
    //printf("calling\n");
    gtk_widget_destroy(outerbox); 
    second_page(window);
}

void button_clicked2(GtkWidget *button1, gpointer data) {
    //g_print("Button clicked: %s\n", gtk_button_get_label(button));
    gtk_widget_destroy(outerbox); 
    get_by_url(window);
}

void button_clicked3(GtkWidget *button1, gpointer data) {
    //g_print("Button clicked: %s\n", gtk_button_get_label(button));
    gtk_widget_destroy(outerbox); 
    get_by_email(window);
}

void filter_page(GtkWidget * window){

    outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), outerbox);
    //printf("works\n");

    GtkWidget *bottombox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_end(GTK_BOX(outerbox),bottombox,FALSE,FALSE,0);
    GtkWidget *creditlabel = gtk_label_new("Project by Ilya Atashsoda and Amirali Kazeroni");
    //printf("wworks2\n");

    GtkWidget *leftcornerlabelbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(leftcornerlabelbox),creditlabel,FALSE, FALSE,0);
    gtk_widget_set_halign(creditlabel, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(bottombox),leftcornerlabelbox , FALSE, FALSE, 0);
    //printf("printf3\n");
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_box_pack_start(GTK_BOX(outerbox), grid, FALSE, FALSE, 0);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    GError *error = NULL;
    GdkPixbuf *pix = gdk_pixbuf_new_from_file (filteredfilename, &error);
    if (pix == NULL) {
    g_printerr ("Error loading file: #%d %s\n", error->code, error->message);
    g_error_free (error);
    exit (1);
    }   
    GtkWidget *imagewidget = gtk_image_new_from_pixbuf (pix);
    gtk_image_set_from_pixbuf(
	    GTK_IMAGE(imagewidget),
	    gdk_pixbuf_scale_simple(pix,600,400,GDK_INTERP_BILINEAR));

    GtkWidget *imagebox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start(GTK_BOX(imagebox),imagewidget,FALSE,FALSE,0);
    //gtk_widget_set_size_request(imagebox,500,400);
    gtk_grid_attach(GTK_GRID(grid),imagebox,0,1,1,1);
    GtkWidget * label0 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid),label0,0,0,1,1);
    gtk_widget_set_size_request(label0, 400, 30);
    
    GtkWidget * apbox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_widget_set_size_request(apbox, 100, 50);
    GtkWidget *applybutton = gtk_button_new_with_label("Apply");
    gtk_widget_set_size_request(applybutton, 100, 50);
    g_signal_connect(applybutton, "clicked", G_CALLBACK(applyfilterbuttonclicked), NULL);

    gtk_widget_set_halign(apbox, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(apbox),applybutton,FALSE,FALSE,0);
    gtk_grid_attach(GTK_GRID(grid),apbox,0,3,1,1);

    GtkWidget *barbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_grid_attach(GTK_GRID(grid),barbox,0,2,1,1);
    GtkWidget *bargrid = gtk_grid_new();
    gtk_widget_set_halign(barbox, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing(GTK_GRID(bargrid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(bargrid), 5);
    gtk_box_pack_start(GTK_BOX(barbox),bargrid,FALSE,FALSE,0);

    
    GtkWidget *leftbar = gtk_button_new_with_label("<<");
    gtk_widget_set_size_request(leftbar, 10, 30);
    g_signal_connect(leftbar, "clicked", G_CALLBACK(moveindexleft), NULL);

    GtkWidget *firstoption = gtk_button_new_with_label(filterlist[showindex]);
    gtk_widget_set_size_request(firstoption, 150, 30);
    g_signal_connect(firstoption, "clicked", G_CALLBACK(filter_button_clicked), NULL);

    GtkWidget *secondoption = gtk_button_new_with_label(filterlist[showindex+1]);
    gtk_widget_set_size_request(secondoption, 150, 30);
    g_signal_connect(secondoption, "clicked", G_CALLBACK(filter_button_clicked), NULL);

    GtkWidget *thirdoption = gtk_button_new_with_label(filterlist[showindex+2]);
    gtk_widget_set_size_request(thirdoption, 150, 30);
    g_signal_connect(thirdoption, "clicked", G_CALLBACK(filter_button_clicked), NULL);

    GtkWidget *rightbar = gtk_button_new_with_label(">>");
    gtk_widget_set_size_request(rightbar, 10, 30);
    g_signal_connect(rightbar, "clicked", G_CALLBACK(moveindexright), NULL);

    gtk_grid_attach(GTK_GRID(bargrid),leftbar,0,0,1,1);
    gtk_grid_attach(GTK_GRID(bargrid),firstoption,1,0,1,1);
    gtk_grid_attach(GTK_GRID(bargrid),secondoption,2,0,1,1);
    gtk_grid_attach(GTK_GRID(bargrid),thirdoption,3,0,1,1);
    gtk_grid_attach(GTK_GRID(bargrid),rightbar,4,0,1,1);

    if (globalcase == 1){
        GtkWidget * label01 = gtk_label_new("Enter a value between 2 and 50, press ENTER and Apply:");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);

        GtkWidget *entry = gtk_entry_new();
        g_signal_connect(entry, "activate", G_CALLBACK(case1entryfunc), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry,0,5,1,1);
        gtk_widget_set_size_request(entry,20,10);
        //printf("works5\n");
        if (invalidvalue){
            GtkWidget * label02 = gtk_label_new("Invalid Input!");
            gtk_grid_attach(GTK_GRID(grid),label02,0,6,1,1);
        }
    }
    else if (globalcase == 8){
        GtkWidget * label01 = gtk_label_new("Enter a value between 2 and 5, press ENTER and Apply:");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);

        GtkWidget *entry = gtk_entry_new();
        g_signal_connect(entry, "activate", G_CALLBACK(case8entryfunc), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry,0,5,1,1);
        gtk_widget_set_size_request(entry,20,10);
        //printf("works5\n");

        if (invalidvalue){
            GtkWidget * label02 = gtk_label_new("Invalid Input!");
            gtk_grid_attach(GTK_GRID(grid),label02,0,6,1,1);
        }
    }
    else if (globalcase == 9){
        GtkWidget * label01 = gtk_label_new("What your desired intensity?(0 won't make a difference) press ENTER and Apply");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);

        GtkWidget *entry = gtk_entry_new();
        g_signal_connect(entry, "activate", G_CALLBACK(case9entryfunc), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry,0,5,1,1);
        gtk_widget_set_size_request(entry,20,10);
        //printf("works5\n");
    }
    else if (globalcase == 10){
        GtkWidget * label01 = gtk_label_new("Enter an intensity level between 1 & 1000 and Press Enter");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);

        GtkWidget *entry1 = gtk_entry_new();
        g_signal_connect(entry1, "activate", G_CALLBACK(case10entryfunc1), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry1,0,5,1,1);
        gtk_widget_set_size_request(entry1,20,10);
        //printf("works5\n");
        GtkWidget * label02 = gtk_label_new("Enter a threshhold level between 1 & 255 and Press Enter and Apply");
        gtk_grid_attach(GTK_GRID(grid),label02,0,6,1,1);

        GtkWidget *entry2 = gtk_entry_new();
        g_signal_connect(entry2, "activate", G_CALLBACK(case10entryfunc2), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry2,0,7,1,1);
        gtk_widget_set_size_request(entry2,20,10);
        if (invalidvalue || invalidvalue1 || invalidvalue2){
            GtkWidget * label04 = gtk_label_new("Invalid Input!");
            gtk_grid_attach(GTK_GRID(grid),label04,0,8,1,1);
        }

        //printf("works5\n");
    }
    else if (globalcase == 11){
        GtkWidget * label01 = gtk_label_new("Enter an rgb value that you wish to keep (R,G,B); press Enter and Apply.");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);

        GtkWidget *entry = gtk_entry_new();
        g_signal_connect(entry, "activate", G_CALLBACK(case11entryfunc), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry,0,5,1,1);
        gtk_widget_set_size_request(entry,20,10);
        //printf("works5\n");
        if (invalidvalue){
            GtkWidget * label02 = gtk_label_new("Invalid Input!");
            gtk_grid_attach(GTK_GRID(grid),label02,0,6,1,1);
        }
    }
    else if (globalcase == 12){
        GtkWidget * label01 = gtk_label_new("A: replaces Green with Red       B: replaces Blue with Red\n");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);
        GtkWidget * label02 = gtk_label_new("C: replaces Green with Blue      D: replaces Red with Blue\n");
        gtk_grid_attach(GTK_GRID(grid),label02,0,5,1,1);
        GtkWidget * label03 = gtk_label_new("E: replaces Blue with Green      F: replaces Red with Green\n");
        gtk_grid_attach(GTK_GRID(grid),label03,0,6,1,1);
        GtkWidget * label04 = gtk_label_new("Which version(A,B,C,D,E,F) of the filter do you want to apply? Press Enter and Apply.");
        gtk_grid_attach(GTK_GRID(grid),label04,0,7,1,1);

        GtkWidget *entry = gtk_entry_new();
        g_signal_connect(entry, "activate", G_CALLBACK(case12entryfunc), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry,0,8,1,1);
        gtk_widget_set_size_request(entry,20,10);
        //printf("works5\n");
        if (invalidvalue){
            GtkWidget * label05 = gtk_label_new("Invalid Input!");
            gtk_grid_attach(GTK_GRID(grid),label05,0,9,1,1);
        }
    }

    else if (globalcase == 15){
        GtkWidget * label01 = gtk_label_new("Do you want the filter to work with Minimum or Maximum values? Enter Max or Min and then Apply.");
        gtk_grid_attach(GTK_GRID(grid),label01,0,4,1,1);

        GtkWidget *entry = gtk_entry_new();
        g_signal_connect(entry, "activate", G_CALLBACK(case15entryfunc), NULL);
        gtk_grid_attach(GTK_GRID(grid),entry,0,5,1,1);
        gtk_widget_set_size_request(entry,20,10);
        //printf("works5\n");
        if (invalidvalue){
            GtkWidget * label02 = gtk_label_new("Invalid Input!");
            gtk_grid_attach(GTK_GRID(grid),label02,0,6,1,1);
        }
    }
    gtk_widget_show_all(window);

}

void get_by_url(GtkWidget * window)
{

    // Create a vertical box container
    // GtkWidget *outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), outerbox);
    //printf("works\n");

    GtkWidget *bottombox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_end(GTK_BOX(outerbox),bottombox,FALSE,FALSE,0);
    GtkWidget *creditlabel = gtk_label_new("Project by Ilya Atashsoda and Amirali Kazeroni");
    //printf("wworks2\n");

    GtkWidget *leftcornerlabelbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(leftcornerlabelbox),creditlabel,FALSE, FALSE,0);
    gtk_widget_set_halign(creditlabel, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(bottombox),leftcornerlabelbox , FALSE, FALSE, 0);
    //printf("printf3\n");

    GtkWidget *previousbutton = gtk_button_new_with_label("< Go Back");
    gtk_widget_set_size_request(previousbutton, 200, 30);
    g_signal_connect(previousbutton, "clicked", G_CALLBACK(prev_button_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(bottombox),previousbutton,FALSE, FALSE,0);
    //printf("works4\n");

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_box_pack_start(GTK_BOX(outerbox), grid, FALSE, FALSE, 0);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    //printf("works4\n");

    // Create the text box
    GtkWidget *entry = gtk_entry_new();
    g_signal_connect(entry, "activate", G_CALLBACK(on_text_entered_url), NULL);
    //printf("works5\n");

    GtkWidget * label = gtk_label_new("Enter url (Only sites that support secure connection are supported):");
    GtkWidget * label2 = gtk_label_new("eg. https://www.example.com/image.jpg");
    GtkWidget * label0 = gtk_label_new("");
    GtkWidget * label01 = gtk_label_new("");
    //printf("works6");

    
    gtk_grid_attach(GTK_GRID(grid),label0,0,0,1,1);
    gtk_widget_set_size_request(label0, 400, 30);
    
    gtk_grid_attach(GTK_GRID(grid),label01,0,1,1,1);
    gtk_widget_set_size_request(label, 400, 30);

    
    gtk_grid_attach(GTK_GRID(grid),label,0,2,1,1);
    gtk_widget_set_size_request(label, 600, 30);

    gtk_grid_attach(GTK_GRID(grid),label2,0,3,1,1);
    gtk_widget_set_size_request(label2, 600, 30);

    gtk_grid_attach(GTK_GRID(grid), entry, 0, 4, 1, 1);
    gtk_widget_set_size_request(entry, 500, 30);

    if (urlerror){
        GtkWidget * label02 = gtk_label_new("Invalid url or unsupported file format!");
        gtk_grid_attach(GTK_GRID(grid), label02, 0, 5, 1, 1);
        gtk_widget_set_size_request(label02, 500, 30);
    }
    //printf("works7\n");

    // Show all the widgets
    
    gtk_widget_show_all(window);
    
}

void get_by_email(GtkWidget * window)
{

    // Create a vertical box container
    // GtkWidget *outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), outerbox);
    //printf("works\n");

    GtkWidget *bottombox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_end(GTK_BOX(outerbox),bottombox,FALSE,FALSE,0);
    GtkWidget *creditlabel = gtk_label_new("Project by Ilya Atashsoda and Amirali Kazeroni");
    //printf("wworks2\n");

    GtkWidget *leftcornerlabelbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(leftcornerlabelbox),creditlabel,FALSE, FALSE,0);
    gtk_widget_set_halign(creditlabel, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(bottombox),leftcornerlabelbox , FALSE, FALSE, 0);
    //printf("printf3\n");

    GtkWidget *previousbutton = gtk_button_new_with_label("< Go Back");
    gtk_widget_set_size_request(previousbutton, 200, 30);
    g_signal_connect(previousbutton, "clicked", G_CALLBACK(prev_button_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(bottombox),previousbutton,FALSE, FALSE,0);
    //printf("works4\n");

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_box_pack_start(GTK_BOX(outerbox), grid, FALSE, FALSE, 0);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    //printf("works4\n");
    GtkWidget * approvebutton = gtk_button_new_with_label("Email Sent");
    gtk_widget_set_size_request(approvebutton,200,30);
    g_signal_connect(approvebutton,"clicked",G_CALLBACK(approve_button_clicked),NULL);
    
    // Create the text box
    
    GtkWidget * label = gtk_label_new("Send your image file to the email below: #bos_be_kazeron");
    GtkWidget * label2 = gtk_label_new("drprojepop@gmail.com");
    GtkWidget * label0 = gtk_label_new("");
    GtkWidget * label01 = gtk_label_new("");
    if (approvedemail){
        GtkWidget * label03 = gtk_label_new("PLEASE WAIT!");
        gtk_grid_attach(GTK_GRID(grid),label03,0,5,1,1);
        gtk_widget_set_size_request(label03, 600, 30);
    }

    if (emailfailed){
        GtkWidget * label02 = gtk_label_new("Image file could not be received! Try Again Later!");
        gtk_grid_attach(GTK_GRID(grid),label02,0,4,1,1);
        gtk_widget_set_size_request(label02, 600, 30);
    }
    //printf("works6");

    
    gtk_grid_attach(GTK_GRID(grid),label0,0,0,1,1);
    gtk_widget_set_size_request(label0, 400, 30);
    
    gtk_grid_attach(GTK_GRID(grid),label01,0,1,1,1);
    gtk_widget_set_size_request(label01, 400, 30);

    gtk_grid_attach(GTK_GRID(grid),approvebutton,0,4,1,1);
    gtk_widget_set_size_request(approvebutton, 400, 30);
    
    gtk_grid_attach(GTK_GRID(grid),label,0,2,1,1);
    gtk_widget_set_size_request(label, 600, 30);

    gtk_grid_attach(GTK_GRID(grid),label2,0,3,1,1);
    gtk_widget_set_size_request(label2, 600, 30);

    //gtk_grid_attach(GTK_GRID(grid), entry, 0, 4, 1, 1);
    //gtk_widget_set_size_request(entry, 500, 30);
    //printf("works7\n");

    // Show all the widgets
    
    gtk_widget_show_all(window);
}

void second_page(GtkWidget * window){
    // Create a vertical box container
    // GtkWidget *outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), outerbox);
    //printf("works\n");

    GtkWidget *bottombox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_end(GTK_BOX(outerbox),bottombox,FALSE,FALSE,0);
    GtkWidget *creditlabel = gtk_label_new("Project by Ilya Atashsoda and Amirali Kazeroni");
    //printf("wworks2\n");

    GtkWidget *leftcornerlabelbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(leftcornerlabelbox),creditlabel,FALSE, FALSE,0);
    gtk_widget_set_halign(creditlabel, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(bottombox),leftcornerlabelbox , FALSE, FALSE, 0);
    //printf("printf3\n");

    GtkWidget *previousbutton = gtk_button_new_with_label("< Go Back");
    gtk_widget_set_size_request(previousbutton, 200, 30);
    g_signal_connect(previousbutton, "clicked", G_CALLBACK(prev_button_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(bottombox),previousbutton,FALSE, FALSE,0);
    //printf("works4\n");

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_box_pack_start(GTK_BOX(outerbox), grid, FALSE, FALSE, 0);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    //printf("works4\n");

    // Create the text box
    GtkWidget *entry = gtk_entry_new();
    g_signal_connect(entry, "activate", G_CALLBACK(on_text_entered_secondpage), NULL);
    //printf("works5\n");

    GtkWidget * label = gtk_label_new("Enter file name with extension:");
    GtkWidget * label2 = gtk_label_new("eg. picture.jpg");
    GtkWidget * label0 = gtk_label_new("");
    GtkWidget * label01 = gtk_label_new("");
    if (imagefilenotfound){
        imagefilenotfound=0;
        GtkWidget * label02 = gtk_label_new("image file not found! Enter a valid file name!");
        gtk_grid_attach(GTK_GRID(grid),label02,0,5,1,1);
        gtk_widget_set_size_request(label02, 500, 30);
    }
    //printf("works6");

    
    gtk_grid_attach(GTK_GRID(grid),label0,0,0,1,1);
    gtk_widget_set_size_request(label0, 400, 30);
    
    gtk_grid_attach(GTK_GRID(grid),label01,0,1,1,1);
    gtk_widget_set_size_request(label, 400, 30);

    
    gtk_grid_attach(GTK_GRID(grid),label,0,2,1,1);
    gtk_widget_set_size_request(label, 600, 30);

    gtk_grid_attach(GTK_GRID(grid),label2,0,3,1,1);
    gtk_widget_set_size_request(label2, 600, 30);

    gtk_grid_attach(GTK_GRID(grid), entry, 0, 4, 1, 1);
    gtk_widget_set_size_request(entry, 500, 30);


    //printf("works7\n");


    // Show all the widgets
    
    gtk_widget_show_all(window);
    
}

void first_page(GtkWidget *window){
    //printf("hello\n");
    
    GtkWidget *label;
    GtkWidget *vbox;
    GtkWidget *label2;
    GtkWidget *cornerlabelbox;
    GtkWidget *bottombox;

    // Create a grid container
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 40);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    
    //printf("button\n");
    // Create the buttons
    
    GtkWidget *button1= gtk_button_new_with_label("input by File name");
    
    GtkWidget *button2 = gtk_button_new_with_label("  input by download link  ");
    
    GtkWidget *button3 = gtk_button_new_with_label("  input by sending email  ");
    //printf("button 2\n");

    // Set smaller sizes for the buttons
    gtk_widget_set_size_request(button1, 200, 50);
    gtk_widget_set_size_request(button2, 200, 50);
    gtk_widget_set_size_request(button3, 200, 50);
    // gtk_widget_set_size_request(button4, 200, 30);

    // Connect the "clicked" signal of the buttons to a callback function

    //g_signal_connect(button4, "clicked", G_CALLBACK(button_clicked), NULL);
    // Attach the buttons to the grid cells

    //printf("hello3\n");
    gtk_grid_attach(GTK_GRID(grid), button1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button2, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button3, 0, 2, 1, 1);
    // gtk_grid_attach(GTK_GRID(grid), button4, 1, 0, 1, 1);

    // Set the grid container to center-align its contents
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    label = gtk_label_new("Choose your input method : ");
    GtkWidget * label01 = gtk_label_new("");
    GtkWidget * label02 = gtk_label_new("");

    outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    // Create a vertical box container for the label
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 60);
    gtk_box_pack_start(GTK_BOX(outerbox),vbox,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox), label01, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label02, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window),outerbox);
    

    label2 = gtk_label_new("Project by Ilya Atashsoda and Amirali Kazeroni");

    cornerlabelbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(cornerlabelbox),label2,FALSE, FALSE,0);
    gtk_widget_set_halign(label2, GTK_ALIGN_CENTER);
    bottombox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_end(GTK_BOX(outerbox),bottombox , FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bottombox),cornerlabelbox , FALSE, FALSE, 0);
    //printf("heloow");
    g_signal_connect(button1, "clicked", G_CALLBACK(button_clicked1), NULL);
    g_signal_connect(button2, "clicked", G_CALLBACK(button_clicked2), NULL);
    g_signal_connect(button3, "clicked", G_CALLBACK(button_clicked3), NULL);
    // Show all the widgets
    gtk_widget_show_all(window);

}

void pleasewait1(GtkWidget * window)
{

    // Create a vertical box container
    // GtkWidget *outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    outerbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), outerbox);
    //printf("works\n");

    GtkWidget *bottombox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_end(GTK_BOX(outerbox),bottombox,FALSE,FALSE,0);
    GtkWidget *creditlabel = gtk_label_new("Project by Ilya Atashsoda and Amirali Kazeroni");
    //printf("wworks2\n");

    GtkWidget *leftcornerlabelbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(leftcornerlabelbox),creditlabel,FALSE, FALSE,0);
    gtk_widget_set_halign(creditlabel, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(bottombox),leftcornerlabelbox , FALSE, FALSE, 0);
    //printf("printf3\n");

    //GtkWidget *previousbutton = gtk_button_new_with_label("< Go Back");
    //gtk_widget_set_size_request(previousbutton, 200, 30);
    //g_signal_connect(previousbutton, "clicked", G_CALLBACK(prev_button_clicked), NULL);
    //gtk_box_pack_end(GTK_BOX(bottombox),previousbutton,FALSE, FALSE,0);
    //printf("works4\n");

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_box_pack_start(GTK_BOX(outerbox), grid, FALSE, FALSE, 0);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    //printf("works4\n");
    //GtkWidget * approvebutton = gtk_button_new_with_label("Email Sent");
    //gtk_widget_set_size_request(approvebutton,200,30);
    //g_signal_connect(approvebutton,"clicked",G_CALLBACK(approve_button_clicked),NULL);
    
    // Create the text box
    
    GtkWidget * label = gtk_label_new("");
    GtkWidget * label2 = gtk_label_new("");
    GtkWidget * label0 = gtk_label_new("");
    GtkWidget * label01 = gtk_label_new("Please Wait :)");

    //printf("works6");

    
    gtk_grid_attach(GTK_GRID(grid),label0,0,0,1,1);
    gtk_widget_set_size_request(label0, 400, 30);
    
    gtk_grid_attach(GTK_GRID(grid),label01,0,1,1,1);
    gtk_widget_set_size_request(label01, 400, 30);

    //gtk_grid_attach(GTK_GRID(grid),approvebutton,0,4,1,1);
    //gtk_widget_set_size_request(approvebutton, 400, 30);
    
    gtk_grid_attach(GTK_GRID(grid),label,0,2,1,1);
    gtk_widget_set_size_request(label, 600, 30);

    gtk_grid_attach(GTK_GRID(grid),label2,0,3,1,1);
    gtk_widget_set_size_request(label2, 600, 30);

    //gtk_grid_attach(GTK_GRID(grid), entry, 0, 4, 1, 1);
    //gtk_widget_set_size_request(entry, 500, 30);
    //printf("works7\n");

    // Show all the widgets
    if (invalidformat){
        GtkWidget *failedbutton= gtk_button_new_with_label("GO BACK TO FIRST PAGE");
        gtk_grid_attach(GTK_GRID(grid),failedbutton,0,4,1,1);
        gtk_widget_set_size_request(failedbutton, 600, 30);
        g_signal_connect(failedbutton, "clicked", G_CALLBACK(prev_button_clicked), NULL);
        GtkWidget * labelfailed = gtk_label_new("Invalid Format");
        gtk_grid_attach(GTK_GRID(grid),labelfailed,0,5,1,1);
        gtk_widget_set_size_request(labelfailed, 400, 30);
        invalidformat=0;
        gtk_widget_show_all(window);
    }
    
    

    else {
        gtk_widget_show_all(window);
        
    // Checking whether the image was saved to memory correctly or not:
    if (image == NULL)
    {
        fprintf(logfile,"Image file could not be received !\n");
        fclose(image);
        exit(4);
    }



    // reading size of a BMP header to check if it is actually BMP:
    
    if (fread(&myheader, sizeof(BMPHeader), 1, image) != 1)
    {
        fprintf(logfile,"Failed to read file's header\n");
        fclose(image);
        exit(5);
    }

    //Image variables that we'll work with in all cases:




    //returning to the start of the file for get_image_format
    fseek(image, 0, SEEK_SET);


    globalformat = is_bmp(&myheader,image);
    fseek(image, 0, SEEK_SET);

    switch (globalformat) {
        /*
        case 'B': {
            // returning to the first of the file because of some unusual BMPs
            fseek(image, 0, SEEK_SET);

            // reading BMP header
            if (fread(&header, sizeof(BMPHeader), 1, image) != 1) {
                printf("Failed to read BMP header\n");
                fclose(image);
                exit(6);
            }

            // returning to the first of the file because of some unusual BMPs
            fseek(image, 0, SEEK_SET);

            BYTE header_towrite[header.bfOffBits];
            if (fread(&header_towrite, header.bfOffBits, 1, image) != 1) {
                printf("hamoo erore\n");
                fclose(image);
                exit(7);
            }

            printf("\n\noffset:%i  bitdepth:%i  headersize:%i\n\n", header.bfOffBits, header.biBitCount, header.biSize);


            // Get image's dimensions
            height = abs(header.biHeight);
            width = header.biWidth;
            channels = header.biBitCount;

            // go to the start of image data(data array)
            fseek(image, header.bfOffBits, SEEK_SET);


            // Allocate memory for image
            RGB (*image_mat)[width] = calloc(height, width * sizeof(RGB));

            if (image_mat == NULL) {
                printf("Not enough memory to store image.\n");
                fclose(image);
                exit(8);
            }


            // Determine padding for scanlines
            int padding = (4 - (width * sizeof(RGB)) % 4) % 4;

            // Iterate over infile's scanlines
            for (int i = 0; i < height; i++) {
                // Read row into pixel array
                fread(image_mat[i], sizeof(RGB), width, image);

                // Skip over padding
                fseek(image, padding, SEEK_CUR);
            }

            //filterapplication(height, width, image_mat, 0, channels, NULL);
            filter_page(window);
            // Write new pixels to outfile
            FILE *output = fopen("OutPic.bmp", "wb");
            if (output == NULL) {
                printf("Could not write out the output file\n");
                exit(9);
            }

            // first writing the BMP header
            fwrite(&header_towrite, header.bfOffBits, 1, output);

            for (int i = 0; i < height; i++) {
                // Write row to outfile
                fwrite(image_mat[i], sizeof(RGB), width, output);

                // Write padding at end of row
                for (int k = 0; k < padding; k++) {
                    fputc(0x00, output);
                }
            }

            free(image_mat);
            // Close files
            if (!email_input) {free(file_name); }
            fclose(image);
            fclose(output);
            break;
        }
        case 'J':
        {
            unsigned char *pixels = load_jpeg_image2(file_name, &width, &height, &channels);
            //making it into our RGB format
            RGB(*image_mat)[width] = calloc(height, width * sizeof(RGB));
            if (image_mat == NULL) {
                printf("Not enough memory to store image.\n");
                fclose(image);
                exit(10);
            }

            int i=0;
            for (int j=0;j< height;j++){
                for (int k=0; k< width;k++){
                    image_mat[j][k].Red = pixels[i++];
                    image_mat[j][k].Green = pixels[i++];
                    image_mat[j][k].Blue = pixels[i++];
                    if (channels == 4) { i++; } //if the image was RGBA instead of RGB (it seems that JPEG can not be RGBA)
                }
            }
            //filterapplication(height, width, image_mat, 0, channels, NULL);
            filter_page(window);
            i=0;
            for (int j=0;j< height;j++){
                for (int k=0; k< width;k++){
                    pixels[i++] = image_mat[j][k].Red;
                    pixels[i++] = image_mat[j][k].Green;
                    pixels[i++] = image_mat[j][k].Blue;
                    if (channels == 4) { i++; } //if the image was RGBA instead of RGB (it seems that JPEG can not be RGBA)
                }
            }

            write_jpeg_image("OutPic.jpg", width, height, channels, pixels);
            free(pixels);
            free(image_mat);
            if (!email_input) {free(file_name); }
            fclose(image);
            break;
        }
        case 'P':
        {
            //in avali kar nakard bara bazi image ha
            //unsigned char *pixels = load_png(file_name, &width, &height, &channels);
            unsigned char *pixels = stbi_load(file_name, &width, &height, &channels, 0);
            //making it into our RGB format
            RGB(*image_mat)[width] = calloc(height, width * sizeof(RGB));
            if (image_mat == NULL) {
                printf("Not enough memory to store image.\n");
                fclose(image);
                exit(11);
            }

            int i = 0;
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    image_mat[j][k].Red = pixels[i++];
                    image_mat[j][k].Green = pixels[i++];
                    image_mat[j][k].Blue = pixels[i++];
                    if (channels == 4) { i++; } //value alpha ro skip mikonim
                }
            }

            //filterapplication(height, width, image_mat, 1, channels, pixels);
            filter_page(window);
            i = 0;
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    pixels[i++] = image_mat[j][k].Red;
                    pixels[i++] = image_mat[j][k].Green;
                    pixels[i++] = image_mat[j][k].Blue;
                    if (channels == 4) { i++; } //value alpha e pixel ro taghir nemidim
                }
            }
            //oon chizi ke akhar migire in png_write function e mishe width*channels
            write_png("OutPic.png", pixels, width, height, channels);
            free(pixels);
            free(image_mat);
            if (!email_input) {free(file_name); }
            fclose(image);
            
            break;
        }
        */
        case 'X':
            invalidformat=1;
            gtk_widget_destroy(outerbox);
            pleasewait1(window);
            return ;
        default:
        gtk_widget_destroy(outerbox);
        filter_page(window);
        break;
    }
    }
}


void destroywindow(void){
    fprintf(logfile,"END OF LOGs\n");
    fclose(logfile);
    gtk_main_quit();
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    logfile = fopen("ProgramLogs.txt","wb");

    // Create the main window
    //GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    latestfiltername = malloc(1024);
    file_name = malloc(1024);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "AP Project");
    gtk_window_set_default_size(GTK_WINDOW(window),1290 , 800);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(destroywindow), NULL);
    
        
    first_page(window);
    gtk_main();
    
}



///////////////////////////////////////////////////////

//EMAIL FUNCTIONS:
char* find_attachment_UID(const char* url, const char* username, const char* password, int first_time) {
    CURL *curl;
    CURLcode res;
    char *res_uid = NULL;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_PORT, 993);
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(logfile, "Login failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return NULL;
        }

        //Alternative: const char *select_command = "SELECT \"[Gmail]/All Mail\"";
        const char *select_command = "SELECT INBOX";
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, select_command);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(logfile, "Selecting mailbox failed: %s (%d)\n", curl_easy_strerror(res), res);
            return NULL;
        } else {
            if (first_time) {fprintf(logfile,"\nINBOX selected successfully!\n");}

            // Modify fetch_command to fetch the entire body of the email
            const char *fetch_command = "SEARCH ALL";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, fetch_command);

            struct MemoryStruct chunk;
            chunk.memory = malloc(1);  // Start with empty memory
            chunk.size = 0;

            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_email);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                fprintf(logfile, "Fetching UID failed: %s (%d)\n", curl_easy_strerror(res), res);
                return NULL;
            } else {
                if (first_time) {fprintf(logfile,"UID fetched successfully!\n\n   Please Wait...\n\n");}
                // Access the result through chunk.memory
                // res_uid will be the same as chunk.memory
                res_uid = chunk.memory;
            }

        }

        curl_easy_cleanup(curl);
    }

    return res_uid;
}

    static size_t write_data_email(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        fprintf(logfile, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}


FILE* fetch_attachment(char *url, const char *username, const char *password){
    CURL *curl;
    CURLcode res;

    FILE *fp = fopen("fetched_data.txt", "wb");
    if (!fp) {
        fprintf(logfile,"Failed to create output file for base64 data\n");
        return NULL;
    }

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);


        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        res = curl_easy_perform(curl);



        if (res != CURLE_OK) {
            fprintf(logfile,"Failed to connect to server: %s\n", curl_easy_strerror(res));
            return NULL;
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

    return fopen("fetched_data.txt", "rb");
}


// Function to download the attachment of the last email in Gmail
int downloadLastEmailAttachment(const char* url, const char* username, const char* password) {
   int uid, uid_temp, past_uid, first_time = 1;
   int timer = 0;
   //finding UID of the last email
    char *uid_str = find_attachment_UID(url, username, password, first_time);
    if (uid_str == NULL) {return 0;}
    
    while ((uid_temp =  read_int(uid_str)) != 0){ // ba in while mirim akharin email ba in subject ro peyda mikonim
        uid = uid_temp;
    }
    past_uid = uid;
    //printf("%i,  %i\n", past_uid, uid);

   while(1){
        first_time = 0; // ke dige hye oon 3 khate please wait print nashe
        timer++; //for when the image has little size so the past_uid and uid are probably always the same 

        //finding UID of the last email
        char *uid_str = find_attachment_UID(url, username, password, first_time);
        if (uid_str == NULL) {return 0;}
    
        while ((uid_temp =  read_int(uid_str)) != 0){ // ba in while mirim akharin email ba in subject ro peyda mikonim
            uid = uid_temp;
        }

        if(past_uid != uid){
            //printf("%i,  %i\n", past_uid, uid);
            break;
        }

        
        if (timer == 300)
        {
            break;
        }
            
   }
   
    free(uid_str); // we no longer need it

    //making the url using the uid to fetch the attachment data from
    char *final_url = malloc(3000);
    sprintf(final_url, "imaps://imap.gmail.com/INBOX/;UID=%d/;SECTION=TEXT", uid);

    //fetching the base64 encoded data of the attachment from the server
    FILE *email = fetch_attachment(final_url, username, password);
    free(final_url); //we no longer need it
    if (email == NULL) {return 0;}

    //determing the file size for later use
    fseek(email, 0, SEEK_END);
    long file_size = ftell(email);
    fseek(email, 0 , SEEK_SET);

    //getting the base64 encoded data of just the attachment
    char buffer[1501];
    fread(buffer, 1, 1500, email);
    long where = last_index_of_substring(buffer, "Content");
    fseek(email, 0, SEEK_SET);// miri k az aval be andaze where bekhunim
    fread(buffer, 1, where, email);
    char c;
    while ((c = fgetc(email)) != '\n'){ // hala ham baz mikhunim ta beresim be '\n' va az baadesh data asli
        where++;
    }
    // checking if the sender is a gmail
    long temp_where = where;
    c = fgetc(email);
    if (c == 'X'){
        where++;
        while ((c = fgetc(email)) != '\n'){ // hala ham baz mikhunim ta beresim be '\n' va az baadesh data asli
            where++;
        }
    }
    else {
        where = temp_where;
    }
    // we want from this point on of the file


    long size_to_read = file_size - where;
    char *encoded_att = malloc(size_to_read + 1);
    fread(encoded_att, 1, size_to_read, email);
    encoded_att[size_to_read] = '\0';
    fclose(email);
/*
    //removing the final line of the txt
    //bazi email ha bade line akhar hm ye doone '\n daran bazia EOF dige in for e bara remove kardane oonaiee ke akharesh '\n' daran
    for (long i = size_to_read; i > 0; --i) {
        if (encoded_att[i] == '\n') {encoded_att[size_to_read-1] = '\0';}
        else if (encoded_att[i] != '\0') { break;}
    }
printf("\nsalam goli111\n");
    char *final_encoded_att = malloc(size_to_read + 1);
    int i, j, last_newline = 0;
     printf("\nsalam goli333\n");
    //first finding the last '\n' dar halati ke dige baade line akhare txt EOF e dige
    for (i = 0; i < strlen(encoded_att); ++i) {
        if (encoded_att[i] == '\n')
        {
            last_newline = i;
        }
    }
    printf("\nsalam goli222\n");
    //copying till before the last new line
    for (j = 0; j < last_newline+1; ++j) {
        final_encoded_att[j] = encoded_att[j];
    }
    final_encoded_att[j] = '\0';
    

*/

    //for testing where
    FILE *f = fopen("encoded_data.txt", "wb");
    if (!f) {
        fprintf(logfile,"Failed to open fetched data file for reading only attachment data\n");
        return 0;
    }
    //fwrite(final_encoded_att, 1, strlen(final_encoded_att), f);
    fwrite(encoded_att, 1, strlen(encoded_att), f);
    fclose(f);

    //now decoding
    //decode64(final_encoded_att);
    decode64(encoded_att);
    fprintf(logfile,"Attachment downloaded successfully!\n");
    return 1;

}


void decode64(const char* encoded_image) {
    base64_decodestate state;
    base64_init_decodestate(&state);

    // Calculate the size of the decoded image data
    size_t encoded_length = strlen(encoded_image);
    size_t decoded_length = 3 * encoded_length / 4;
    char* decoded_image = malloc(decoded_length);

    // Decode the image data
    size_t num_decoded = base64_decode_block(encoded_image, encoded_length, decoded_image, &state);

    // Save the decoded image data to a file
    FILE* file = fopen("decoded_image.jpg", "wb");
    fwrite(decoded_image, sizeof(char), num_decoded, file);
    fclose(file);

    // Free the memory used by the decoded image data
    free(decoded_image);
}

size_t discard_response(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    /* Discard the received data */
    return size * nmemb;
}

/* Callback function to write downloaded data to file */
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

int read_int(char *input){
    static int i = 0;
    int flag = 0;
    int m = 1; // sign
    int integer = 0;
    char c;
    while ((c = input[i++]) != '\0')
    {
        if (c == '+') { m = 1; continue;}
        if (c == '-') { m = -1; continue;}
        if (c <= '9' && c >= '0') { integer = 10 * integer + (c - '0'); flag = 1;}
        if (flag == 1 && (c == ' ' || c == '\n')) {break;}
    }
    return m * integer;
}

long last_index_of_substring(char* str, char* substr) {
    int str_length = strlen(str);
    int substr_length = strlen(substr);
    int i = str_length - substr_length;
    while (i >= 0) {
        if (strncmp(str + i, substr, substr_length) == 0) {
            return i;
        }
        i--;
    }
    return -1;
}

//END OF EMAIL FUNCTIONS

//URL FUNCTIONS

void download_url (char * SERVER, char * PATH){

    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    if (ctx == NULL) {
        fprintf(logfile,"Error creating SSL context\n");
        return;
    }

    SSL *ssl = SSL_new(ctx);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        fprintf(logfile,"Error opening socket\n");
        return;
    }

    server = gethostbyname(SERVER);

    if (server == NULL) {
        fprintf(logfile,"Error resolving server hostname\n");
        urlerror = 1;
        return;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(logfile,"Error connecting to server\n");
        urlerror = 1;
        return;
    }

    BIO *sbio = BIO_new_socket(sockfd, BIO_NOCLOSE);
    SSL_set_bio(ssl, sbio, sbio);
    if (SSL_set_tlsext_host_name(ssl, SERVER) != 1) {
        fprintf(logfile,"Error setting SNI\n");
        return;
    }
    if (SSL_connect(ssl) <= 0) {
        fprintf(logfile,"Error establishing SSL connection\n");
        return;
    }

    char request[1024];
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", PATH, SERVER);
    SSL_write(ssl, request, strlen(request));

    char response[1024];
    int bytes_read;
    int total_bytes_read = 0;
    FILE *fp = fopen("response.txt", "wb");

    if (fp == NULL) {
        fprintf(logfile,"Error opening file\n");
        return;
    }

    do {
        bytes_read = SSL_read(ssl, response, sizeof(response));
        //fprintf(logfile,"reading %i",bytes_read);

        if (bytes_read > 0) {
            //fprintf(logfile,"bytes read: %i\n",bytes_read);
            fwrite(response, 1, bytes_read, fp);
            total_bytes_read += bytes_read;
        }
    } while (bytes_read > 0);

    fclose(fp);

    fprintf(logfile,"Total bytes read: %d\n", total_bytes_read);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);
    urlerror=0;

}


FILE * formatchecker(FILE * fp)
{
    int filesize,max;
    char tmpbyte;
    fseek(fp,0L,SEEK_END);
    filesize = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    if (filesize > 3000){
        max=3000;
        for (int i = 0; i < max; i++)
        {
            fread(&tmpbyte,sizeof(char),1,fp);
            if (tmpbyte == '\n') 
            {
                fread(&tmpbyte, sizeof(char), 1, fp);
                if (tmpbyte == 'B') {
                    fread(&tmpbyte, sizeof(char), 1, fp);
                    if (tmpbyte == 'M') {
                        fread(&tmpbyte, sizeof(char), 1, fp);
                        if (tmpbyte != 'P') 
                        {
                            fseek(fp, 0L, SEEK_SET);
                            return parsebmp(fp);

                        }
                        else
                        {
                            fseek(fp,-2L,SEEK_CUR);
                        }
                    }
                    else {
                        fseek(fp,-1L,SEEK_CUR);
                    }
                }
            
                else if (memcmp(&tmpbyte,"\x89",1)==0){
                    char nextseven[7];
                    fread(nextseven,sizeof(char),7,fp);
                    if (memcmp(nextseven,"\x50\x4E\x47\x0D\x0A\x1A\x0A",7)==0){
                        fseek(fp,0L,SEEK_SET);
                        return parsepng(fp);

                    }
                    else{
                        fseek(fp,-7L,SEEK_CUR);
                    }
                }

                else if (memcmp(&tmpbyte,"\xFF",1)==0){
                    char nexttwo[2];
                    fread(nexttwo,sizeof(char),2,fp);
                    if (memcmp(nexttwo,"\xD8\xFF",2)==0){
                        fseek(fp,0L,SEEK_SET);
                        return parsejpg(fp);

                    }
                    else {
                        fseek(fp,-2L,SEEK_CUR);
                    }

                }
            }
        }
    }
    return NULL;
}

FILE * parsebmp(FILE * fp){
    char outfilename[] = "downloadout.out";
    FILE * outfile = fopen(outfilename, "wb");

    char tmpbyte;
    fseek(fp,0L,SEEK_END);
    int filesize = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    int write = 0;
    for (int i = 0; i < filesize; i++){
        fread(&tmpbyte,sizeof(char),1,fp);
        if (write)
        {
            fwrite(&tmpbyte,sizeof(char),1,outfile);
        }
        else
        {
            if (tmpbyte == '\n') {
                fread(&tmpbyte, sizeof(char), 1, fp);
                if (tmpbyte == 'B') {
                    fread(&tmpbyte, sizeof(char), 1, fp);
                    if (tmpbyte == 'M') {
                        fread(&tmpbyte, sizeof(char), 1, fp);
                        if (tmpbyte != 'P') {
                            fseek(fp, -3L, SEEK_CUR);
                            write = 1;
                            fprintf(logfile,"Writing BMP File\n");
                        }
                    }
                }
            }
        }
    }
    return outfile;
    fprintf(logfile,"Done!\n");
}

FILE * parsepng(FILE * fp){

    char outfilename[] = "downloadout.out";
    FILE * outfile = fopen(outfilename, "wb");
    char tmpbyte;
    fseek(fp,0L,SEEK_END);
    int filesize = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    int write = 0;
    for (int i = 0; i < filesize; i++)
    {
        fread(&tmpbyte,sizeof(char),1,fp);
        if (write)
        {
            fwrite(&tmpbyte,sizeof(char),1,outfile);
        }
        else if (memcmp(&tmpbyte,"\x89",1)==0)
        {
            char nextseven[7];
            fread(nextseven,sizeof(char),7,fp);
            if (memcmp(nextseven,"\x50\x4E\x47\x0D\x0A\x1A\x0A",7)==0){
                write = 1;
                fseek(fp,-8L,SEEK_CUR);
                fprintf(logfile,"Writing PNG File.\n");
            }
            else{
                fseek(fp,-7L,SEEK_CUR);
            }
        }
    }
    fprintf(logfile,"Done!\n");
    return outfile;
}

FILE * parsejpg(FILE * fp){


    char outfilename[] = "downloadout.out";
    FILE * outfile = fopen(outfilename, "wb");
    char tmpbyte;
    fseek(fp,0L,SEEK_END);
    int filesize = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    int write = 0;
    for (int i = 0; i < filesize; i++)
    {
        fread(&tmpbyte,sizeof(char),1,fp);
        if (write)
        {
            fwrite(&tmpbyte,sizeof(char),1,outfile);
        }
        else if (memcmp(&tmpbyte,"\xFF",1)==0)
        {
            char nexttwo[2];
            fread(nexttwo,sizeof(char),2,fp);
            if (memcmp(nexttwo,"\xD8\xFF",2)==0){
                write = 1;
                fseek(fp,-3L,SEEK_CUR);
                fprintf(logfile,"Writing JPEG File.\n");
            }
            else{
                fseek(fp,-2L,SEEK_CUR);
            }
        }
    }
    fprintf(logfile,"Done!\n");
    return outfile;
}
// END OF URL FUNCTIONS


char is_bmp(const BMPHeader *header, FILE *fp)
{
    int notbmp=0;
    // Check the signature
    if (header->bfType != 0x4D42)
    {
        notbmp = 1; // Not a BMP file
    }
    // Check the DIB header size
    /*if (header->dib_header_size != 40) {
        printf("2\n");
        return 0; // Unsupported DIB header size
    }*/
    // Check the compression type
    if (header->biCompression != 0 && header->biCompression != 3)
    {
        notbmp=1; // Unsupported compression type
    }
    // Check the number of color planes
    if (header->biPlanes != 1)
    {
        notbmp=1; // Unsupported number of color planes
    }
    // Check the color depth
    if (header->biBitCount != 1 && header->biBitCount != 4 && header->biBitCount != 8 && header->biBitCount != 16 && header->biBitCount != 24 && header->biBitCount != 32)
    {
        notbmp=1; // Unsupported color depth
    }
    // Check the image size
    if (header->biSizeImage > header->bfSize - header->bfOffBits)
    {
        notbmp=1; // Invalid image size
    }

    /*if (header->bfOffBits != 54)
    {
        printf("7\n");
        return 0;
    }*/

    // All checks passed, assume it's a valid BMP file
    if (notbmp){
        fseek(fp,0L,SEEK_SET);
        return get_image_format(fp);
    }
    else
        return 'B';
}

// CONVERSIONS //

char get_image_format(FILE *fp)
{
    // read first 8 bytes to determine file format
    char header[8];
    if (fread(header, 1, 8, fp) != 8)
    {
        fprintf(logfile, "Error reading file header\n");
        fclose(fp);
        return 'X';
    }

    // check file format based on header
    if (memcmp(header, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) == 0)
    {
        fclose(fp);
        return 'P';
    }
    else if (memcmp(header, "\xFF\xD8\xFF", 3) == 0)
    {
        fclose(fp);
        return 'J';
    }
    else
    {
        fprintf(logfile,"File format not supported !");
        fclose(fp);
        return 'X';
    }
}


void filterapplication(int filter, int height, int width, RGB (*image_mat)[width], int is_png, int channel, unsigned char *pixels){
    // Filter image
    switch (filter)
    {
        case 1:
        {
            /*
            while (intensity < 2 || intensity > 50)
            {
                printf("Invalid input; Intensity should be between or equal to 2 and 50!\nEnter desired intensity(2 <= level <= 50): ");
                scanf("%i", &intensity);
            }
            */
            blur(height, width, image_mat, case1entry);
            break;
        }
        case 2:
            grayscale(height, width, image_mat);
            break;
        case 3:
            if (is_png == 1) {reflectImage(pixels, width, height, channel, image_mat);}
            else { reflect(height, width, image_mat);}
            break;
        case 4:
            sepia(height, width, image_mat);
            break;
        case 5:
            artistic_filter(height, width, image_mat);
            break;
        case 6:
            cartoon_like(height, width, image_mat);
            break;
        case 7:
            emboss(height, width, image_mat);
            break;
        case 8:
        {
            int level;
            /*
            while (level <= 2 || level >= 5) // !!! !!! !!! !!!
            {
                printf("Invalid input; Color Level should be between 2 and 5!\nEnter desired color level (2 < level < 5): ");
                scanf("%i", &level); // !!! !!! !!! !!!
            }
            */
            posterize(height, width, image_mat, case8entry);
            break;
        }
        case 9:
        {
            sponge(height, width, image_mat, case9entry);
            break;
        }
        case 10:
        {
            
            neon_glow(height, width, image_mat, case10entry1, case10entry2);
            break;
        }
        case 11:
        {
            //printf("COLOR WASH : %d:,%c",case11entry,case11entry);
            color_wash(height, width, image_mat, case11entry);
            break;
        }
        case 12:
        {
            improvised(height, width, image_mat, case12entry);
            break;
        }
        case 13:
        {
            improvised2(height, width, image_mat);
            break;
        }

        case 14:
        {
            improvised3(height, width, image_mat);
            break;
        }

        case 15:

        {
            //printf("JEDI HERE");
            //printf("HEREEEE,%s",case15entry);
            improvised4(height, width, image_mat, case15entry);
            
            break;
        }

        default:
            fprintf(logfile,"Invalid Filter Option\n");

    }
}



unsigned char* load_jpeg_image2(const char* filename, int* width, int* height, int* channels)
{
    // Load the image using stb_image library
    unsigned char* data = stbi_load(filename, width, height, channels, 0);

    if (data == NULL) {
        // Handle error case
        fprintf(logfile, "Error loading image '%s': %s\n", filename, stbi_failure_reason());
        return NULL;
    }

    // Return the pixel data array
    return data;
}

void write_jpeg_image(const char* filename, int width, int height, int channels, unsigned char* data)
{
    // Write the image data to a JPEG file using stb_image_write library
    if (stbi_write_jpg(filename, width, height, channels, data, 100) == 0) {
        // Handle error case
        fprintf(logfile, "Error writing image '%s'\n", filename);
    }
}


unsigned char* load_png(const char* filename, int* width, int* height, int* channels) {
    unsigned char* data = stbi_load(filename, width, height, channels, STBI_rgb_alpha);
    if (!data) {
        fprintf(logfile,"Error loading PNG image '%s': %s\n", filename, stbi_failure_reason());
        return NULL;
    }
    return data;
}



void write_png(const char* filename, const unsigned char* data, int width, int height, int channels) {
    int stride = channels * width; // calculate the number of bytes per row
    int result = stbi_write_png(filename, width, height, channels, data, stride);
    if (!result) {
        fprintf(logfile,"Error writing PNG image '%s'\n", filename);
        return;
    }
}
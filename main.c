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
void filterapplication(int height, int width, RGB (*image_mat)[width], int is_png, int channel, unsigned char *pixels);
//Conversion functions:
// get format
char get_image_format(FILE *fp);
// JPEG functions
unsigned char* load_jpeg_image2(const char* filename, int* width, int* height, int* channels);
void write_jpeg_image(const char* filename, int width, int height, int channels, unsigned char* data);
// PNG functions
unsigned char* load_png(const char* filename, int* width, int* height, int* channels);
void write_png(const char* filename, const unsigned char* data, int width, int height, int channels);


int main()
{
    printf("How do you want to input your raw image ?\n");
    printf("1.File\n");
    printf("2.Email\n");
    printf("3.Download Link\n");
    printf("Option number: ");
    int option;
    scanf("%i", &option);
    FILE *image = NULL;
    char *file_name = malloc(256);//free
    int email_input = 0;
    if (file_name==NULL){
        printf("failed to allocate memory for file_name\n");
        exit(1);
    }
    switch (option)
    {
        case 1:
        {
            printf("What's the file name ? ");
            scanf("%s", file_name);
            image = fopen(file_name, "rb");
            break;
        }

        case 2:
        {
            // Email info that this program works with:
            email_input = 1;
            char *email_username = "youremail@gmail.com";
            char *email_pass = "yourpassword";
            char *server = "imaps://imap.gmail.com";
            printf("Send your image without any subject to this Email address: youremail@gmail.com\n");
            printf("When you sent the Email enter y: ");
            char temp;
            scanf("%s", &temp);
            file_name = "decoded_image.png";
            if (downloadLastEmailAttachment(server, email_username, email_pass)){
                image = fopen(file_name, "rb");
            }
            break;
        }

        case 3:
        {
            strcpy(file_name,"downloadout.jpg");
            printf("Input the link: ");
            char *domain = malloc(1024);
            char *path = malloc(1024);
            char *link = malloc(1024);
            if (domain == NULL) { exit(2);}
            if(path==NULL){exit(2);}
            scanf("%s", link);
            sscanf(link,"https://%[^/]%s",domain,path);
            printf("domain :-%s-path :-%s-",domain,path);
            download_url(domain,path);
            FILE * httpsresponse = fopen("response.txt","rb");
            if (!httpsresponse){exit(3);}
            fclose(formatchecker(httpsresponse));
            image = fopen(file_name,"rb");
            free(domain);
            free(path);
            break;
        }

        default:
            printf("Invalid Option\n");
            exit(3);
    }

    // Checking whether the image was saved to memory correctly or not:
    if (image == NULL)
    {
        printf("Image file could not be received !\n");
        fclose(image);
        exit(4);
    }



    // reading size of a BMP header to check if it is actually BMP:
    BMPHeader header;
    if (fread(&header, sizeof(BMPHeader), 1, image) != 1)
    {
        printf("Failed to read file's header\n");
        fclose(image);
        exit(5);
    }

    //Image variables that we'll work with in all cases:
    int height;
    int width;
    int channels;



    //returning to the start of the file for get_image_format
    fseek(image, 0, SEEK_SET);


    char format = is_bmp(&header,image);
    fseek(image, 0, SEEK_SET);

    switch (format) {
        case 'B': {
            printf("BMP Case\n");
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

            filterapplication(height, width, image_mat, 0, channels, NULL);

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
            printf("JPEG CASE\n");
            unsigned char *pixels = load_jpeg_image2(file_name, &width, &height, &channels);
            printf("Width : %d height : %d channels : %d\n",width,height,channels);
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
            filterapplication(height, width, image_mat, 0, channels, NULL);
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
            printf("PNG CASE\n");
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

            printf("Width : %d height : %d channels : %d\n",width,height,channels);

            int i = 0;
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    image_mat[j][k].Red = pixels[i++];
                    image_mat[j][k].Green = pixels[i++];
                    image_mat[j][k].Blue = pixels[i++];
                    if (channels == 4) { i++; } //value alpha ro skip mikonim
                }
            }

            filterapplication(height, width, image_mat, 1, channels, pixels);

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
        case 'X':
            printf("case rideman!\n");
            return 4;
        default:
            break;
    }


    exit(0);
}

void filterapplication(int height, int width, RGB (*image_mat)[width], int is_png, int channel, unsigned char *pixels){
    int filter;
    printf("Which filter do you want to apply ?\n1.Blur\n2.Grayscale\n3.Reflect\n4.Sepia\n5.Artistic Edge\n6.Dark Grain\n");
    printf("7.Emboss\n8.Posterize\n9.Sponge\n10.Neon Glow\n11.Color Wash\n12.Improvised\n13.Improvised2\n14.Improvised3\n15.Improvised4\n");
    printf("Filter number: ");
    scanf("%i", &filter);
    // Filter image
    switch (filter)
    {
        case 1:
        {
            int intensity; printf("What your desired intensity(2 <= intensity <= 50) for this filter ? ");
            scanf("%i", &intensity);
            while (intensity < 2 || intensity > 50)
            {
                printf("Invalid input; Intensity should be between or equal to 2 and 50!\nEnter desired intensity(2 <= level <= 50): ");
                scanf("%i", &intensity);
            }
            blur(height, width, image_mat, intensity);
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
            printf("What is your desired color level(2 < level < 5) for this filter ? ");
            scanf("%i", &level);
            while (level <= 2 || level >= 5) // !!! !!! !!! !!!
            {
                printf("Invalid input; Color Level should be between 2 and 5!\nEnter desired color level (2 < level < 5): ");
                scanf("%i", &level); // !!! !!! !!! !!!
            }
            posterize(height, width, image_mat, level);
            break;
        }
        case 9:
        {
            int intensity; printf("What your desired intensity(0 won't make a difference) for this filter ? "); scanf("%i", &intensity);
            sponge(height, width, image_mat, intensity);
            break;
        }
        case 10:
        {
            int intensity, threshold; printf("What is your desired intensity for this filter ? "); scanf("%i", &intensity);
            printf("What is your desired threshold for this filter ? ");
            scanf("%i", &threshold);
            neon_glow(height, width, image_mat, intensity, threshold);
            break;
        }
        case 11:
        {
            char c;
            printf("Which one of the RGB values do you want to keep (enter R,G,B)? "); scanf("%s", &c);
            color_wash(height, width, image_mat, c);
            break;
        }
        case 12:
        {
            char c;
            printf("A: replaces Green with Red            B: replaces Blue with Red\n");
            printf("C: replaces Green with Blue            D: replaces Red with Blue\n");
            printf("E: replaces Blue with Green            F: replaces Red with Green\n");
            printf("Which version(A,B,C,D,E,F) of the filter? "); scanf("%s", &c);
            improvised(height, width, image_mat, c);
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
            char *inp = malloc(256);
            printf("Do you want the filter to work with Minimum or Maximum values (enter Max or Min)? "); scanf("%s", inp);
            improvised4(height, width, image_mat, inp);
            free(inp);
            break;
        }

        default:
            printf("Invalid Filter Option\n");

    }
}
/* Callback function to write downloaded data to file */
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

// Function to download an image from a URL and return a file pointer to the downloaded image
FILE *download_image(char *url)
{
    CURL *curl_handle;
    static const char *pagefilename = "page.out";
    FILE *pagefile;

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* set URL to get here */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* Switch on full protocol/debug output while testing */
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);

    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    /* open the file */
    pagefile = fopen(pagefilename, "wb");
    if (pagefile)
    {

        /* write the page body to this file handle */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

        /* get it! */
        curl_easy_perform(curl_handle);

        /* close the header file */
        fclose(pagefile);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    curl_global_cleanup();

    return fopen("page.out", "rb");
}

size_t discard_response(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    /* Discard the received data */
    return size * nmemb;
}

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
            fprintf(stderr, "Login failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return NULL;
        }

        //Alternative: const char *select_command = "SELECT \"[Gmail]/All Mail\"";
        const char *select_command = "SELECT INBOX";
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, select_command);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Selecting mailbox failed: %s (%d)\n", curl_easy_strerror(res), res);
            return NULL;
        } else {
            if (first_time) {printf("\nINBOX selected successfully!\n");}

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
                fprintf(stderr, "Fetching UID failed: %s (%d)\n", curl_easy_strerror(res), res);
                return NULL;
            } else {
                if (first_time) {printf("UID fetched successfully!\n\n   Please Wait...\n\n");}
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
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
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
        printf("Failed to create output file for base64 data\n");
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
            printf("Failed to connect to server: %s\n", curl_easy_strerror(res));
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
        printf("Failed to open fetched data file for reading only attachment data\n");
        return 0;
    }
    //fwrite(final_encoded_att, 1, strlen(final_encoded_att), f);
    fwrite(encoded_att, 1, strlen(encoded_att), f);
    fclose(f);

    //now decoding
    //decode64(final_encoded_att);
    decode64(encoded_att);
    printf("Attachment downloaded successfully!\n");
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
    FILE* file = fopen("decoded_image.png", "wb");
    fwrite(decoded_image, sizeof(char), num_decoded, file);
    fclose(file);

    // Free the memory used by the decoded image data
    free(decoded_image);
}



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
        fprintf(stderr, "Error reading file header\n");
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
        printf("File format not supported !");
        fclose(fp);
        return 'X';
    }
}




unsigned char* load_jpeg_image2(const char* filename, int* width, int* height, int* channels)
{
    // Load the image using stb_image library
    unsigned char* data = stbi_load(filename, width, height, channels, 0);

    if (data == NULL) {
        // Handle error case
        fprintf(stderr, "Error loading image '%s': %s\n", filename, stbi_failure_reason());
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
        fprintf(stderr, "Error writing image '%s'\n", filename);
    }
}


unsigned char* load_png(const char* filename, int* width, int* height, int* channels) {
    unsigned char* data = stbi_load(filename, width, height, channels, STBI_rgb_alpha);
    if (!data) {
        printf("Error loading PNG image '%s': %s\n", filename, stbi_failure_reason());
        return NULL;
    }
    return data;
}



void write_png(const char* filename, const unsigned char* data, int width, int height, int channels) {
    int stride = channels * width; // calculate the number of bytes per row
    int result = stbi_write_png(filename, width, height, channels, data, stride);
    if (!result) {
        printf("Error writing PNG image '%s'\n", filename);
        return;
    }
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

void download_url (char * SERVER, char * PATH){

    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    if (ctx == NULL) {
        printf("Error creating SSL context\n");

    }

    SSL *ssl = SSL_new(ctx);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        printf("Error opening socket\n");

    }

    server = gethostbyname(SERVER);

    if (server == NULL) {
        printf("Error resolving server hostname\n");

    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error connecting to server\n");

    }

    BIO *sbio = BIO_new_socket(sockfd, BIO_NOCLOSE);
    SSL_set_bio(ssl, sbio, sbio);
    if (SSL_set_tlsext_host_name(ssl, SERVER) != 1) {
        printf("Error setting SNI\n");

    }
    if (SSL_connect(ssl) <= 0) {
        printf("Error establishing SSL connection\n");
    }

    char request[1024];
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", PATH, SERVER);
    SSL_write(ssl, request, strlen(request));

    char response[1024];
    int bytes_read;
    int total_bytes_read = 0;
    FILE *fp = fopen("response.txt", "wb");

    if (fp == NULL) {
        printf("Error opening file\n");

    }

    do {
        bytes_read = SSL_read(ssl, response, sizeof(response));
        printf("reading %i",bytes_read);

        if (bytes_read > 0) {
            printf("bytes read: %i\n",bytes_read);
            fwrite(response, 1, bytes_read, fp);
            total_bytes_read += bytes_read;
        }
    } while (bytes_read > 0);

    fclose(fp);

    printf("Total bytes read: %d\n", total_bytes_read);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);

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
    printf("PARSING BMP\n");
    char outfilename[] = "downloadout.jpg";
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
                            printf("Writing BMP File\n");
                        }
                    }
                }
            }
        }
    }
    return outfile;
    printf("Done!\n");
}

FILE * parsepng(FILE * fp){
    printf("PARSING PNG\n");
    char outfilename[] = "downloadout.jpg";
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
                printf("Writing PNG File.\n");
            }
            else{
                fseek(fp,-7L,SEEK_CUR);
            }
        }
    }
    printf("Done!\n");
    return outfile;
}

FILE * parsejpg(FILE * fp){

    printf("PARSING JPG\n");
    char outfilename[] = "downloadout.jpg";
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
                printf("Writing JPEG File.\n");
            }
            else{
                fseek(fp,-2L,SEEK_CUR);
            }
        }
    }
    printf("Done!\n");
    return outfile;
}

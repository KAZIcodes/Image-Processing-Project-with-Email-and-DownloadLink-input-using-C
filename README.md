# Image-Processing-using-C
Image Processing Project with Email and DownloadLink User's image input Supported using C for
Advanced Programing Course at Shiraz University

GENERAL DESCRIPTION:
  You can input your image in 3 ways:
  1. Image file that is available locally
  2. Send the image file via email
  3. Provide a download link to the image
<br>      
The program supports three image formats: PNG, JPG(also JPEG) and BMP(BMP is handeled explicitly and without the use of any external libraries).
The algorithm for the filters with the title 'improv' or 'improvised' are completely our own ideas influenced by some photoshop features.




TECHNICAL NOTE:
  1. The source files contain both backend version(main.c) and GUI version(GUI.c) and you should compile one of them at a time with other source            files and libreries. 
  2. You should add your Gmail address and google's app password(the normal password for your Gmail will not work) for it(which you can generate under      the Two-Factor Authentication of your Gmail account) into the source files(main.c and GUI.c) so that the code works.
  3. BE AWARE: The Email section of the project is based on imap protocol of google at the time of writing the project and if the protocol is changed       or modified, you should update the parsing section of the code(which is handled explicitly and without any library) accordingly.
  4. If you want to compile the GUI version, be sure to have the filtered directory in the directory that you are running the program after                 compilation.
    

  Libraries Used: GTK3, Openssl, libb64, libcurl, STBimage (note that they may have some dependencies).


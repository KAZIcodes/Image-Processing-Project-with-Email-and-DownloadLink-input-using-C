# Image-Processing-using-C
Image Processing Project with Email and DownloadLink User's image input Supported using C
Advanced Programing Course Project at Shiraz University

GENERAL DESCRIPTION:
  You can input your image in 3 ways: 1.image file from directory 2.send it via Email 3.download link to the image
  and there are 3 supported image formats: PNG, JPG(also JPEG) and BMP(BMP is handeled explicitly and without any library).
  The algorithm of filters with improve or improvised title are completely our own ideas influenced by some photoshop features and are not from other    sources.




TECHNICAL NOTE:
  1. The source files contain both only-backend version(main.c) and GUI version(GUI.c) and you should compile one of them at a time with other source       files and libreries. 
  2. You should add your Gmail address and google's app password(the normal password for your Gmail will not work) for it(which you can generate under      the Two-Factor Authentication of your Gmail account) into the source files(main.c and GUI.c) so that the code works.
  3. BE AWARE: The Email section of the project is based on imap protocol of google at the time of writing the project and if the protocol is changed       or modified, you should update the parsing section of the code(which is handeled explicitly and without any library) accordingly.
    

  Libraries Used: GTK3, Openssl, libb64, libcurl, STBimage (note that they may have some dependencies)



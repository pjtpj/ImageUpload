#ifndef VERSION_H

#define BUILDCOUNT_NUM 1
#define BUILDCOUNT_STR "1"

#define VER_FILEVERSION_STR      "1.0.0." BUILDCOUNT_STR "\0"
#define VER_FILEVERSION          1,0,0,BUILDCOUNT_NUM
#define VER_FILEDESCRIPTION_STR  "Image Upload\0"
#define VER_INTERNALNAME_STR     "ImageUpload\0"
#define VER_ORIGINALFILENAME_STR "ImageUploadCtrl.dll\0"
#define VER_PRODUCTNAME_STR      "Image Upload\0"
#define VER_COMPANYNAME_STR      "Teztech, Inc.\0"
#define VER_LEGALTRADEMARKS_STR  "\0"
#define VER_LEGALCOPYRIGHT_STR   "Copyright (C) 2003-2007 Teztech, Inc.\0"
#define VER_PRODUCTVERSION_STR   VER_FILEVERSION_STR
#define VER_PRODUCTVERSION       VER_FILEVERSION
#define VER_COMMENT_STR          "\0"
#define VER_FILETYPE             VFT_DLL
#define VER_FILESUBTYPE          0
#define VER_FILEFLAGSMASK        VS_FFI_FILEFLAGSMASK
#define VER_FILEFLAGS            0L
#define VER_FILEOS               VOS_DOS_WINDOWS32

#define VERSION_H
#endif

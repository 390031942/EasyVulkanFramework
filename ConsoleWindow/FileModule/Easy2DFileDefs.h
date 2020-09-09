 #include<qstring.h>

#ifndef FileModule
#define FileMoudle

#define FILEICON_PATH(filetype) QString("FileIcon_")+QString(filetype!=QString::null?filetype:"folder")+".png"

#define MinWindowSize QSize(250,100)

#define Default_ButtonSize_X 75
#define Default_ButtonSize_Y 100

#define Default_m_Spacing_X   20
#define Default_m_Spacing_Y   10

#define Default_Left_Spacing  10
#define Default_Right_Spacing 10
#define Default_Up_Spacing    -10

#define Default_FileButtonIconSizeX     60
#define Default_FileButtonIconSizeY     60

#define Default_FileButtonLabelSizeX     70
#define Default_FileButtonLabelSizeY     18

#define Default_FileButtonLabelOffsetX   -1
#define Default_FileButtonLabelOffsetY   80


#define File_GetButtonCountPerRow(width,sizeX,spacingX)  (width +spacingX - Default_Left_Spacing-Default_Right_Spacing) / (sizeX +spacingX)
#define File_GetCorrectedSpacingX(countPerRow,width,sizeX) (width - Default_Left_Spacing-Default_Right_Spacing - countPerRow * sizeX) / (countPerRow - 1)

#define File_GetPosition(i,countPerRow,sizeX,spacingX,sizeY,spacingY) Default_Left_Spacing+(sizeX + spacingX)*(i%countPerRow),\
                                            Default_Up_Spacing + ((i) /countPerRow)*(sizeY + spacingY)

#endif // FileMoudle 

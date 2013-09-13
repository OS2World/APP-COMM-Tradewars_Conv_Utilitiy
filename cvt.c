#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WARPS 6
#define _TWBSIG_1_ 0xADDE3412
#define _TWBSIG_2_ 0x2143CEFA
#define _TWVER_MAJOR_ 0
#define _TWVER_MINOR_ 14
#define TRUE 1
#define FALSE 0

typedef struct
{
    unsigned long sig1;
    unsigned long sig2;
    unsigned long reserved;
    unsigned char majorVer;
    unsigned char minorVer;
    unsigned short sectorCount;
} TWBinaryHeader;

typedef struct
{
    unsigned char explored;
    unsigned char reserved[3];
    unsigned short outWarps[MAX_WARPS];
} TWBinarySectorData;

typedef struct
{
    unsigned short outWarps[MAX_WARPS];
    unsigned long explored;
} TWOldBinaryData;

void Usage(void);
void FileError(char* fn);
void ReadError(char* fn);
void WriteError(char *fn);
void GarbageData(char *fn);
void NewFormat(char* fn);
int GarbageCheck(TWOldBinaryData *od);

int main(int argc, char *argv[])
{
    char iname[256], oname[256];
    FILE *ifile, *ofile;
    unsigned long fi=1, scount=0;
    TWOldBinaryData od;
    TWBinarySectorData bd;
    TWBinaryHeader bh;

    if (argc!=3)
        Usage();

    strncpy(iname, argv[1], sizeof(iname)-1);
    strncpy(oname, argv[2], sizeof(oname)-1);

    ifile=fopen(iname, "rb");
    if (!ifile)
        FileError(iname);

    ofile=fopen(oname, "wb");
    if (!ofile)
        FileError(oname);

    printf("\nSectors converted:     0");
    fflush(stdout);

    do
        {
        if (fread(&od, sizeof(od), 1, ifile)!=1)
            {
            if (!feof(ifile))
                ReadError(iname);
            break;
            }
        if (fi)
            {
            fi=0;
            memcpy(&bh, &od, sizeof(bh));
            if (bh.sig1==_TWBSIG_1_ && bh.sig2==_TWBSIG_2_)
                NewFormat(iname);
            memset(&bh, 0, sizeof(bh));
            if (fwrite(&bh, sizeof(bh), 1, ofile)!=1)
                WriteError(oname);
            }
        if (GarbageCheck(&od))
            GarbageData(iname);
        memset(&bd, 0, sizeof(bd));
        memcpy(&bd.outWarps, &od.outWarps, sizeof(unsigned short)*6);
        bd.explored=(unsigned char)od.explored;
        if (fwrite(&bd, sizeof(bd), 1, ofile)!=1)
            WriteError(oname);
        scount++;
        if (scount%10==0)
            {
            printf("\b\b\b\b\b%5u", scount);
            fflush(stdout);
            }
        } while (!feof(ifile));

    printf("\b\b\b\b\b%5u\n", scount);
    fflush(stdout);

    fclose(ifile);
    fseek(ofile, 0, SEEK_SET);

    bh.sig1=_TWBSIG_1_;
    bh.sig2=_TWBSIG_2_;
    bh.majorVer=(unsigned char)_TWVER_MAJOR_;
    bh.minorVer=(unsigned char)_TWVER_MINOR_;
    bh.sectorCount=(unsigned short)scount;

    if (fwrite(&bh, sizeof(bh), 1, ofile)!=1)
        WriteError(oname);

    return 0;
}

void Usage(void)
{
    printf("\nUsage: cvt.exe <oldname> <newname>\n");
    printf("\noldname - filename of old database\n");
    printf("newname - filename of new database\n");
    exit(1);
}

void FileError(char *fn)
{
    printf("\nUnable to open file %s\n", fn);
    exit(1);
}

void ReadError(char *fn)
{
    printf("\nError reading file %s\n", fn);
    exit(1);
}

void WriteError(char *fn)
{
    printf("\nError writing file %s\n", fn);
    exit(1);
}

void GarbageData(char *fn)
{
    printf("\nInput file (%s) contains invalid data.\n", fn);
    exit(1);
}

void NewFormat(char *fn)
{
    printf("\nSpecified input file (%s) already in new format.\n", fn);
    exit(1);
}

int GarbageCheck(TWOldBinaryData *od)
{
    int i, zeroSet=FALSE;

    if (od->explored!=FALSE && od->explored!=TRUE)
        return TRUE;

    for (i=0; i<MAX_WARPS; i++)
        {
        if (od->outWarps[i]==0)
            {
            if (!zeroSet)
                zeroSet=TRUE;
            }
        else
            {
            if (zeroSet)
                return FALSE;
            }
        }

    return FALSE;
}

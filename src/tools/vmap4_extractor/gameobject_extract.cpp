#include "model.h"
#include "dbcfile.h"
#include "adtfile.h"
#include "vmapexport.h"

#include <algorithm>
#include <stdio.h>

char output_path1[128]=".";
char input_path1[1024]=".";

bool ExtractSingleModel(std::string& fname)
{
    if (fname.substr(fname.length() - 4, 4) == ".mdx")
    {
        fname.erase(fname.length() - 2, 2);
        fname.append("2");
    }

    std::string originalName = fname;

    char* name = GetPlainName((char*)fname.c_str());
    FixNameCase(name, strlen(name));
    FixNameSpaces(name, strlen(name));

    std::string output(szWorkDirWmo);
    output += "/";
    output += name;

    if (FileExists(output.c_str()))
        return true;

    Model mdl(originalName);
    if (!mdl.open())
        return false;

    return mdl.ConvertToVMAPModel(output.c_str());
}

extern HANDLE LocaleMpq;

void ExtractGameobjectModels()
{
	printf("Extracting GameObject models...");
    char localMPQ[512];

    sprintf(localMPQ, "%s/Data/model.MPQ", input_path1); // not sure for model.mpq
    if (FileExists(localMPQ)==false)
    {   // Use model.mpq
        printf(localMPQ, "%s/Data/%s/locale-%s.MPQ", input_path1);
    }

    if (!SFileOpenArchive(localMPQ, 0, MPQ_OPEN_READ_ONLY, &LocaleMpq))
    {
        exit(1);
    }


    HANDLE dbcFile;
    if (!SFileOpenFileEx(LocaleMpq, "DBFilesClient\\GameObjectDisplayInfo.dbc", SFILE_OPEN_PATCHED_FILE, &dbcFile))
    {
        if (!SFileOpenFileEx(LocaleMpq, "DBFilesClient\\GameObjectDisplayInfo.dbc", SFILE_OPEN_PATCHED_FILE, &dbcFile))
        {
            printf("Fatal error: Cannot find GameObjectDisplayInfo.dbc in archive!\n");
            exit(1);
        }
    }

    
	DBCFile dbc(dbcFile);
    
    if(!dbc.open())
    {
        printf("Fatal error: Invalid GameObjectDisplayInfo.dbc file format!\n");
        exit(1);
    }

    std::string basepath = szWorkDirWmo;
    basepath += "/";
    std::string path;

    FILE * model_list = fopen((basepath + "temp_gameobject_models").c_str(), "wb");

    for (DBCFile::Iterator it = dbc.begin(); it != dbc.end(); ++it)
    {
        path = it->getString(1);

        if (path.length() < 4)
            continue;

        FixNameCase((char*)path.c_str(), path.size());
        char * name = GetPlainName((char*)path.c_str());
        FixNameSpaces(name, strlen(name));

        char * ch_ext = GetExtension(name);
        if (!ch_ext)
            continue;

        strToLower(ch_ext);

        bool result = false;
        if (!strcmp(ch_ext, ".wmo"))
            result = ExtractSingleWmo(path);
        else if (!strcmp(ch_ext, ".mdl"))   // TODO: extract .mdl files, if needed
            continue;
        else //if (!strcmp(ch_ext, ".mdx") || !strcmp(ch_ext, ".m2"))
            result = ExtractSingleModel(path);

        if (result)
        {
            uint32 displayId = it->getUInt(0);
            uint32 path_length = strlen(name);
            fwrite(&displayId, sizeof(uint32), 1, model_list);
            fwrite(&path_length, sizeof(uint32), 1, model_list);
            fwrite(name, sizeof(char), path_length, model_list);
        }
    }

    fclose(model_list);

    printf("Done!\n");
}

/*
/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "mangLua.h"
#include "Chat.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(mangLua);

void mangLua::LoadDirectory(char* Dirname, LoadedScripts* lscr)
{
    sLog.outBasic("[Lua]: Scanning lua_scripts/.");
    #ifdef WIN32
        HANDLE hFile;
        WIN32_FIND_DATA FindData;
        memset(&FindData, 0, sizeof(FindData));
        char SearchName[MAX_PATH];

        strcpy(SearchName, Dirname);
        strcat(SearchName, "\\*.*");

        hFile = FindFirstFile(SearchName, &FindData);

        // break if we don't find dir
        if(hFile == NULL)
        {
            sLog.outError("%s No `lua_scripts` directory found!", CONSOLE_HEADER);
            return;
        }

        FindNextFile(hFile, &FindData);
        while( FindNextFile(hFile, &FindData) )
        {
            if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                strcpy(SearchName,Dirname);
                strcat(SearchName, "\\");
                strcat(SearchName, FindData.cFileName);
                LoadDirectory(SearchName, lscr);
            }
            else
            {
                std::string fname = Dirname;
                fname += "\\";
                fname += FindData.cFileName;
                size_t len = strlen(fname.c_str());
                int i = 0;
                char ext[MAX_PATH];
                while(len > 0)
                {
                    ext[i++] = fname[--len];
                    if(fname[len] == '.')
                        break;
                }
                ext[i++] = '\0';
                if(!_stricmp(ext,"aul."))
                    lscr->luaFiles.insert(fname);
            }
        }
        FindClose(hFile);
    #endif
}

void mangLua::LoadEngine()
{
    LoadedScripts lscr;
    LoadDirectory("lua_scripts", &lscr);
}
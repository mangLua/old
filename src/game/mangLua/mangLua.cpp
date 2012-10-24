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
#include "Policies/SingletonImp.h"
#include "Chat.h"
#include "mangLuaFunctions.h"
#include "FunctionTables.h"

INSTANTIATE_SINGLETON_1(mangLua);

void mangLua::Start()
{
    L = luaL_newstate();

    LoadEngine();
}

void mangLua::report(lua_State* L)
{
    const char* msg = lua_tostring(L, -1);
    while(msg)
    {
        lua_pop(L, -1);
        printf("\t%s\n",msg);
        msg = lua_tostring(L, -1);
    }
}

void mangLua::LoadDirectory(char* Dirname, LoadedScripts* lscr)
{
    sLog.outString("[mangLua]: Scanning lua_scripts/.");
    #ifdef WIN32
        HANDLE hFile;
        WIN32_FIND_DATA FindData;
        memset(&FindData, 0, sizeof(FindData));
        char SearchName[MAX_PATH];

        strcpy(SearchName, Dirname);
        strcat(SearchName, "\\*.*");
        sLog.outString("[mangLua]: Searchdir: %s",SearchName);
        hFile = FindFirstFile(SearchName, &FindData);

        if(hFile == INVALID_HANDLE_VALUE)
        {
            sLog.outError("[mangLua]: %s No `lua_scripts` directory found!");
            return;
        }

        FindNextFile(hFile, &FindData);
        while( FindNextFile(hFile, &FindData) )
        {
            if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                sLog.outString("[mangLua]: Directory: %s",FindData.cFileName);
                strcpy(SearchName,Dirname);
                strcat(SearchName, "\\");
                strcat(SearchName, FindData.cFileName);
                LoadDirectory(SearchName, lscr);
            }
            else
            {
                sLog.outString("[mangLua]: File: %s",FindData.cFileName);
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
                {
                   sLog.outString("[mangLua]: Adding File: %s",fname.c_str());
                   lscr->luaFiles.insert(fname);
                }
            }
        }
        FindClose(hFile);
    #endif
}

void mangLua::LoadEngine()
{
    luaL_openlibs(L);

    LoadedScripts lscr;
    LoadDirectory("lua_scripts", &lscr);

    sLog.outString("[mangLua]: LoadDirectory complete");
    sLog.outString("[mangLua]: Files loaded: %i",lscr.luaFiles.size());
    sLog.outString("[mangLua]: LoadEngine complete");

    char filename[200];
    unsigned int cnt_uncomp = 0;

    for(std::set<std::string>::iterator itr = lscr.luaFiles.begin(); itr != lscr.luaFiles.end(); ++itr)
    {
        strcpy(filename, itr->c_str());

        if (luaL_loadfile(L, filename) != 0)
        {
            sLog.outError("[mangLua]: loading %s failed.(could not load)", itr->c_str());
        }
        else
        {
        if (lua_pcall(L, 0, 0, 0) != 0)
        {
            sLog.outError("[mangLua]: %s failed.(could not run)", itr->c_str());
        }
        else
            sLog.outString("[mangLua]: loaded %s.", itr->c_str());
        }
        cnt_uncomp++;
    }
    sLog.outString("[mangLua]: Loaded %i Lua scripts.", cnt_uncomp);
}
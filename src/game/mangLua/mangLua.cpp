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
		sLog.outBasic("[Lua]: Searchdir: %s",SearchName);
        hFile = FindFirstFile(SearchName, &FindData);

        // break if we don't find dir
        if(hFile == INVALID_HANDLE_VALUE)
        {
            sLog.outError("%s No `lua_scripts` directory found!", CONSOLE_HEADER);
            return;
        }
		sLog.outBasic("[Lua]: If we got here, we found something: %s",FindData.cFileName);

        FindNextFile(hFile, &FindData);
        while( FindNextFile(hFile, &FindData) )
        {
            if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
				sLog.outBasic("[Lua]: Directory: %s",FindData.cFileName);
                strcpy(SearchName,Dirname);
                strcat(SearchName, "\\");
                strcat(SearchName, FindData.cFileName);
                LoadDirectory(SearchName, lscr);
            }
            else
            {
				sLog.outBasic("[Lua]: File1: %s",FindData.cFileName);
                std::string fname = Dirname;
                fname += "\\";
                fname += FindData.cFileName;
                size_t len = strlen(fname.c_str());
				sLog.outBasic("[Lua]: File2: %s",fname.c_str());
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
					sLog.outBasic("[Lua]: Adding File: %s",fname.c_str());
                    lscr->luaFiles.insert(fname);
				}
            }
        }
        FindClose(hFile);
    #endif
}

void mangLua::LoadEngine()
{
	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
    LoadedScripts lscr;
    LoadDirectory("lua_scripts", &lscr);
	sLog.outBasic("LoadDirectory complete");
	sLog.outBasic("Files loaded: %i",lscr.luaFiles.size());
	sLog.outBasic("LoadEngine complete");

char filename[200];
	unsigned int cnt_uncomp = 0;
for(std::set<std::string>::iterator itr = lscr.luaFiles.begin(); itr != lscr.luaFiles.end(); ++itr)
{
strcpy(filename, itr->c_str());
if(luaL_loadfile(L, filename) != 0)
{
sLog.outError("loading %s failed.(could not load)", itr->c_str());
}
else
{
if(lua_pcall(L, 0, 0, 0) != 0)
{
sLog.outError("%s failed.(could not run)", itr->c_str());
}
else
sLog.outBasic("loaded %s.", itr->c_str());
}
cnt_uncomp++;
}
sLog.outBasic("Loaded %i Lua scripts.", cnt_uncomp);
}
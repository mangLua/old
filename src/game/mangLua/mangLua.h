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

#include "Policies/Singleton.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
};

struct LoadedScripts
{
    std::set<std::string> luaFiles;
};


class mangLua
{
private:
    lua_State* L;

public:
    void Start();
    void LoadEngine();
    void LoadDirectory(char* Dirname, LoadedScripts* lscr);

    static void report(lua_State* /*L*/);
};

#define smangLua MaNGOS::Singleton<mangLua>::Instance()
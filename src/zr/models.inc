/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          models.inc
 *  Type:          Core
 *  Description:   Model manager.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/*
 * Note: Data structures and constants defined in models.h.inc.
 */

/**
 * Parsed model data.
 */
new ModelData[MODELS_MAX][ModelAttributes];

/**
 * Number of valid models.
 */
new ModelCount;


/**
 * Prepare all model/download data.
 */
ModelsLoad()
{
    new Handle:kvModels = INVALID_HANDLE;
    
    // Register config file.
    ConfigRegisterConfig(File_Models, Structure_List, CONFIG_FILE_ALIAS_MODELS);
    
    // Get models file path.
    decl String:modelPath[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_MODELS, modelPath);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Missing model list: \"%s\"", modelPath);
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Models, modelPath);
    
    // Prepare key/value structure.
    kvModels = CreateKeyValues(CONFIG_FILE_ALIAS_MODELS);
    
    // Log what models file that is loaded.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Loading models from file \"%s\".", modelPath);
    
    // Load model data file.
    FileToKeyValues(kvModels, modelPath);
    
    // Try to find the first model.
    KvRewind(kvModels);
    if (!KvGotoFirstSubKey(kvModels))
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Can't find any models in \"%s\"", modelPath);
    }
    
    decl String:buffer[256];
    decl String:name[64];
    decl String:path[PLATFORM_MAX_PATH];
    decl String:team[64];
    decl String:access[64];
    decl String:group[64];
    
    ModelCount = 0;
    new failedCount;
    new publicCount;
    new downloadCount;
    
    // Loop through all models and store attributes in ModelData array.
    do
    {
        if (ModelCount > MODELS_MAX)
        {
            // Maximum number of models reached. Log a warning and exit the loop.
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Warning: Maximum number of models reached (%d). Skipping other models.", MODELS_MAX + 1);
            
            break;
        }
        
        KvGetString(kvModels, "name", name, sizeof(name));
        strcopy(ModelData[ModelCount][Model_Name], 64, name);
        
        KvGetString(kvModels, "path", path, sizeof(path));
        strcopy(ModelData[ModelCount][Model_Path], 64, path);
        
        KvGetString(kvModels, "team", team, sizeof(team));
        ModelData[ModelCount][Model_Team] = ModelsStringToTeam(team);
        
        KvGetString(kvModels, "access", access, sizeof(access));
        ModelData[ModelCount][Model_Access] = ModelsStringToAccess(access);
        
        KvGetString(kvModels, "group", group, sizeof(group));
        strcopy(ModelData[ModelCount][Model_Group], 64, group);
        
        
        // Validate model attributes.
        
        // Build path and check if model file exist.
        strcopy(buffer, sizeof(buffer), path);
        StrCat(buffer, sizeof(buffer), name);
        StrCat(buffer, sizeof(buffer), ".mdl");
        if (!FileExists(buffer, true))
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Warning: Invalid model name/path setting at index %d. File not found: \"%s\".", ModelCount + failedCount, buffer);
            failedCount++;
            continue;
        }
        
        // Validate team.
        if (ModelData[ModelCount][Model_Team] == ModelTeam_Invalid)
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Warning: Invalid model team setting at index %d: \"%s\".", ModelCount + failedCount, team);
            failedCount++;
            continue;
        }
        
        // Validate access.
        if (ModelData[ModelCount][Model_Access] == ModelAccess_Invalid)
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Warning: Invalid model access setting at index %d: \"%s\".", ModelCount + failedCount, access);
            failedCount++;
            continue;
        }
        else
        {
            // Increment public model counter for the current team.
            if (ModelData[ModelCount][Model_Access] == ModelAccess_Public)
            {
                publicCount++;
            }
        }
        
        // Validate group.
        if (ModelData[ModelCount][Model_Access] == ModelAccess_Group &&
            FindAdmGroup(group) == INVALID_GROUP_ID)
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Warning: Invalid model group setting at index %d. Couldn't find SourceMod group \"%s\".", ModelCount + failedCount, group);
            failedCount++;
            continue;
        }
        
        // Open directory with model files.
        new Handle:dir = OpenDirectory(path);
        
        // Check if failed.
        if (dir == INVALID_HANDLE)
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Error opening directory: %s", dir);
            continue;
        }
        
        // Reset file counter for the current model.
        downloadCount = 0;
        
        new FileType:type;
        decl String:file[64];
        decl String:fileShort[64];
        
        // Search for model files with the specified name and add them to
        // downloads table.
        while (ReadDirEntry(dir, file, sizeof(file), type))
        {
            // Skip if entry isn't a file.
            if (type != FileType_File)
            {
                continue;
            }
            
            // Find break point index in the string to get model name.
            // Add one to make space for null terminator.
            new breakpoint = FindCharInString(file, '.') + 1;
            strcopy(fileShort, breakpoint, file);
            
            // If this file doesn't match model name, then skip it.
            if (!StrEqual(name, fileShort, false))
            {
                continue;
            }
            
            // Format a full path string.
            strcopy(buffer, sizeof(buffer), path);
            Format(buffer, sizeof(buffer), "%s%s", buffer, file);
            
            AddFileToDownloadsTable(buffer);
            downloadCount++;
        }
        
        CloseHandle(dir);
        
        // Check if no model files were found.
        if (!downloadCount)
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Couldn't find any model files for \"%s\". Check name and path.", name);
        }
        else
        {
            ModelCount++;
        }
    } while (KvGotoNextKey(kvModels));
    
    CloseHandle(kvModels);
    
    // Check if there are no public models.
    if (!publicCount)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Missing public model in \"%s\". There must be at least one public model.", modelPath);
    }
    
    // Precache models.
    ModelsPrecache();
    
    // Log model validation info.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Successful: %d | Unsuccessful: %d", ModelCount, failedCount);
    
    // Set config data.
    ConfigSetConfigLoaded(File_Models, true);
    ConfigSetConfigReloadFunc(File_Models, GetFunctionByName(GetMyHandle(), "ModelsOnConfigReload"));
}

/**
 * Called when config is being reloaded.
 */
public ModelsOnConfigReload(ConfigFile:config)
{
    // Reload models config.
    ModelsLoad();
}

/**
 * Precaches all models.
 */
ModelsPrecache()
{
    decl String:file[PLATFORM_MAX_PATH];

    // Loop through all models, build full path and cache them.
    for (new model = 0; model < ModelCount; model++)
    {
        ModelsGetFullPath(model, file, sizeof(file));
        PrecacheModel(file);
    }
}

/**
 * Returns a random model index according to the specified filter settings.
 *
 * @param client                Optional. Client index used to check for
 *                              permissions in "group" or "admins" access mode.
 *                              Use negative index to disable permission check
 *                              (default).
 * @param teamFilter            Optional. Team filtering settings. Use 
 *                              ModelTeam_Invalid to disable filter. Default is
 *                              ModelTeam_Zombies.
 * @param accessRequireFlags    Optional. One or more required access flags.
 *                              Default is MODEL_ACCESS_PUBLIC.
 * @return              Random model index according to filter, or -1 on error.
 */
ModelsGetRandomModel(client = -1, ModelTeam:teamFilter = ModelTeam_Zombies, accessRequireFlags = MODEL_ACCESS_PUBLIC)
{
    decl modelIndexes[MODELS_MAX];
    new listCount;
    
    // Loop through all models.
    for (new index = 0; index < ModelCount; index++)
    {
        // Check team filtering. Skip if no match.
        if (teamFilter != ModelTeam_Invalid &&
            ModelsGetTeam(index) != teamFilter)
        {
            continue;
        }
        
        // Cache current model access flag.
        new ModelAccess:access = ModelsGetAccess(index);
        new accessFlag = ModelsGetAccessFlag(access);
        
        // Check access filtering. Skip if no match.
        if (accessRequireFlags > 0 &&
            !(accessRequireFlags & accessFlag))
        {
            continue;
        }
        
        // Do client group authentication if client is specified.
        if (client > 0)
        {
            // Check if current model use group authentication.
            if (access == ModelAccess_Group)
            {
                decl String:group[64];
                ModelsGetGroup(index, group, sizeof(group));
                
                if (!ZRIsClientInGroup(client, group))
                {
                    // Client not authorized to use this model.
                    continue;
                }
            }
            else
            {
                // No group authentication. Do regular authentication if model
                // is a admin model.
                if (access == ModelAccess_Admins &&
                    !ZRIsClientAdmin(client))
                {
                    // Client not authorized to use this model.
                    continue;
                }
            }
        }
        
        // Model passed filter tests. Add to list.
        modelIndexes[listCount] = index;
        listCount++;
    }
    
    // Check if any models passed the filter.
    if (listCount)
    {
        return modelIndexes[GetRandomInt(0, listCount - 1)];
    }
    else
    {
        return -1;
    }
}

/**
 * Validates the specified index according to maximum number of models, and
 * number of models in use. Unused indexes will fail validation by default.
 *
 * @param index         Model index to validate.
 * @param rangeOnly     Optional. Do not check if the index is in use. Default
 *                      is false, check if in use.
 * @return              True if valid, false otherwise.
 */
bool:ModelsIsValidIndex(index, bool:rangeOnly = false)
{
    new bool:rangeValid = (index >= 0 && index < MODELS_MAX);
    
    if (rangeOnly)
    {
        // Only check if the index is valid.
        return rangeValid;
    }
    else
    {
        // Check if the index is valid, and if it's in use.
        return rangeValid && (index < ModelCount);
    }
}

/**
 * Gets the name for the specified model.
 *
 * @param index     Model index.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written, or -1 on error.
 */
ModelsGetName(index, String:buffer[], maxlen)
{
    // Validate index.
    if (!ModelsIsValidIndex(index))
    {
        return -1;
    }
    
    return strcopy(buffer, maxlen, ModelData[index][Model_Name]);
}

/**
 * Gets the path for the specified model.
 *
 * @param index     Model index.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written, or -1 on error.
 */
ModelsGetPath(index, String:buffer[], maxlen)
{
    // Validate index.
    if (!ModelsIsValidIndex(index))
    {
        return -1;
    }
    
    return strcopy(buffer, maxlen, ModelData[index][Model_Path]);
}

/**
 * Gets the team for the specified model.
 *
 * @param index     Model index.
 * @return          Team for the specified model, ModelTeam_Invalid on error.
 */
ModelTeam:ModelsGetTeam(index)
{
    // Validate index.
    if (!ModelsIsValidIndex(index))
    {
        return ModelTeam_Invalid;
    }
    
    return ModelData[index][Model_Team];
}

/**
 * Gets the access setting for the specified model.
 *
 * @param index     Model index.
 * @return          Access setting for the specified model, ModelAccess_Invalid
 *                  on error.
 */
ModelAccess:ModelsGetAccess(index)
{
    // Validate index.
    if (!ModelsIsValidIndex(index))
    {
        return ModelAccess_Invalid;
    }
    
    return ModelData[index][Model_Access];
}

/**
 * Gets the access flag for the specified access setting.
 *
 * @param access    Access setting to convert.
 * @return          Access flag, or 0 on error.
 */
ModelsGetAccessFlag(ModelAccess:access)
{
    switch (access)
    {
        case ModelAccess_Public:
        {
            return MODEL_ACCESS_PUBLIC;
        }
        case ModelAccess_Admins:
        {
            return MODEL_ACCESS_ADMINS;
        }
        case ModelAccess_Hidden:
        {
            return MODEL_ACCESS_HIDDEN;
        }
        case ModelAccess_MotherZombies:
        {
            return MODEL_ACCESS_MOTHER_ZOMBIES;
        }
        case ModelAccess_Group:
        {
            return MODEL_ACCESS_GROUP;
        }
    }
    
    // Invalid access flag.
    return 0;
}

/**
 * Gets the group for the specified model.
 *
 * @param index     Model index.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written, or -1 on error.
 */
ModelsGetGroup(index, String:buffer[], maxlen)
{
    // Validate index.
    if (!ModelsIsValidIndex(index))
    {
        return -1;
    }
    
    return strcopy(buffer, maxlen, ModelData[index][Model_Group]);
}

/**
 * Gets the full model file path for the specified model.
 *
 * @param index     Model index.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @return          Number of cells written, or -1 on error.
 */
ModelsGetFullPath(index, String:buffer[], maxlen)
{
    decl String:path[PLATFORM_MAX_PATH];
    decl String:name[64];
    
    ModelsGetPath(index, path, sizeof(path));
    ModelsGetName(index, name, sizeof(name));
    
    buffer[0] = 0;
    
    StrCat(buffer, maxlen, path);
    StrCat(buffer, maxlen, name);
    StrCat(buffer, maxlen, ".mdl");
}

/**
 * Converts the specified string to a team setting.
 *
 * @param team  String to convert.
 * @return      Team setting, or ModelTeam_Invalid on error.
 */
ModelTeam:ModelsStringToTeam(const String:team[])
{
    if (StrEqual(team, "zombies", false))
    {
        return ModelTeam_Zombies;
    }
    else if (StrEqual(team, "humans", false))
    {
        return ModelTeam_Humans;
    }
    
    return ModelTeam_Invalid;
}

/**
 * Converts the specified class team ID to a team setting.
 *
 * @param teamid    Class team ID.
 * @return          Team setting, or ModelTeam_Invalid on error.
 */
ModelTeam:ModelsTeamIdToTeam(teamid)
{
    switch (teamid)
    {
        case ZR_CLASS_TEAM_ZOMBIES:
        {
            return ModelTeam_Zombies;
        }
        case ZR_CLASS_TEAM_HUMANS:
        {
            return ModelTeam_Humans;
        }
    }
    
    return ModelTeam_Invalid;
}

/**
 * Converts the specified string to a access setting.
 *
 * @param access    String to convert.
 * @return          Access setting, or ModelAccess_Invalid on error.
 */
ModelAccess:ModelsStringToAccess(const String:access[])
{
    if (StrEqual(access, "public", false))
    {
        return ModelAccess_Public;
    }
    else if (StrEqual(access, "admins", false))
    {
        return ModelAccess_Admins;
    }
    else if (StrEqual(access, "hidden", false))
    {
        return ModelAccess_Hidden;
    }
    else if (StrEqual(access, "motherzombies", false))
    {
        return ModelAccess_MotherZombies;
    }
    else if (StrEqual(access, "group", false))
    {
        return ModelAccess_Group;
    }
    
    return ModelAccess_Invalid;
}

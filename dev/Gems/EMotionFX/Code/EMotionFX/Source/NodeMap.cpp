/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

// include required headers
#include "NodeMap.h"
#include "Actor.h"
#include "EMotionFXManager.h"
#include "Importer/Importer.h"
#include "Importer/NodeMapFileFormat.h"
#include <MCore/Source/DiskFile.h>
#include <AzFramework/API/ApplicationAPI.h>


namespace EMotionFX
{
    // constructor
    NodeMap::NodeMap()
        : BaseObject()
    {
        mAutoDeleteSourceActor  = true;
        mSourceActor            = nullptr;
    }


    // destructor
    NodeMap::~NodeMap()
    {
        // get rid of the source actor if we should and if there is even one
        if (mAutoDeleteSourceActor && mSourceActor)
        {
            mSourceActor->Destroy();
        }
    }


    // creation
    NodeMap* NodeMap::Create()
    {
        return new NodeMap();
    }


    // preallocate space
    void NodeMap::Reserve(uint32 numEntries)
    {
        mEntries.Reserve(numEntries);
    }


    // resize the entries array
    void NodeMap::Resize(uint32 numEntries)
    {
        mEntries.Resize(numEntries);
    }


    // modify the first name of a given entry
    void NodeMap::SetFirstName(uint32 entryIndex, const char* name)
    {
        mEntries[entryIndex].mFirstNameID = MCore::GetStringIdPool().GenerateIdForString(name);
    }


    // modify the second name
    void NodeMap::SetSecondName(uint32 entryIndex, const char* name)
    {
        mEntries[entryIndex].mSecondNameID = MCore::GetStringIdPool().GenerateIdForString(name);
    }


    // modify a given entry
    void NodeMap::SetEntry(uint32 entryIndex, const char* firstName, const char* secondName)
    {
        mEntries[entryIndex].mFirstNameID = MCore::GetStringIdPool().GenerateIdForString(firstName);
        mEntries[entryIndex].mSecondNameID = MCore::GetStringIdPool().GenerateIdForString(secondName);
    }


    // set the entry of the firstName item to the second name
    void NodeMap::SetEntry(const char* firstName, const char* secondName, bool addIfNotExists)
    {
        // check if there is already an entry for this name
        const uint32 entryIndex = FindEntryIndexByName(firstName);
        if (entryIndex == MCORE_INVALIDINDEX32)
        {
            // if there is no such entry yet, and we also don't want to add a new one, then there is nothing to do
            if (addIfNotExists == false)
            {
                return;
            }

            // add a new entry
            AddEntry(firstName, secondName);
            return;
        }

        // modify it
        SetSecondName(entryIndex, secondName);
    }


    // add an entry
    void NodeMap::AddEntry(const char* firstName, const char* secondName)
    {
        MCORE_ASSERT(GetHasEntry(firstName) == false);  // prevent duplicates
        mEntries.AddEmpty();
        SetEntry(mEntries.GetLength() - 1, firstName, secondName);
    }


    // remove a given entry by its index
    void NodeMap::RemoveEntryByIndex(uint32 entryIndex)
    {
        mEntries.Remove(entryIndex);
    }


    // remove a given entry by its name
    void NodeMap::RemoveEntryByName(const char* firstName)
    {
        const uint32 entryIndex = FindEntryIndexByName(firstName);
        if (entryIndex == MCORE_INVALIDINDEX32)
        {
            return;
        }

        mEntries.Remove(entryIndex);
    }


    // remove a given entry by its name ID
    void NodeMap::RemoveEntryByNameID(uint32 firstNameID)
    {
        const uint32 entryIndex = FindEntryIndexByNameID(firstNameID);
        if (entryIndex == MCORE_INVALIDINDEX32)
        {
            return;
        }

        mEntries.Remove(entryIndex);
    }


    // set the filename
    void NodeMap::SetFileName(const char* fileName)
    {
        mFileName = fileName;
    }


    // get the filename
    const char* NodeMap::GetFileName() const
    {
        return mFileName.c_str();
    }


    // get the filename
    const AZStd::string& NodeMap::GetFileNameString() const
    {
        return mFileName;
    }


    // try to write a string to the file
    bool NodeMap::WriteFileString(MCore::DiskFile* f, const AZStd::string& textToSave, MCore::Endian::EEndianType targetEndianType) const
    {
        MCORE_ASSERT(f);

        // convert endian and write the number of characters that follow
        if (textToSave.size() == 0)
        {
            uint32 numCharacters = 0;
            MCore::Endian::ConvertUnsignedInt32To(&numCharacters, targetEndianType);
            if (f->Write(&numCharacters, sizeof(uint32)) == 0)
            {
                return false;
            }

            return true;
        }
        else
        {
            uint32 numCharacters = static_cast<uint32>(textToSave.size());
            MCore::Endian::ConvertUnsignedInt32To(&numCharacters, targetEndianType);
            if (f->Write(&numCharacters, sizeof(uint32)) == 0)
            {
                return false;
            }
        }

        // write the string in UTF8 format
        if (f->Write(textToSave.c_str(), textToSave.size()) == 0)
        {
            return false;
        }

        return true;
    }


    // calculate the size of a string on disk
    uint32 NodeMap::CalcFileStringSize(const AZStd::string& text) const
    {
        return static_cast<uint32>(sizeof(uint32) + text.size() * sizeof(char));
    }


    // calculate the chunk size
    uint32 NodeMap::CalcFileChunkSize() const
    {
        // add the node map info header
        uint32 numBytes = sizeof(FileFormat::NodeMapChunk);

        // for all entries
        const uint32 numEntries = mEntries.GetLength();
        for (uint32 i = 0; i < numEntries; ++i)
        {
            numBytes += CalcFileStringSize(GetFirstNameString(i));
            numBytes += CalcFileStringSize(GetSecondNameString(i));
        }

        // return the number of bytes
        return numBytes;
    }


    // build a node map
    bool NodeMap::Save(const char* fileName, MCore::Endian::EEndianType targetEndianType) const
    {
        // try to create the file
        MCore::DiskFile f;
        if (f.Open(fileName, MCore::DiskFile::WRITE) == false)
        {
            MCore::LogError("NodeMap::Save() - Cannot write to file '%s', is the file maybe in use by another application?", fileName);
            return false;
        }

        // try to write the file header
        FileFormat::NodeMap_Header header;
        header.mFourCC[0] = 'N';
        header.mFourCC[1] = 'O';
        header.mFourCC[2] = 'M';
        header.mFourCC[3] = 'P';
        header.mHiVersion   = 1;
        header.mLoVersion   = 0;
        header.mEndianType  = (uint8)targetEndianType;
        if (f.Write(&header, sizeof(FileFormat::NodeMap_Header)) == 0)
        {
            MCore::LogError("NodeMap::Save() - Cannot write the header to file '%s', is the file maybe in use by another application?", fileName);
            return false;
        }

        // write the chunk header
        FileFormat::FileChunk chunkHeader;
        chunkHeader.mChunkID        = FileFormat::CHUNK_NODEMAP;
        chunkHeader.mVersion        = 1;
        chunkHeader.mSizeInBytes    = CalcFileChunkSize();// calculate the chunk size
        MCore::Endian::ConvertUnsignedInt32To(&chunkHeader.mChunkID,       targetEndianType);
        MCore::Endian::ConvertUnsignedInt32To(&chunkHeader.mSizeInBytes,   targetEndianType);
        MCore::Endian::ConvertUnsignedInt32To(&chunkHeader.mVersion,       targetEndianType);
        if (f.Write(&chunkHeader, sizeof(FileFormat::FileChunk)) == 0)
        {
            MCore::LogError("NodeMap::Save() - Cannot write the chunk header to file '%s', is the file maybe in use by another application?", fileName);
            return false;
        }

        // the main info
        FileFormat::NodeMapChunk nodeMapChunk;
        nodeMapChunk.mNumEntries = mEntries.GetLength();
        MCore::Endian::ConvertUnsignedInt32To(&nodeMapChunk.mNumEntries, targetEndianType);
        if (f.Write(&nodeMapChunk, sizeof(FileFormat::NodeMapChunk)) == 0)
        {
            MCore::LogError("NodeMap::Save() - Cannot write the node map chunk to file '%s', is the file maybe in use by another application?", fileName);
            return false;
        }

        // write the source actor string
        if (WriteFileString(&f, mSourceActorFileName, targetEndianType) == false)
        {
            MCore::LogError("NodeMap::Save() - Cannot write the source actor filename string '%s' to node map file.", mSourceActorFileName.c_str());
            return false;
        }

        // for all entries
        const uint32 numEntries = mEntries.GetLength();
        for (uint32 i = 0; i < numEntries; ++i)
        {
            if (WriteFileString(&f, GetFirstNameString(i), targetEndianType) == false)
            {
                MCore::LogError("NodeMap::Save() - Cannot write the first string to file '%s', is the file maybe in use by another application?", fileName);
                return false;
            }

            // write the second string
            if (WriteFileString(&f, GetSecondNameString(i), targetEndianType) == false)
            {
                MCore::LogError("NodeMap::Save() - Cannot write the second string to file '%s', is the file maybe in use by another application?", fileName);
                return false;
            }
        }

        // success
        f.Close();
        return true;
    }


    // set the source actor filename used
    void NodeMap::SetSourceActorFileName(const char* fileName)
    {
        mSourceActorFileName = fileName;
    }


    // update the source actor pointer
    void NodeMap::SetSourceActor(Actor* actor, bool deleteExisting)
    {
        if (deleteExisting && mSourceActor)
        {
            mSourceActor->Destroy();
        }

        mSourceActor = actor;
    }


    // get the source actor pointer
    Actor* NodeMap::GetSourceActor() const
    {
        return mSourceActor;
    }


    // enable or disable auto deletion of the source actor on destruct of the node map
    void NodeMap::SetAutoDeleteSourceActor(bool autoDelete)
    {
        mAutoDeleteSourceActor = autoDelete;
    }


    // check whether we auto delete the source actor on the node map destruct or not
    bool NodeMap::GetAutoDeleteSourceActor() const
    {
        return mAutoDeleteSourceActor;
    }


    // get the source actor filename
    const char* NodeMap::GetSourceActorFileName() const
    {
        return mSourceActorFileName.c_str();
    }


    // get the source actor filename as string object
    const AZStd::string& NodeMap::GetSourceActorFileNameString() const
    {
        return mSourceActorFileName;
    }


    // try to load the source actor based on the source actor string
    bool NodeMap::LoadSourceActor()
    {
        // check if the source actor filename has been set
        if (mSourceActorFileName.empty())
        {
            MCore::LogInfo("EMotionFX::NodeMap::LoadSourceActor() - The source actor filename is not set, so there is nothing to load for nodemap '%s'...", mFileName.c_str());
            return true;
        }

        // get rid of any existing one
        if (mSourceActor)
        {
            mSourceActor->Destroy();
            mSourceActor = nullptr;
        }

        // try to load the source actor, in a minimal way so we just have the hierarchy available
        Importer::ActorSettings settings;
        settings.mLoadGeometryLODs          = false;
        settings.mLoadMeshes                = false;
        settings.mLoadMorphTargets          = false;
        settings.mLoadStandardMaterialLayers= false;
        settings.mLoadSkinningInfo          = false;
        settings.mLoadCollisionMeshes       = false;

        // Build the filename and load the actor.
        AZStd::string filename = GetEMotionFX().ConstructAbsoluteFilename(mSourceActorFileName.c_str());
        mSourceActor = GetImporter().LoadActor(filename.c_str(), &settings);
        return (mSourceActor != nullptr);
    }


    // get the number of entries
    uint32 NodeMap::GetNumEntries() const
    {
        return mEntries.GetLength();
    }


    // get the first name as char pointer
    const char* NodeMap::GetFirstName(uint32 entryIndex) const
    {
        return MCore::GetStringIdPool().GetName(mEntries[entryIndex].mFirstNameID).c_str();
    }


    // get the second node name as char pointer
    const char* NodeMap::GetSecondName(uint32 entryIndex) const
    {
        return MCore::GetStringIdPool().GetName(mEntries[entryIndex].mSecondNameID).c_str();
    }


    // get the first node name as string
    const AZStd::string& NodeMap::GetFirstNameString(uint32 entryIndex) const
    {
        return MCore::GetStringIdPool().GetName(mEntries[entryIndex].mFirstNameID);
    }


    // get the second node name as string
    const AZStd::string& NodeMap::GetSecondNameString(uint32 entryIndex) const
    {
        return MCore::GetStringIdPool().GetName(mEntries[entryIndex].mSecondNameID);
    }


    // check if we already have an entry for this name
    bool NodeMap::GetHasEntry(const char* firstName) const
    {
        return (FindEntryIndexByName(firstName) != MCORE_INVALIDINDEX32);
    }


    // find an entry index by its name
    uint32 NodeMap::FindEntryIndexByName(const char* firstName) const
    {
        const uint32 numEntries = mEntries.GetLength();
        for (uint32 i = 0; i < numEntries; ++i)
        {
            const AZStd::string& firstNameEntry = GetFirstName(i);
            if (firstNameEntry == firstName)
            {
                return i;
            }
        }

        return MCORE_INVALIDINDEX32;
    }


    // find an entry index by its name ID
    uint32 NodeMap::FindEntryIndexByNameID(uint32 firstNameID) const
    {
        const uint32 numEntries = mEntries.GetLength();
        for (uint32 i = 0; i < numEntries; ++i)
        {
            if (mEntries[i].mFirstNameID == firstNameID)
            {
                return i;
            }
        }

        return MCORE_INVALIDINDEX32;
    }


    // find the second name for a given first name
    const char* NodeMap::FindSecondName(const char* firstName) const
    {
        const uint32 entryIndex = FindEntryIndexByName(firstName);
        if (entryIndex == MCORE_INVALIDINDEX32)
        {
            return nullptr;
        }

        return GetSecondName(entryIndex);
    }


    // find the second name based on a first given name
    void NodeMap::FindSecondName(const char* firstName, AZStd::string* outString)
    {
        const uint32 entryIndex = FindEntryIndexByName(firstName);
        if (entryIndex == MCORE_INVALIDINDEX32)
        {
            outString->clear();
            return;
        }

        *outString = GetSecondNameString(entryIndex);
    }
}   // namespace EMotionFX

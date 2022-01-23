#include "mod-audio.h"

#include "moon/api/fs/moonfs.h"
#include "libs/nlohmann/json.hpp"
#include "moon/addons/engine.h"
#include "moon/api/hooks/hook.h"
#include "moon/addons/interfaces/sound-entry.h"
#include "moon/addons/interfaces/file-entry.h"

extern "C" {
#include "text/libs/io_utils.h"
#include "pc/platform.h"
#include "pc/fs/fs.h"
#include "PR/libaudio.h"
#include "audio/load.h"
#include "audio/external.h"

#include "game/sound_init.h"
#include "game/level_update.h"
#include "game/area.h"
}

#include <iostream>
#include <vector>
#include <map>

using namespace std;
using json = nlohmann::json;

u8 backgroundQueueSize = 0;


vector<BitModule*> soundCache;

namespace Moon {
    void saveAddonSound(BitModule *addon, std::string soundPath, EntryFileData* data){
        if(addon->sounds == nullptr)
            addon->sounds = new SoundEntry();

        SoundEntry* soundEntry = addon->sounds;
        ALSeqFile* header = (ALSeqFile*) data->data;

        if(soundPath == "sound/sequences.bin"){
            soundEntry->seqHeader = header;
            alSeqFileNew(soundEntry->seqHeader, (u8*) data->data);
            soundEntry->seqCount = header->seqCount;
            return;
        }
        if(soundPath == "sound/sound_data.ctl"){
            soundEntry->ctlHeader = header;
            alSeqFileNew(soundEntry->ctlHeader, (u8*) data->data);
            soundEntry->ctlEntries = new CtlEntry[header->seqCount];
            return;
        }
        if(soundPath == "sound/sound_data.tbl"){
            soundEntry->tblHeader = header;
            alSeqFileNew(soundEntry->tblHeader, (u8*) data->data);
            return;
        }
        if(soundPath == "sound/bank_sets"){
            soundEntry->bankSets = (u8*) data->data;
            return;
        }
    }
}

namespace Moon {
    void setSoundEntry(SoundEntry *entry) {
        gSeqFileHeader = entry->seqHeader;
        gSequenceCount = entry->seqCount;

        gAlCtlHeader   = entry->ctlHeader;
        gCtlEntries    = entry->ctlEntries;

        gAlTbl         = entry->tblHeader;
        gAlBankSets    = entry->bankSets;
    }
}

namespace MoonInternal {

    void buildAudioCache(vector<int> order){
        soundCache.clear();

        for(int i = 0; i < order.size(); i++){
            BitModule* addon = Moon::addons[order[i]];
            cout << "Is null: " << (addon->sounds == nullptr) << endl;
            if(addon->sounds != nullptr)
                soundCache.push_back(addon);
        }

        Moon::setSoundEntry(soundCache[soundCache.size() - 1]->sounds);
    }

    void resetSoundSystem(){
        sound_reset(0);
        play_music(SEQ_PLAYER_LEVEL, gCurrentArea->musicParam2, 0);
        lower_background_noise(1);
    }

    void setupSoundEngine( string state ){
        if(state == "Exit"){
            for(auto &addon : Moon::addons){
                delete addon->sounds;
            }
        }
    }
}
#pragma once

#include <set>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#define SLIDER_OFFSET 200
#define SLIDER_CATEGORY_EXTRA 512
#define SLIDER_CATEGORY_EXPRESSIONS 1024

#define SLIDER_MOD_DIRECTORY "actors\\character\\FaceGenMorphs\\"
#define SLIDER_DIRECTORY "actors\\character\\FaceGenMorphs\\morphs\\"

#define MORPH_CACHE_TEMPLATE "%08X.tri"
#define MORPH_CACHE_DIR "cache\\"
#define MORPH_CACHE_PATH "actors\\character\\FaceGenMorphs\\morphs\\cache\\"

namespace RM
{
    class BSFaceGenNiNode;
    class TESNPC;
    class SliderArray;
    class RaceMenuSlider;
    struct SKSESerializationInterface;
    class TESRace;
    class BGSHeadPart;
    class TESForm;
    class TESModelTri;

    class IPluginInterface
    {
    public:
        IPluginInterface() { };
        virtual ~IPluginInterface() { };

        virtual uint32_t GetVersion() = 0;
        virtual void Revert() = 0;
    };

    class IInterfaceMap 
    {
    public:
        virtual IPluginInterface * QueryInterface(const char * name) = 0;
        virtual bool AddInterface(const char * name, IPluginInterface * pluginInterface) = 0;
        virtual IPluginInterface * RemoveInterface(const char * name) = 0;
    };

    struct InterfaceExchangeMessage
    {
        enum
        {
            kMessage_ExchangeInterface = 0x9E3779B9
        };

        IInterfaceMap * interfaceMap = nullptr;
    };



    inline size_t hash_lower(const char* str, size_t count)
    {
        const size_t _FNV_offset_basis = 14695981039346656037ULL;
        const size_t _FNV_prime = 1099511628211ULL;

        size_t _Val = _FNV_offset_basis;
        for (size_t _Next = 0; _Next < count; ++_Next)
        {    // fold in another byte
            _Val ^= (size_t)tolower(str[_Next]);
            _Val *= _FNV_prime;
        }
        return _Val;
    }

    class SKEEFixedString
    {
    public:
        SKEEFixedString() : m_internal() { m_hash = hash_lower(m_internal.c_str(), m_internal.size()); }
        SKEEFixedString(const char * str) : m_internal(str) { m_hash = hash_lower(m_internal.c_str(), m_internal.size()); }
        SKEEFixedString(const std::string & str) : m_internal(str) { m_hash = hash_lower(m_internal.c_str(), m_internal.size()); }
        SKEEFixedString(const RE::BSFixedString & str) : m_internal(str.c_str()) { m_hash = hash_lower(m_internal.c_str(), m_internal.size()); }
    
        bool operator<(const SKEEFixedString& x) const
        {
            return _stricmp(m_internal.c_str(), x.m_internal.c_str()) < 0;
        }
    
        bool operator==(const SKEEFixedString& x) const
        {
            if (m_internal.size() != x.m_internal.size())
                return false;
    
            if (_stricmp(m_internal.c_str(), x.m_internal.c_str()) == 0)
                return true;
    
            return false;
        }
    
        size_t length() const { return m_internal.size(); }
    
        std::string AsString() const { return m_internal; }
        //SKEEFixedString operator BSFixedString() const { return RE::BSFixedString(m_internal.c_str()); }
        //RE::BSFixedString AsBSFixedString() const { return operator RE::BSFixedString(); }
    
        const char * c_str() const { return operator const char *(); }
        operator const char *() const { return m_internal.c_str(); }
    
        size_t GetHash() const
        {
            return m_hash;
        }

    protected:
        std::string        m_internal;
        size_t            m_hash;
    };

    typedef std::shared_ptr<SKEEFixedString> StringTableItem;

}

namespace std 
{
    template <> struct std::hash<RM::SKEEFixedString>
    {
        size_t operator()(const RM::SKEEFixedString & x) const
        {
            return x.GetHash();
        }
    };
    template <> struct std::hash<RM::StringTableItem>
    {
        size_t operator()(const RM::StringTableItem & x) const
        {
            return x->GetHash();
        }
    };
}

namespace RM
{
    class SliderInternal
    {
    public:
        SliderInternal()
        {
            category = -1;
            name = "";
            lowerBound = "";
            upperBound = "";
            type = 0;
            presetCount = 0;
        }

        void copy(SliderInternal * slider)
        {
            category = slider->category;
            name = slider->name;
            lowerBound = slider->lowerBound;
            upperBound = slider->upperBound;
            type = slider->type;
            presetCount = slider->presetCount;
        }

        enum
        {
            kCategoryExpressions = SLIDER_CATEGORY_EXPRESSIONS,
            kCategoryExtra = SLIDER_CATEGORY_EXTRA,
            kCategoryBody = 4,
            kCategoryHead = 8,
            kCategoryFace = 16,
            kCategoryEyes = 32,
            kCategoryBrow = 64,
            kCategoryMouth = 128,
            kCategoryHair = 256
        };

        enum 
        {
            kTypeSlider = 0,
            kTypePreset = 1,
            kTypeHeadPart = 2
        };

        int32_t                category;
        SKEEFixedString        name;
        SKEEFixedString        lowerBound;
        SKEEFixedString        upperBound;
        uint8_t                type;
        uint8_t                presetCount;
    };

    typedef std::shared_ptr<SliderInternal> SliderInternalPtr;

    class SliderGender
    {
    public:
        SliderGender()
        {
            slider[0] = NULL;
            slider[1] = NULL;
        }

        SliderInternalPtr slider[2];
    };
    typedef std::shared_ptr<SliderGender> SliderGenderPtr;
    typedef std::vector<SliderInternalPtr> SliderList;
    typedef std::unordered_map<TESRace*, SliderList> RaceSliders;
    typedef std::vector<SKEEFixedString> MorphSet;

    class MorphMap : public std::map<SKEEFixedString, MorphSet>
    {
    public:
        class Visitor
        {
        public:
            virtual bool Accept(const SKEEFixedString & morphName) { return false; };
        };
    
        void AddMorph(const SKEEFixedString & key, const SKEEFixedString & value);
        bool Visit(const SKEEFixedString & key, Visitor & visitor);
        void Revert();
    };

    class SliderMap : public std::unordered_map<SKEEFixedString, SliderGenderPtr>
    {
    public:
        SliderMap() : std::unordered_map<SKEEFixedString, SliderGenderPtr>(){ }

        void AddSlider(const SKEEFixedString & key, uint8_t gender, SliderInternal & slider);

    #ifdef _DEBUG_DATADUMP
        void DumpMap();
    #endif
    };

    typedef std::shared_ptr<SliderMap>    SliderMapPtr;


    class SliderSet : public std::set<SliderMapPtr>
    {
    public:
        bool for_each_slider(std::function<bool(SliderGenderPtr)> func);
    };

    typedef std::shared_ptr<SliderSet> SliderSetPtr;

    class RaceMap : public std::unordered_map<TESRace*, SliderSetPtr>
    {
    public:
        SliderSetPtr GetSliderSet(TESRace * race);
        bool AddSliderMap(TESRace * race, SliderMapPtr sliderMap);
        bool CreateDefaultMap(TESRace * race);

        void Revert();

    #ifdef _DEBUG_DATADUMP
        void DumpMap();
    #endif
    };

    class ValueSet : public std::unordered_map<StringTableItem, float>
    {
    public:
        void SetValue(const SKEEFixedString & name, float value);
        void ClearValue(const SKEEFixedString & name);
        float GetValue(const SKEEFixedString & name);
    };

    class ValueMap : public std::unordered_map<RE::TESNPC*, ValueSet>
    {
    public:
        ValueSet * GetValueSet(RE::TESNPC* npc);
        void EraseNPC(RE::TESNPC* npc);

        float GetMorphValueByName(RE::TESNPC* npc, const SKEEFixedString & name);
        void SetMorphValue(RE::TESNPC* npc, const SKEEFixedString & name, float value);
    };

    #define VERTEX_THRESHOLD 0.00001
    #define VERTEX_MULTIPLIER 10000

    class MappedSculptData : public std::unordered_map<uint16_t, RE::NiPoint3>
    {
    public:
        void force_insert(value_type const & v)
        {
            if (abs(v.second.x) < VERTEX_THRESHOLD && abs(v.second.y) < VERTEX_THRESHOLD && abs(v.second.z) < VERTEX_THRESHOLD)
                return;

            auto res = insert(v);
            if (!res.second)
                (*res.first).second = v.second;
        }

        void add(value_type const & v)
        {
            auto res = insert(v);
            if (!res.second)
                (*res.first).second += v.second;

            if (abs((*res.first).second.x) < VERTEX_THRESHOLD && abs((*res.first).second.y) < VERTEX_THRESHOLD && abs((*res.first).second.z) < VERTEX_THRESHOLD)
                erase(res.first);
        }
    };
    typedef std::shared_ptr<MappedSculptData> MappedSculptDataPtr;

    class SculptData : public std::unordered_map<StringTableItem, MappedSculptDataPtr>
    {
    public:
        MappedSculptDataPtr GetSculptHost(SKEEFixedString, bool create = true);

        static SKEEFixedString GetHostByPart(BGSHeadPart * headPart);
    };
    using SculptDataPtr = std::shared_ptr<SculptData>;

    class SculptStorage : public std::unordered_map < TESNPC*, SculptDataPtr >
    {
    public:
        void SetSculptTarget(TESNPC * npc, SculptDataPtr sculptData);
        SculptDataPtr GetSculptTarget(TESNPC* npc, bool create = true);
        void EraseNPC(TESNPC * npc);
    };

    class TRIFile
    {
    public:
        TRIFile()
        {
            vertexCount = -1;
        }

        bool Load(const char * triPath);
        bool Apply(RE::BSGeometry * geometry, SKEEFixedString morph, float relative);

        struct Morph
        {
            SKEEFixedString name;
            float multiplier;
        
            struct Vertex
            {
                int16_t x, y, z;
            };

            std::vector<Vertex> vertices;
        };

        int32_t vertexCount;
        std::unordered_map<SKEEFixedString, Morph> morphs;
    };

    class TRIModelData
    {
    public:
        TRIModelData()
        {
            vertexCount = -1;
            morphModel = NULL;
        }
        int32_t vertexCount;
        std::shared_ptr<TRIFile> triFile;
        TESModelTri * morphModel;
    };

    typedef std::unordered_map<SKEEFixedString, TRIModelData> ModelMap;

    class FaceMorphInterface : public IPluginInterface
    {
    public:
        //uintptr_t vtable;
        SliderList * currentList;
        RaceSliders m_internalMap;
        RaceMap m_raceMap;
        MorphMap m_morphMap;
        ValueMap m_valueMap;
        ModelMap m_modelMap;
    
        SculptStorage m_sculptStorage;
    
        friend class RacePartDefaultGen;
    };


    class IBodyMorphInterface : public IPluginInterface
    {
    public:
        enum
        {
            kPluginVersion1 = 1,
            kPluginVersion2,
            kPluginVersion3,
            kPluginVersion4,
            kCurrentPluginVersion = kPluginVersion4,
            kSerializationVersion1 = 1,
            kSerializationVersion2,
            kSerializationVersion3,
            kSerializationVersion = kSerializationVersion3
        };
        class MorphKeyVisitor
        {
        public:
            virtual void Visit(const char*, float) = 0;
        };

        class StringVisitor
        {
        public:
            virtual void Visit(const char*) = 0;
        };

        class ActorVisitor
        {
        public:
            virtual void Visit(RE::TESObjectREFR*) = 0;
        };

        class MorphValueVisitor
        {
        public:
            virtual void Visit(RE::TESObjectREFR *, const char*, const char*, float) = 0;
        };
    
        class MorphVisitor
        {
        public:
            virtual void Visit(RE::TESObjectREFR *, const char*) = 0;
        };

        virtual void SetMorph(RE::TESObjectREFR * actor, const char * morphName, const char * morphKey, float relative) = 0;
        virtual float GetMorph(RE::TESObjectREFR * actor, const char * morphName, const char * morphKey) = 0;
        virtual void ClearMorph(RE::TESObjectREFR * actor, const char * morphName, const char * morphKey) = 0;

        virtual float GetBodyMorphs(RE::TESObjectREFR * actor, const char * morphName) = 0;
        virtual void ClearBodyMorphNames(RE::TESObjectREFR * actor, const char * morphName) = 0;

        virtual void VisitMorphs(RE::TESObjectREFR * actor, MorphVisitor & visitor) = 0;
        virtual void VisitKeys(RE::TESObjectREFR * actor, const char * name, MorphKeyVisitor & visitor) = 0;
        virtual void VisitMorphValues(RE::TESObjectREFR * actor, MorphValueVisitor & visitor) = 0;

        virtual void ClearMorphs(RE::TESObjectREFR * actor) = 0;

        virtual void ApplyVertexDiff(RE::TESObjectREFR * refr, RE::NiAVObject * rootNode, bool erase = false) = 0;

        virtual void ApplyBodyMorphs(RE::TESObjectREFR * refr, bool deferUpdate = true) = 0;
        virtual void UpdateModelWeight(RE::TESObjectREFR * refr, bool immediate = false) = 0;

        virtual void SetCacheLimit(uint64_t limit) = 0;
        virtual bool HasMorphs(RE::TESObjectREFR * actor) = 0;
        virtual uint32_t EvaluateBodyMorphs(RE::TESObjectREFR * actor) = 0;

        virtual bool HasBodyMorph(RE::TESObjectREFR * actor, const char * morphName, const char * morphKey) = 0;
        virtual bool HasBodyMorphName(RE::TESObjectREFR * actor, const char * morphName) = 0;
        virtual bool HasBodyMorphKey(RE::TESObjectREFR * actor, const char * morphKey) = 0;
        virtual void ClearBodyMorphKeys(RE::TESObjectREFR * actor, const char * morphKey) = 0;
        virtual void VisitStrings(StringVisitor & visitor) = 0;
        virtual void VisitActors(ActorVisitor & visitor) = 0;
        virtual uint64_t ClearMorphCache() = 0;
    };
}

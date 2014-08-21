#ifndef _GARRISON_H_INCLUDED_
#define _GARRISON_H_INCLUDED_

#include "DB2Stores.h"

enum GarrisonFactionIndex
{
    GARRISON_FACTION_HORDE      = 0,
    GARRISON_FACTION_ALLIANCE   = 1
};

enum GarrisonMissionState
{
    GARRISON_MISSION_AVAILABLE      = 0,
    GARRISON_MISSION_IN_PROGRESS    = 1,
    GARRISON_MISSION_COMPLETE       = 2
};

enum GarrisonLearnBluePrintResult
{
    GARRISON_LEARN_BLUEPRINT_LEARNED            = 0,
    GARRISON_LEARN_BLUEPRINT_UNABLE_TO_LEARN    = 1,
    GARRISON_LEARN_BLUEPRINT_KNOWN              = 21
};

enum GarrisonPlotType
{
    GARRISON_PLOT_TYPE_SMALL    = 0,
    GARRISON_PLOT_TYPE_MEDIUM   = 1,
    GARRISON_PLOT_TYPE_LARGE    = 2,
    GARRISON_PLOT_TYPE_MAX      = 3,
};

enum GarrisonPurchaseBuildingResult
{
    GARRISON_PURCHASE_BUILDING_OK                       = 0,
    GARRISON_PURCHASE_BUILDING_BUILDING_EXIST           = 24,
    GARRISON_PURCHASE_BUILDING_INVALID_PLOT             = 10,
    GARRISON_PURCHASE_BUILDING_INVALID_BUILDING_ID      = 11,
    GARRISON_PURCHASE_BUILDING_INVALID_PLOT_BUILDING    = 16,
    GARRISON_PURCHASE_BUILDING_REQUIRE_BLUE_PRINT       = 22,
    GARRISON_PURCHASE_BUILDING_NOT_ENOUGH_CURRENCY      = 46,
    GARRISON_PURCHASE_BUILDING_NOT_ENOUGH_GOLD          = 47,
};

extern uint32 gGarrisonEmptyPlotGameObject[GARRISON_PLOT_TYPE_MAX];
extern uint32 gGarrisonBuildingPlotGameObject[GARRISON_PLOT_TYPE_MAX];

#define GARRISON_PLOT_INSTANCE_COUNT 40

struct GarrisonPlotInstanceInfoLocation
{
    uint32 SiteLevelID;
    uint32 PlotInstanceID;
    float X, Y, Z;
    float O;
};

extern GarrisonPlotInstanceInfoLocation gGarrisonPlotInstanceInfoLocation[GARRISON_PLOT_INSTANCE_COUNT];

struct GarrisonMission
{
    uint64 DB_ID;
    uint32 MissionID;
    uint32 OfferTime;
    uint32 OfferMaxDuration;
    uint32 StartTime;
    GarrisonMissionState State;
};

struct GarrisonFollower
{
    uint32 CurrentMissionID;
};

struct GarrisonBuilding
{
    uint32 PlotInstanceID;
    uint32 BuildingID;
    uint32 SpecID;
    uint32 TimeBuiltStart;
    uint32 TimeBuiltEnd;

    bool Active;
};

class Player;

class Garrison
{
    public:
        /// Constructor
        Garrison(Player * p_Owner);

        /// Save this garrison to DB
        void Save();

        /// Get GarrSiteLevelEntry for current garrison
        const GarrSiteLevelEntry * GetGarrisonSiteLevelEntry();
        /// Get Garrison Faction Index
        GarrisonFactionIndex GetGarrisonFactionIndex();

        /// Get plots for level
        std::vector<GarrisonPlotInstanceInfoLocation> GetPlots();
        /// Get plot instance plot type
        uint32 GetPlotType(uint32 p_PlotInstanceID);
        /// Plot is free ?
        bool PlotIsFree(uint32 p_PlotInstanceID);
        /// Get plot location
        GarrisonPlotInstanceInfoLocation GetPlot(uint32 p_PlotInstanceID);

        /// Add mission
        bool AddMission(uint32 p_MissionRecID);
        /// Player have mission
        bool HaveMission(uint32 p_MissionRecID);
        /// Get missions
        std::vector<GarrisonMission> GetMissions();
        /// Get all completed missions
        std::vector<GarrisonMission> GetCompletedMissions();

        /// Can build building X at slot instance Y
        bool IsBuildingPlotInstanceValid(uint32 p_BuildingRecID, uint32 p_PlotInstanceID);
        /// Player fill all condition
        GarrisonPurchaseBuildingResult CanPurchaseBuilding(uint32 p_BuildingRecID);
        /// PurchaseBuilding
        GarrisonBuilding PurchaseBuilding(uint32 p_BuildingRecID, uint32 p_PlotInstanceID);
        /// Get building
        GarrisonBuilding GetBuilding(uint32 p_PlotInstanceID);

        /// Get known blueprints
        std::vector<int32> GetKnownBlueprints();
        /// Learn blue print
        bool LearnBlueprint(uint32 p_BuildingRecID);
        /// Known blue print
        bool KnownBlueprint(uint32 p_BuildingRecID);

        /// Get known specializations
        std::vector<int32> GetKnownSpecializations();

    private:
        /// Init data for level
        void InitDataForLevel();
        /// Init Game objects
        void InitGameObjects();

        /// Update plot gameobject
        void UpdatePlotGameObject(uint32 p_PlotInstanceID);

    private:
        Player *    m_Owner;            ///< Garrison owner
        uint32      m_GarrisonLevel;    ///< Garrison level
        uint32      m_GarrisonLevelID;  ///< Garrison level ID in 
        uint32      m_GarrisonSiteID;   ///< Garrison site ID

        std::vector<GarrisonPlotInstanceInfoLocation>   m_Plots;
        std::vector<GarrisonMission>                    m_Missions;
        std::vector<GarrisonFollower>                   m_Followers;
        std::vector<GarrisonBuilding>                   m_Buildings;
        std::vector<int32>                              m_KnownBlueprints;
        std::vector<int32>                              m_KnownSpecializations;

        std::map<uint32, uint64> m_PlotsGob;
};


#endif
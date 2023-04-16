#pragma once
#include "Common.h"

class XelsNaga;

namespace WorkerJobs
{
    enum { Minerals, Gas, Build, Combat, Idle, Repair, Move, Scout, None, Num };
}

class WorkerData
{
    XelsNaga & m_bot;

    std::set<UnitTag>           m_workers;
    std::set<UnitTag>           m_depots;
    std::map<int, int>          m_workerJobCount;
    std::map<UnitTag, int>      m_workerJobMap;
    std::map<UnitTag, int>      m_refineryWorkerCount;
    std::map<UnitTag, int>      m_depotWorkerCount;
    std::map<UnitTag, UnitTag>  m_workerRefineryMap;
    std::map<UnitTag, UnitTag>  m_workerDepotMap;

	std::map<sc2::Unit, bool>	m_townhall;

    void clearPreviousJob(const UnitTag & unit);

public:

    WorkerData(XelsNaga & bot);

    void    workerDestroyed(const UnitTag & unit);
    void    updateAllWorkerData();
    void    updateWorker(const UnitTag & unit);
    void    setWorkerJob(const UnitTag & unit, int job, UnitTag jobUnitTag = 0);
    void    drawDepotDebugInfo();
    size_t  getNumWorkers() const;
    int     getWorkerJobCount(int job) const;
    int     getNumAssignedWorkers(const UnitTag & unit);
    int     getWorkerJob(const UnitTag & unit) const;
    UnitTag getMineralToMine(const UnitTag & unit) const;
    UnitTag getWorkerDepot(const UnitTag & unit) const;
    const char * getJobCode(const UnitTag & unit);
    const std::set<UnitTag> & getWorkers() const;


	// add
	void		setAssignedData();
};
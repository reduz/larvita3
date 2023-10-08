//
// C++ Interface: performance_metrics
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PERFORMANCE_METRICS_H
#define PERFORMANCE_METRICS_H


#include "typedefs.h"
#include "table.h"
#include "list.h"


/**
	@author ,,, <red@lunatea>
*/

SCRIPT_BIND_BEGIN;

class PerformanceMetrics {
public:

	enum Section {
	
		SECTION_PROCESS,
		SECTION_PHYSICS,
		SECTION_DRAW,
		SECTION_MAX
	};

	SCRIPT_BIND_END;
	
private:
	struct Measure {
	
		Color color;
		Uint64 total;
		Uint64 last_frame;
		Uint64 frame;
		Uint64 from;

		Uint64 max;
		Uint64 min;
		Uint64 average;
		int count;
			
		Measure() { total=frame=from=min=max=average=count=0; }
	};

	
	static Table<String,Measure> section_perf[SECTION_MAX];
	
	struct SectionTime {
	
		Uint64 total;
		Uint64 last_frame;
		Uint64 frame;
		
		SectionTime() { total=last_frame=frame=0; }
	};
	
	static SectionTime section_time[SECTION_MAX];
	
	
	static Measure* create_area(Section p_section,String p_area);
	
public:

	SCRIPT_BIND_BEGIN;

	static void begin_area(Section p_section,String p_area);
	static void end_area(Section p_section,String p_area);
	static Uint64 get_area_frame_time(Section p_section,String p_area);
	static Uint64 get_area_total_time(Section p_section,String p_area);
	static Color get_area_color(Section p_section,String p_area);
		
	static void get_area_list(Section p_section,List<String> *p_areas);
		
	static void begin_section_frame(Section p_section);
	static void end_section_frame(Section p_section);
	
	static Uint64 get_section_frame_time(Section p_section);
	static Uint64 get_section_total_time(Section p_section);

	static Uint64 get_area_average(Section p_section,String p_area);
	static Uint64 get_area_min(Section p_section,String p_area);
	static Uint64 get_area_max(Section p_section,String p_area);

	static void print();


};

SCRIPT_BIND_END;

#define MEASURE_BEGIN_P(m_area) PerformanceMetrics::begin_area(PerformanceMetrics::SECTION_PROCESS,m_area);
#define MEASURE_END_P(m_area) PerformanceMetrics::end_area(PerformanceMetrics::SECTION_PROCESS,m_area);

#define MEASURE_BEGIN_D(m_area) PerformanceMetrics::begin_area(PerformanceMetrics::SECTION_DRAW,m_area);
#define MEASURE_END_D(m_area) PerformanceMetrics::end_area(PerformanceMetrics::SECTION_DRAW,m_area);

#endif

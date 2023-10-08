//
// C++ Implementation: performance_metrics
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "performance_metrics.h"
#include "os/time.h"
#include "print_string.h"

PerformanceMetrics::SectionTime PerformanceMetrics::section_time[SECTION_MAX];

Table<String,PerformanceMetrics::Measure> PerformanceMetrics::section_perf[SECTION_MAX];

PerformanceMetrics::Measure* PerformanceMetrics::create_area(Section p_section,String p_area) {

	section_perf[p_section][p_area]=Measure();
	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,NULL);
	
	/* make a color from the name */
	for (int i=0;i<p_area.length();i++) {
	
		m->color.r+=p_area[i];
	}
	m->color.g=m->color.r+0x55;
	m->color.b=m->color.g+0x55;
	
	return m;
}

void PerformanceMetrics::begin_area(Section p_section,String p_area) {

	ERR_FAIL_INDEX(p_section,SECTION_MAX);
		
	Measure *m = section_perf[p_section].getptr(p_area);
	if (!m) {
	
		m=create_area(p_section,p_area);
	}
	
	ERR_FAIL_COND(m->from!=0); // use begin/end properly
	m->from=OSTime::get_singleton()->get_ticks_usec();
	
	
}

void PerformanceMetrics::end_area(Section p_section,String p_area) {

	ERR_FAIL_INDEX(p_section,SECTION_MAX);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND(!m);
	ERR_FAIL_COND(m->from==0);
	m->frame+=OSTime::get_singleton()->get_ticks_usec()-m->from;
	m->from=0;
	
	Uint64 total = (m->average * m->count) + m->frame;
	++m->count;
	m->average = total / m->count;
	
	if (m->frame > m->max) m->max = m->frame;
	if (m->frame < m->min) m->min = m->frame;
}
Uint64 PerformanceMetrics::get_area_frame_time(Section p_section,String p_area) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,0);
	
	return m->last_frame;

}

Uint64 PerformanceMetrics::get_area_average(Section p_section,String p_area) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,0);

	Uint64 ret = m->average;
	m->average = 0;
	m->count = 0;
	
	return ret;
}

Uint64 PerformanceMetrics::get_area_min(Section p_section,String p_area) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,0);

	Uint64 ret = m->min;
	m->min = ~(Uint64)0;
	
	return ret;
};

Uint64 PerformanceMetrics::get_area_max(Section p_section,String p_area) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,0);

	Uint64 ret = m->max;
	m->max = 0;
	
	return ret;
};

Uint64 PerformanceMetrics::get_area_total_time(Section p_section,String p_area) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,0);
	return m->total;

}
	
Color PerformanceMetrics::get_area_color(Section p_section,String p_area) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	Measure *m = section_perf[p_section].getptr(p_area);
	ERR_FAIL_COND_V(!m,Color(0,0,0));
	return m->color;

}
	
void PerformanceMetrics::get_area_list(Section p_section,List<String> *p_areas) {

	ERR_FAIL_INDEX(p_section,SECTION_MAX);
	
	const String *k=NULL;
	while( (k=section_perf[p_section].next(k)) ) {
	
		p_areas->push_back(*k);
	}

}
	
void PerformanceMetrics::begin_section_frame(Section p_section) {

	ERR_FAIL_INDEX(p_section,SECTION_MAX);
	section_time[p_section].frame=OSTime::get_singleton()->get_ticks_usec();
}
	
void PerformanceMetrics::end_section_frame(Section p_section) {

	ERR_FAIL_INDEX(p_section,SECTION_MAX);

	section_time[p_section].last_frame=OSTime::get_singleton()->get_ticks_usec()-section_time[p_section].frame;
	section_time[p_section].total+=section_time[p_section].last_frame;
	
	const String *k=NULL;
	while( (k=section_perf[p_section].next(k)) ) {
	
		Measure *m = section_perf[p_section].getptr(*k);
		m->last_frame=m->frame;
		m->total+=m->frame;
		m->frame=0;
	}

}
	
Uint64 PerformanceMetrics::get_section_frame_time(Section p_section) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);

	return section_time[p_section].last_frame;
}

Uint64 PerformanceMetrics::get_section_total_time(Section p_section) {

	ERR_FAIL_INDEX_V(p_section,SECTION_MAX,0);
	
	return section_time[p_section].total;

}
		
void PerformanceMetrics::print() {


}

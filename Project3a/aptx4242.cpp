/***************************************************
 aptx4242
 **************************************************/
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Random.h"
#include "aptx4242.h"
#include "Window.h"

using namespace std;
using String = std::string;

Initializer<aptx4242> __aptx4242_initializer;

//int aptx4242::total_num = 0 ; // sorry, no statics allowed

String aptx4242::player_name(void) const {
    return "aptx4242";
}

String aptx4242::species_name(void) const
{
	//if(health()>4) return "Algae";
	return "aptx4242";
}

Action aptx4242::encounter(const ObjInfo& info)
{
	const String fav_food = "Algae";
        if (info.species == species_name()) {
                /* don't be cannibalistic */
                //set_course(info.bearing + M_PI/4.0);
                return LIFEFORM_IGNORE;
        }
        else {
                hunt_event->cancel();
                SmartPointer<aptx4242> me{this};
		hunt_event = new Event(0.0, [me] (void) { me->hunt(); });
                return LIFEFORM_EAT;
        }

}

void aptx4242::initialize(void)
{
	LifeForm::add_creator(aptx4242::create, "aptx4242");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
aptx4242::aptx4242()
{
    course_changed=0;
    perc_dist=40.0;
    last_block=-1;
    hunt_event = Nil<Event>();
    SmartPointer<aptx4242> me{this};
    (void) new Event(0.0, [me] (void) { me->live();});
}


aptx4242::~aptx4242()
{
}

void aptx4242::spawn(void)
{
	aptx4242* child = new aptx4242;
	reproduce(child);
}


Color aptx4242::my_color(void) const
{
  	return MAGENTA;
}

LifeForm* aptx4242::create(void)
{
	aptx4242* res = Nil<aptx4242>();
	res = new aptx4242;
	res->display();
	return res;
}


/*
 * this event is only called once, when the object is "born"
 *
 * I don't want to call "set_course" and "set_speed" from the constructor
 * 'cause OBJECTS ARE NOT ALIVE WHEN THEY ARE CONSTRUCTED (remember!)
 * so... in the constructor I schedule this event to happen in 0 time
 * units
 */
void aptx4242::live(void)
{
	set_course(drand48() * 2.0 * M_PI);
	set_speed(2 + 5.0 * drand48());
	SmartPointer<aptx4242> me{this};
	hunt_event = new Event(5.0, [me] (void) { me->hunt();});
}

void aptx4242::hunt(void)
{
    const String fav_food = "Algae";
    const String food = "Craig";   
    const String myid = "aptx4242";
 
    hunt_event = Nil<Event>();
    if (health() == 0) { return; }

    block[0]=0;block[1]=0;block[2]=0;block[3]=0;

      perc_dist=health()*100*0.02*20;
      if(perc_dist<40) perc_dist=40;
      else if(perc_dist>100) perc_dist=99;
	std::cout<<"perc_dist="<<perc_dist<<"\n";

    ObjList prey = perceive(perc_dist);
    
    double best_d = HUGE, best_d2 = HUGE;
    double angle_temp=-999;
    double ave_angle=0;
    double health_temp=0;
    double dist_temp=0;
    int count = 0 , count1=0, count2=0;
    for (ObjInfo i : prey) {
	which_block(i.bearing);
	if(i.species != myid && i.species != fav_food)
         { count++ ; health_temp+=i.health;}
	else if(i.species == myid)
	 { count1++ ;}
	bool eat_ani = i.species != myid && i.species != fav_food &&
			i.health < health() &&
			i.their_speed < get_speed();
	bool want_eat = i.species == fav_food || eat_ani;

        if (want_eat) {
		count2++;
            if (best_d > i.distance) {
                set_course(i.bearing);
		//set_speed(i.distance/10);
                best_d = i.distance;
            }
        }
	else{
	    if (best_d2 > i.distance) {
                angle_temp = i.bearing;
                best_d2 = i.distance;
            }
	}
    }

	int togo;
        togo=block_to_go();
	if(togo>0) last_block=togo; 
    if(best_d2<best_d-10){
	set_course(togo*M_PI/2-M_PI/4);
	//set_speed(best_d2/10);
    }

    if(count>0) health_temp=health_temp/count;

    if(best_d == HUGE){
	course_changed++;
        if(course_changed == 1){
		ObjList prey2 = perceive(perc_dist+health()*100*0.02*20);
		double best_d3=HUGE, best_d4=HUGE;
		int count4=0,count5=0;
		double angle_bad=-999;;
		for (ObjInfo i : prey2) {
		 which_block(i.bearing);
		 bool eat_ani = i.species != myid && i.species != fav_food &&
                        i.health < health() &&
                        i.their_speed < get_speed();
        	 bool want_eat = i.species == fav_food || eat_ani;
		 count4++;
           	 if (best_d3 > i.distance && want_eat) {
                   set_course(i.bearing);
                   best_d3 = i.distance;
		   count5++;
		   last_block=at_which_block(i.bearing);
            	 }
		 else if (best_d4 > i.distance){
		   best_d4=i.distance;
		   angle_bad = i.bearing;
		 }
        	}
		int new_togo=block_to_go();
		if(new_togo>0) last_block=new_togo;
	    if(count4==0){
		double angle_haha;
		if(last_block>0)
		   angle_haha= last_block*M_PI/2-M_PI/4;
		else
		   angle_haha = get_course() + M_PI/2.0;
              set_course(angle_haha) ;
	      set_speed(get_speed()/2.0);
	    }
	    else{
		if(angle_bad>0 && count5==0){
			set_course(last_block*M_PI/2-M_PI/4);
		}
	    }
        }
	else if(course_changed>1){
	    course_changed = 0;
		ObjList prey3 = perceive(perc_dist+health()*100*0.02*20);
		int count_haha=0;
		for (ObjInfo i : prey3) {
			count_haha++;
		}
		if(count_haha==0){
	    set_course(get_course() + M_PI) ;
	    set_speed(get_speed()*2.0);
	    last_block=at_which_block(get_course() + M_PI);}
		else{
		set_course(get_course()) ;
            set_speed(get_speed());
		last_block=at_which_block(get_course());
		}
	}
    }
    SmartPointer<aptx4242> me{this};
    hunt_event = new Event(10.0, [me] (void) { me->hunt();});

    if(count>0){ 
      if (health() >= health_temp*2) spawn();
    }
    else{
      if (health() >= 3.0 /*&& count1/(M_PI*perc_dist*perc_dist) < 0.5*/) spawn();
    }

}

void aptx4242::which_block(double angle)
{
    if(angle<=0)
	block[3] ++;
    else{
	if(angle<=M_PI/2)
	   block[0]++;
	else if(angle<=M_PI)
	   block[1]++;
	else if(angle<=M_PI*3/2)
	   block[2]++;
    }
    return;
}

int aptx4242::block_to_go(void)
{
   int togo;
   int num=0;
   for(int i=0; i<4; i++)
   {
	if(num<block[i])
	{ togo=i+1; num=block[i];}

   }
if(num>0)
  return togo;
else
  return -1;
}

int aptx4242::at_which_block(double angle)
{
	int togo;
	if(angle>0)
	{
		togo=(int)floor(angle/(M_PI/2))+1;
	}
	else
	{
		togo=4-(int)floor(-angle/(M_PI/2));
	}
return togo;
}
